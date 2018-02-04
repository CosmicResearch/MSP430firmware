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

#ifndef BONDAR_DISPATHCER
#define BONDAR_DISPATHCER

#include <cstdint>
#include <unordered_map>
#include <vector>
#include "Actuator.h"
#include "Printer.h"
#include "Middleware.h"
#include "events.h"

class Dispatcher {

private:

    static Dispatcher* instance;

    /**
     * At the moment, just one Middleware per event is allowed.
     */

    std::unordered_map<Event event, std::vector<Actuator*>> actuators;
    std::unordered_map<Event event, std::vector<Printer*>> printers;
    std::unordered_map<Event event, Middleware*> middleware;

    void dispatch(Event event, void* data);

    Dispatcher();

    bool started;

    static void printerTask(void* eventStruct);
    static void actuatorTask(void* eventStruct);

public:

    static Dispatcher* createInstance();

    error_t start();

    /**
     * Events are defined in the events.h file
     */

    void subscribe(Event event, Actuator* actuator);
    void subscribe(Event event, Printer* printer);
    void subscribe(Event event, Middleware* middleware);

    static void dispatch(void* eventStruct);

};

#endif