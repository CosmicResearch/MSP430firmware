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

#include "Senscape.h"

std::unordered_map<uint8_t, bool> readings;

error_t postTask(void (*function)(void*), void *param) {
    function(param);
    return SUCCESS;
}

void pinMode(uint8_t pin, uint8_t mode) {
    if (mode == INPUT) {
        std::cout << "Set pin " << (int)pin << " as INPUT" << std::endl;
    }
    else {
        std::cout << "Set pin " << (int)pin << " as OUTPUT" << std::endl;
    }
}

void digitalWrite(uint8_t pin, uint8_t value) {
    if (value == HIGH) {
        std::cout << "Set pin " << (int)pin << " as " << "HIGH" << std::endl;
    }
    else {
        std::cout << "Set pin " << (int)pin << " as " << "LOW" << std::endl;
    }
    
}

bool digitalRead(uint8_t pin) {
    if (readings.find(pin) != readings.end()) {
        return readings[pin];
    }
    return true;
}

void defineReadingForPin(uint8_t pin, bool reading) {
    readings[pin] = reading;
}

void wait(uint16_t time) {
    //Don't
}