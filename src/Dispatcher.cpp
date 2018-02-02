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
}

Dispatcher* Dispatcher::createInstance() {
    if (Dispatcher::instance == NULL) {
        Dispatcher::instance = new Dispatcher();
    }
    return Dispatcher::instance;
}

/**
 * Subscribe methods for actuators, printers and middleware
 */

void Dispatcher::subscribe(int event, Actuator* actuator) {
    if (this->actuators.find(event) == this->actuators.end()) {
        this->actuators[event] = std::vector<Actuator*>(5);
    }
    this->actuators[event].push_back(actuator);
}

void Dispatcher::subscribe(int event, Printer* printer) {
    if (this->printers.find(event) == this->printers.end()) {
        this->printers[event] = std::vector<Printer*>(5);
    }
    this->printers[event].push_back(printer);
}

void Dispatcher::subscribe(int event, Middleware* middleware) {
    this->middleware[event] = middleware;
}


/**
 * Dispatch methods
 */

void Dispatcher::dispatch(void* eventStruct) {
    event_t eventData = *((event_t*)eventStruct);
    Dispatcher::instance->dispatch(eventData.event, eventData.data);
}

void Dispatcher::dispatch(Event event, void* data) {
    if (this->middleware.find(event) != this->middleware.end()) {
        data = this->middleware[event].execute(event, data);
    }
    if (this->printers.find(event) != this->printers.end()) {
        for (int i = 0; i < this->printers[event].size(); ++i) {
            this->printers[event][i]->print(event, data);
        }
    }
    if (this->actuators.find(event) != this->actuators.end()) {
        for (int i = 0; i < this->actuators[event].size(); ++i) {
            this->actuators[event][i]->actuate(event, data);
        }
    }
}