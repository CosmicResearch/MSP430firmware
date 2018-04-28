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

#include "MainActuator.h"

MainActuator::MainActuator() {
    this->started = false;
    this->apogee = false;
}

error_t MainActuator::start() {
    if (this->started) {
        return EALREADY;
    }

    pinMode(PIN_MAIN, OUTPUT);

    this->started = true;
    return SUCCESS;
}

error_t MainActuator::stop() {
    this->started = false;
    return SUCCESS;
}

bool MainActuator::isStarted() {
    return this->started;
}

void MainActuator::actuate(Event event, void* data) {

    switch(event) {

        case EVENT_APOGEE: {
            this->apogee = true; //We don't want to open the chute before the apogee
            break;
        }

        case EVENT_READ_KALMAN: {

            kalman_data_t* kalman_data = (kalman_data_t*) data;

            if (apogee && kalman_data->altitude <= ALTITUDE_MAIN) {

                bool mainOpen = this->openMain();
                if (!mainOpen) {
                    Dispatcher::instance().dispatch(EVENT_ERROR_MAIN, data); // :| the chute probably won't open
                }
                else {
                    Dispatcher::instance().dispatch(EVENT_MAIN_FIRED, data);
                }

            }

            break;
        }

    }

}

bool MainActuator::openMain() {
    digitalWrite(PIN_MAIN, HIGH);
    wait(100);
    digitalWrite(PIN_MAIN, LOW);
    return true;
}

