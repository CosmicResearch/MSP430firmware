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

#ifndef BONDAR_MAIN_ACTUATOR
#define BONDAR_MAIN_ACTUATOR

#define PIN_MAIN 13
#define PIN_MAIN_ALT 14

#define PIN_MAIN_CONTINUITY_OUT 15
#define PIN_MAIN_CONTINUITY_IN 16

#define PIN_MAIN_CONTINUITY_OUT_ALT 17
#define PIN_MAIN_CONTINUITY_IN_ALT 18

#define ALTITUDE_MAIN 1000

#include "../Actuator.h"
#include "../Dispatcher.h"

class MainActuator : public Actuator {

private:

    bool apogee;

    bool started;

    bool checkContinuity(uint8_t pinOUT, uint8_t pinIN);

    bool openMain();

public:
    MainActuator();

    virtual error_t start();

    virtual error_t stop();

    virtual bool isStarted();

    virtual void actuate(Event event, void* data);
};

#endif /* SRC_ACTUATORS_MAINACTUATOR_H_ */
