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

    if (!started) {
        return;
    }

    if (event == EVENT_APOGEE) {
        digitalWrite(PIN_PILOT, HIGH);
        wait(100);
        digitalWrite(PIN_PILOT, LOW);

        Dispatcher* dispatcher = Dispatcher::createInstance();
        dispatcher->dispatch(EVENT_PILOT_FIRED, data);
    }
}

