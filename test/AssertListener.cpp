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

#include "AssertListener.h"

AssertListener::AssertListener() {

    for (int i = 0; i < N_MAX_EVENTS; ++i) {
        this->numberEvents[i] = 0;
    }

    this->started = false;

}

error_t AssertListener::start() {
    this->started = true;
    return SUCCESS;
}

error_t AssertListener::stop() {
    this->started = false;
    return SUCCESS;
}

bool AssertListener::isStarted() {
    return this->started;
}

void AssertListener::actuate(Event e, void* data) {

    ++(this->numberEvents[e]);

}

int AssertListener::getNumEvent(Event e) {
    return this->numberEvents[e];
}