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
#include "../src/actuators/MainActuator.h"
#include "../src/actuators/PilotActuator.h"
#include <string>

std::unordered_map<uint8_t, bool> readings;

const char* pinName(int p) {
    switch(p) {
        
        case PIN_MAIN:
            return "PIN_MAIN";
        case PIN_MAIN_ALT:
            return "PIN_MAIN_ALT";
        case PIN_PILOT:
            return "PIN_PILOT";
        case PIN_PILOT_ALT:
            return "PIN_PILOT_ALT";
        case PIN_MAIN_CONTINUITY_IN:
            return "PIN_MAIN_CONTINUITY_IN";
        case PIN_MAIN_CONTINUITY_IN_ALT:
            return "PIN_MAIN_CONTINUITY_IN_ALT";
        case PIN_MAIN_CONTINUITY_OUT:
            return "PIN_MAIN_CONTINUITY_OUT";
        case PIN_MAIN_CONTINUITY_OUT_ALT:
            return "PIN_MAIN_CONTINUITY_OUT_ALT";
        case PIN_PILOT_CONTINUITY_IN:
            return "PIN_PILOT_CONTINUITY_IN";
        case PIN_PILOT_CONTINUITY_IN_ALT:
            return "PIN_PILOT_CONTINUITY_IN_ALT";
        case PIN_PILOT_CONTINUITY_OUT:
            return "PIN_PILOT_CONTINUITY_OUT";
        case PIN_PILOT_CONTINUITY_OUT_ALT:
            return "PIN_PILOT_CONTINUITY_OUT_ALT";
        default: {
            std::string s = std::to_string(p);
            return s.c_str();
        }
    }
}

error_t postTask(void (*function)(void*), void *param) {
    function(param);
    return SUCCESS;
}

void pinMode(uint8_t pin, uint8_t mode) {
    if (mode == INPUT) {
        std::cout << "Set pin " << pinName(pin) << " as INPUT" << std::endl;
    }
    else {
        std::cout << "Set pin " << pinName(pin) << " as OUTPUT" << std::endl;
    }
}

void digitalWrite(uint8_t pin, uint8_t value) {
    if (value == HIGH) {
        std::cout << "Set pin " << pinName(pin) << " as " << "HIGH" << std::endl;
    }
    else {
        std::cout << "Set pin " << pinName(pin) << " as " << "LOW" << std::endl;
    }
    
}

bool digitalRead(uint8_t pin) {

    if (readings.find(pin) != readings.end()) {

        const char* what = (readings[pin])?"HIGH":"LOW";

        std::cout << "Reading from pin " << pinName(pin) <<  " " << what << std::endl;

        return readings[pin];
    }

    std::cout << "Reading from pin " << pinName(pin) << " HIGH (not predefined)" << std::endl;

    return true;
}

void defineReadingForPin(uint8_t pin, bool reading) {
    readings[pin] = reading;
}

void wait(uint16_t time) {
    //Don't
}