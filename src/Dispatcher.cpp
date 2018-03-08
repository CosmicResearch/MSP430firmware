/*
    Bondar avionics
    Copyright (C) 2017  Associació Cosmic Research
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Dispatcher.h"

Dispatcher* Dispatcher::instance = NULL;

Dispatcher::Dispatcher() {
    for (int i = 0; i < N_MAX_EVENTS; ++i) {
        for (int j = 0; j < N_PER_EVENT; ++j) {
            this->actuators[i][j] = NULL;
            this->printers[i][j] = NULL;
            this->middleware[i][j] = NULL;
        }
        this->actuators_index[i] = 0;
        this->printers_index[i] = 0;
        this->middleware_index[i] = 0;
    }
    for (int i = 0; i < MAX_NUMBER_OF_TASKS_PER_DEVICE; ++i) {
        processes[i].event = 0;
        processes[i].data = NULL;
        processes[i].n_running = 0;
    }
    this->started = false;
}

Dispatcher* Dispatcher::createInstance() {
    if (Dispatcher::instance == NULL) {
        Dispatcher::instance = new Dispatcher();
    }
    return Dispatcher::instance;
}

void Dispatcher::reset() {
    delete Dispatcher::instance;
    Dispatcher::instance = NULL;
}

/**
 * Starts every Actuator, Printer and Middleware subscribed
 * If any of them returns ERROR, it is deleted and a EVENT_ERROR is dispatched
 */

error_t Dispatcher::start() {

    if (this->started) {
        return EALREADY; //Already started!
    }

    error_t res = SUCCESS;

    for (int i = 0; i < N_MAX_EVENTS; ++i) {
        for (int j = 0; j < N_PER_EVENT; ++j) {
            if (this->actuators[i][j] != NULL && !this->actuators[i][j]->isStarted()) {
                this->actuators[i][j]->start();
            }
            if (this->printers[i][j] != NULL && !this->printers[i][j]->isStarted()) {
                this->printers[i][j]->start();
            }
            if (this->middleware[i][j] != NULL && !this->middleware[i][j]->isStarted()) {
                this->middleware[i][j]->start();
            }
        }
    }

    this->started = true;

    return res;
}

bool Dispatcher::isStarted() {
    return this->started;
}

/**
 * Subscribe methods for actuators, printers and middleware
 */

void Dispatcher::subscribe(Event event, Actuator* actuator) {
    if (event < 0 || event >= N_MAX_EVENTS) {
        return;
    }
    if (actuators_index[event] >= N_PER_EVENT) {
        return;
    }
    uint16_t index = this->actuators_index[event];
    this->actuators[event][index] = actuator;
    this->started = false;
    ++(this->actuators_index[event]);
}

void Dispatcher::subscribe(Event event, Printer* printer) {
    if (event < 0 || event >= N_MAX_EVENTS) {
        return;
    }
    if (printers_index[event] >= N_PER_EVENT) {
        return;
    }
    uint16_t index = this->printers_index[event];
    this->printers[event][index] = printer;
    this->started = false;
    ++(this->printers_index[event]);
}

void Dispatcher::subscribe(Event event, Middleware* middleware) {
    if (event < 0 || event >= N_MAX_EVENTS) {
        return;
    }
    if (middleware_index[event] >= N_PER_EVENT) {
        return;
    }
    uint16_t index = this->middleware_index[event];
    this->middleware[event][index] = middleware;
    this->started = false;
    ++(this->middleware_index[event]);
}

uint8_t Dispatcher::findFirstEventProcess(boolean_t& found) {
    uint8_t process_id;
    found = false;
    uint8_t process_index = 0;
    while (process_index < MAX_NUMBER_OF_TASKS_PER_DEVICE && !found) {
        if (processes[process_index].n_running == 0) {
            process_id = process_index;
            found = true;
        }
        ++process_index;
    }
    return process_index;
}

/**
 * Dispatch methods
 */

void Dispatcher::dispatch(Event event, void* data) {
    if (event < 0 || event >= N_MAX_EVENTS) {
        return;
    }
    if (!this->started) {
        return;
    }

    boolean_t process_available;
    uint8_t process_index = findFirstEventProcess(process_available);

    if (!process_available) {
        return; //We can't spawn new tasks :(
    }

    processes[process_index].event = event;
    processes[process_index].data = data;

    for (int i = 0; i < N_PER_EVENT; ++i) {
        if (this->middleware[event][i] != NULL) {
            event_task_t* eventStruct = new event_task_t;
            eventStruct->process_id = process_index;
            eventStruct->listener = (Listener*)this->middleware[event][i];
            postTask(Dispatcher::middlewareTask, eventStruct);
            ++(processes[process_index].n_running);
        }
        if (this->actuators[event][i] != NULL) {
            event_task_t* eventStruct = new event_task_t;
            eventStruct->process_id = process_index;
            eventStruct->listener = (Listener*)this->actuators[event][i];
            postTask(Dispatcher::actuatorTask, eventStruct);
            ++(processes[process_index].n_running);
        }
        if (this->printers[event][i] != NULL) {
            event_task_t* eventStruct = new event_task_t;
            eventStruct->process_id = process_index;
            eventStruct->listener = (Listener*)this->printers[event][i];
            postTask(Dispatcher::printerTask, eventStruct);
            ++(processes[process_index].n_running);
        }
    }
}

/**
 * Dispatch tasks to make execution async
 */

void Dispatcher::printerTask(void* eventStruct) {
    event_task_t event_data = *((event_task_t*) eventStruct);
    const process_t process_data = Dispatcher::createInstance()->getProcessData(event_data.process_id);
    ((Printer*)event_data.listener)->print(process_data.event, process_data.data);
    Dispatcher::createInstance()->markStoppedRunning(event_data.process_id);
    delete (event_task_t*)eventStruct;
}

void Dispatcher::actuatorTask(void* eventStruct) {
    event_task_t event_data = *((event_task_t*) eventStruct);
    const process_t process_data = Dispatcher::createInstance()->getProcessData(event_data.process_id);
    ((Actuator*)event_data.listener)->actuate(process_data.event, process_data.data);
    Dispatcher::createInstance()->markStoppedRunning(event_data.process_id);
    delete (event_task_t*)eventStruct;
}

void Dispatcher::middlewareTask(void* eventStruct) {
    event_task_t event_data = *((event_task_t*) eventStruct);
    const process_t process_data = Dispatcher::createInstance()->getProcessData(event_data.process_id);
    ((Middleware*)event_data.listener)->execute(process_data.event, process_data.data);
    Dispatcher::createInstance()->markStoppedRunning(event_data.process_id);
    delete (event_task_t*)eventStruct;
}

void Dispatcher::markStoppedRunning(uint8_t process_id) {
    if (processes[process_id].n_running > 0) {
        --processes[process_id].n_running;
        if (processes[process_id].n_running == 0) {
            safeDeleteEventData(processes[process_id].event, processes[process_id].data);
        }
    }
}

const process_t Dispatcher::getProcessData(uint8_t process_id) {
    if (process_id >= MAX_NUMBER_OF_TASKS_PER_DEVICE) {
        return process_t();
    }
    return processes[process_id];
}
