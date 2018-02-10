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

#ifndef BONDAR_PRINTER
#define BONDAR_PRINTER

#include "events.h"
#include "Senscape.h"

class Printer : public Listener {

    /**
     * 
     * Printers print data to the SD card, Serial port or sends it through the radio link.
     * 
     */

public:

    /**
     * Any *expensive* operation should be done here and not in the constructor
     * Returns: SUCCESS if it has been started or any error defined in error.h otherwise
    */
    virtual error_t start() = 0;

    virtual error_t stop() = 0;

    virtual bool isStarted() = 0;

    /**
     * event: one of the event codes defined in events.h
     * data: pointer to data about the event or NULL
    */ 
    virtual void print(Event event, void* data) = 0;

};

#endif
