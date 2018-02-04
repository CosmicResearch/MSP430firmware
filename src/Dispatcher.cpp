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
    this->actuators = std::unordered_map<Event, std::vector<Actuator*> >(16);
    this->printers = std::unordered_map<Event, std::vector<Printer*> >(16);
    this->middleware = std::unordered_map<Event, Middleware*>(16);
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
        return ERROR;
    }

    error_t res = SUCCESS;

    std::vector<event_t> errors_to_dispatch(5);

    for (std::pair<Event, std::vector<Actuator*> > pair_vector_actuators : this->actuators) {
        for (int i = 0; i < vector_actuators.second.size(); ++i) {
            error_t ret = pair_vector_actuators.second[i]->start();
            if (ret != SUCCESS) {
                errors_to_dispatch.push_back({EVENT_ERROR_ACTUATOR_INIT, pair_vector_actuators.second[i]});
                pair_vector_actuators.second.erase(pair_vector_actuators.second.begin() + i);
            }
        }
    }
    for (std::pair<Event, std::vector<Printer*> > pair_vector_printers : this->printers) {
        for (int i = 0; i < pair_vector_printers.second.size(); ++i) {
            error_t ret = pair_vector_printers.second[i]->start();
            if (ret != SUCCESS) {
                errors_to_dispatch.push_back({EVENT_ERROR_PRINTER_INIT, pair_vector_printers.second[i]});
                pair_vector_printers.second.erase(pair_vector_printers.second.begin() + i);
            }
        }
    }
    for (std::pair<Event, Middleware*> pair_middleware : this->middleware) {
        error_t ret = pair_middleware.second->start();
        if (ret != SUCCESS) {
            errors_to_dispatch.push_back({EVENT_ERROR_MIDDLEWARE_INIT, pair_middleware.second});
            this->middleware.erase(pair_middleware.first);
        }
    }

    this->started = true;

    for (int i = 0; i < errors_to_dispatch.size(); ++i) {
        this->dispatch(errors_to_dispatch[i].event, errors_to_dispatch[i].data);
    }

    return res;
}

/**
 * Subscribe methods for actuators, printers and middleware
 */

void Dispatcher::subscribe(Event event, Actuator* actuator) {
    if (this->actuators.find(event) == this->actuators.end()) {
        this->actuators[event] = std::vector<Actuator*>(5);
    }
    this->actuators[event].push_back(actuator);
    this->started = false;
}

void Dispatcher::subscribe(Event event, Printer* printer) {
    if (this->printers.find(event) == this->printers.end()) {
        this->printers[event] = std::vector<Printer*>(5);
    }
    this->printers[event].push_back(printer);
    this->started = false;
}

void Dispatcher::subscribe(Event event, Middleware* middleware) {
    this->middleware[event] = middleware;
    this->started = false;
}


/**
 * Dispatch methods
 */

void Dispatcher::dispatch(void* eventStruct) {
    if (!this->started) {
        return;
    }
    event_t eventData = *((event_t*)eventStruct);
    Dispatcher::instance->dispatch(eventData.event, eventData.data);
    delete eventStruct;
}

void Dispatcher::dispatch(Event event, void* data) {
    if (!this->started) {
        return;
    }
    if (this->middleware.find(event) != this->middleware.end()) {
        data = this->middleware[event].execute(event, data);
    }
    if (this->actuators.find(event) != this->actuators.end()) {
        for (int i = 0; i < this->actuators[event].size(); ++i) {
            event_t* eventStruct = new event_t;
            eventStruct->event = event;
            eventStruct->data = data;
            eventStruct->listener = this->actuators[event][i];
            postTask(Dispatcher::actuatorTask, eventStruct);
        }
    }
    if (this->printers.find(event) != this->printers.end()) {
        for (int i = 0; i < this->printers[event].size(); ++i) {
            event_t* eventStruct = new event_t;
            eventStruct->event = event;
            eventStruct->data = data;
            eventStruct->listener = this->printers[event][i];
            postTask(Dispatcher::printerTask, eventStruct);
        }
    }
}

void Dispatcher::printerTask(void* eventStruct) {
    event_t eventData = *((event_t*) eventStruct);
    ((Printer*)eventData.listener)->print(eventData.event, eventData.data); 
}

void Dispatcher::actuatorTask(void* eventStruct) {
    event_t eventData = *((event_t*) eventStruct);
    ((Printer*)eventData.listener)->actuate(eventData.event, eventData.data); 
}