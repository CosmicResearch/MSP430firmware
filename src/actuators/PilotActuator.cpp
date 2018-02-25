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

#include "PilotActuator.h"

PilotActuator::PilotActuator() {

    this->started = false;

}

error_t PilotActuator::start() {

    if (this->started) {
        return EALREADY;
    }

    pinMode(PIN_PILOT, OUTPUT);
    pinMode(PIN_PILOT_ALT, OUTPUT);
    this->started = true;
    return SUCCESS;

}

error_t PilotActuator::stop() {

    this->started = false;
    return SUCCESS;

}

bool PilotActuator::isStarted() {

    return this->started;

}

void PilotActuator::actuate(Event event, void* data) {

    if (event == EVENT_APOGEE) {
        digitalWrite(PIN_PILOT, HIGH);
        wait(100);
        digitalWrite(PIN_PILOT, LOW);

        //check continuity. If we have continuity, the operation has failed and the parachute has not been fired

        if (this->checkContinuity(PIN_PILOT_CONTINUITY_OUT, PIN_PILOT_CONTINUITY_IN)) {

            //The first igniter has continuity, this means it hasn't been fired
            //Let's try to open the pilot with the second igniter

            digitalWrite(PIN_PILOT_ALT, HIGH);
            wait(100);
            digitalWrite(PIN_PILOT_ALT, LOW);

            if (this->checkContinuity(PIN_PILOT_CONTINUITY_OUT_ALT, PIN_PILOT_CONTINUITY_IN_ALT)) {
                Dispatcher* dispatcher = Dispatcher::createInstance();
                dispatcher->dispatch(EVENT_ERROR_PILOT, data); //SHEEET. The pilot has not been fired
            }

        }

        Dispatcher* dispatcher = Dispatcher::createInstance();
        dispatcher->dispatch(EVENT_MAIN_FIRED, data);
    }
}

bool PilotActuator::checkContinuity(uint8_t pinOUT, uint8_t pinIN) {

    bool res;

    pinMode(pinOUT, OUT);
    pinMode(pinIN, INPUT);

    digitalWrite(pinOUT, HIGH);
    wait(10);

    res = digitalRead(pinIN);

    digitalWrite(pinOUT, LOW);

    return res;
}

