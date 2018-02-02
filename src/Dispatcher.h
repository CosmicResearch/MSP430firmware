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
#include <Actuator.h>
#include <Printer.h>
#include <Middleware.h>

class Dispatcher {

private:

    static Dispatcher* instance;

    /**
     * At the moment, just one Middleware per event is allowed.
     */

    std::unordered_map<int event, std::vector<Actuator*>> actuators;
    std::unordered_map<int event, std::vector<Printer*>> printers;
    std::unordered_map<int event, Middleware*> middleware;

    Dispatcher();

public:

    static Dispatcher* createInstance();

    /**
     * Events are defined in the events.h file
     */

    void subscribeActuator(int event, Actuator* actuator);
    void subscribePrinter(int event, Printer* printer);
    void subscribeMiddleware(int event, Middleware* middleware);

    void dispatchData(int event, void* data);
    void dispatchNotification(int event);
    void dispatchError(int event);

};

#endif