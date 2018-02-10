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

#include "Actuator.h"
#include "Printer.h"
#include "Middleware.h"
#include "events.h"

class Dispatcher {

private:

    static Dispatcher* instance;

    /**
     * At the moment, just one Middleware per event is allowed. 5 actuators and printers per event
     */

    Actuator* actuators[N_MAX_EVENTS][N_PER_EVENT];
    Printer* printers[N_MAX_EVENTS][N_PER_EVENT];
    Middleware* middleware[N_MAX_EVENTS];

    uint16_t actuators_index[N_MAX_EVENTS];
    uint16_t printers_index[N_MAX_EVENTS];

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

    bool isStarted();

    void dispatch(Event event, void* data);

};

#endif
