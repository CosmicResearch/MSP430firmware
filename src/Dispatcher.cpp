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
        }
        this->middleware[i] = NULL;
        this->actuators_index[i] = 0;
        this->printers_index[i] = 0;
    }
    this->started = false;
}

Dispatcher* Dispatcher::createInstance() {
    if (Dispatcher::instance == NULL) {
        Dispatcher::instance = new Dispatcher();
    }
    return Dispatcher::instance;
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
            if (this->actuators[i][j] != NULL) {
                this->actuators[i][j]->start();
            }
            if (this->printers[i][j] != NULL) {
                this->printers[i][j]->start();
            }
        }
        if (this->middleware[i] != NULL) {
            this->middleware[i]->start();
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
    this->middleware[event] = middleware;
    this->started = false;
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
    if (this->middleware[event] != NULL) {
        data = this->middleware[event]->execute(event, data);
    }
    for (int i = 0; i < N_PER_EVENT; ++i) {
        if (this->actuators[event][i] != NULL) {
            event_t* eventStruct = new event_t;
            eventStruct->event = event;
            eventStruct->data = data;
            eventStruct->listener = (Listener*)this->actuators[event][i];
            postTask(Dispatcher::actuatorTask, eventStruct);
        }
        if (this->printers[event][i] != NULL) {
            event_t* eventStruct = new event_t;
            eventStruct->event = event;
            eventStruct->data = data;
            eventStruct->listener = (Listener*)this->printers[event][i];
            postTask(Dispatcher::printerTask, eventStruct);
        }
    }
}

void Dispatcher::printerTask(void* eventStruct) {
    event_t eventData = *((event_t*) eventStruct);
    ((Printer*)eventData.listener)->print(eventData.event, eventData.data);
    delete ((event_t*)eventStruct)->data;
    delete (event_t*)eventStruct;
}

void Dispatcher::actuatorTask(void* eventStruct) {
    event_t eventData = *((event_t*) eventStruct);
    ((Printer*)eventData.listener)->print(eventData.event, eventData.data);
    delete ((event_t*)eventStruct)->data;
    delete (event_t*)eventStruct;
}
