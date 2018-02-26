/*
 Bondar avionics - tests
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

#include "TestPrinter.h"

TestPrinter::TestPrinter() {
    this->started = false;
}

error_t TestPrinter::start() {
    this->started = true;
    return SUCCESS;
}

error_t TestPrinter::stop() {
    this->started = false;
    return SUCCESS;
}

bool TestPrinter::isStarted() {
    return this->started;
}

void TestPrinter::print(Event e, void* data) {

    if (e == EVENT_APOGEE) {
        std::cout << "APOGEE detected!" << std::endl;
    }
    else if (e == EVENT_MAIN_FIRED) {
        std::cout << "MAIN fired!" << std::endl;
    }
    else if (e == EVENT_PILOT_FIRED) {
        std::cout << "PILOT fired!" << std::endl;
    }
    else if (e == EVENT_ERROR_PILOT) {
        std::cout << "PILOT ERROR!!!" << std::endl;
    }
    else if (e == EVENT_ERROR_MAIN) {
        std::cout << "MAIN ERROR!!!" << std::endl;
    }

}