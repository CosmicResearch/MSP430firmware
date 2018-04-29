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

#ifndef BONDAR_RADIOPRINTER
#define BONDAR_RADIOPRINTER

#include "Bondar.h"
#include "SensSX1272.h"

class RadioPrinter : public Listener {

public:
    RadioPrinter(SensSX1272* radio);

    virtual error_t start();

    virtual error_t stop();

    virtual bool isStarted();

    virtual void execute(Event e, void* data);

private:
    SensSX1272* radio;

    bool started, starting;


};

#endif /* SRC_PRINTERS_RADIOPRINTER_H_ */
