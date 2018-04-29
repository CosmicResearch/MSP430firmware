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

#ifndef BONDAR_DEBUG_PRINTER
#define BONDAR_DEBUG_PRINTER

#include "Bondar.h"

class DebugPrinter : public Listener {

private:

    bool started;
    uint8_t id;

public:

    DebugPrinter(uint8_t id);

    virtual error_t start();

    virtual error_t stop();

    virtual bool isStarted();

    virtual void execute(Event e, void* data);

};

#endif
