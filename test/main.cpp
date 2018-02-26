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

#include "../src/Dispatcher.h"
#include "../src/events.h"
#include "../src/actuators/MainActuator.h"
#include "../src/actuators/PilotActuator.h"
#include "Senscape.h"
#include "TestPrinter.h"

Dispatcher* init() {
    Dispatcher* dispatcher = Dispatcher::createInstance();

    PilotActuator* pilotActuator = new PilotActuator();
    MainActuator* mainActuator = new MainActuator();
    TestPrinter* testPrinter = new TestPrinter();

    dispatcher->subscribe(EVENT_APOGEE, pilotActuator);
    dispatcher->subscribe(EVENT_APOGEE, mainActuator);
    dispatcher->subscribe(EVENT_READ_KALMAN, mainActuator);

    dispatcher->subscribe(EVENT_APOGEE, testPrinter);
    dispatcher->subscribe(EVENT_READ_KALMAN, testPrinter);
    dispatcher->subscribe(EVENT_MAIN_FIRED, testPrinter);
    dispatcher->subscribe(EVENT_PILOT_FIRED, testPrinter);
    dispatcher->subscribe(EVENT_ERROR_MAIN, testPrinter);
    dispatcher->subscribe(EVENT_ERROR_PILOT, testPrinter);

    dispatcher->start();
    return dispatcher;
}

void testPilotAndMainSuccess() {

    std::cout << "==========================================================================" << std::endl;
    std::cout << "=Starting test with Pilot and Main, both should open at the first attempt=" << std::endl;
    std::cout << "==========================================================================" << std::endl;

    Dispatcher* dispatcher = init();

    defineReadingForPin(PIN_MAIN_CONTINUITY_IN, false);
    defineReadingForPin(PIN_PILOT_CONTINUITY_IN, false);

    std::cout << "Dispatch EVENT_APOGEE" << std::endl;
    dispatcher->dispatch(EVENT_APOGEE, new int(1600));
    std::cout << "Dispatch EVENT_READ_KALMAN with ALTITUDE_MAIN" << std::endl;
    dispatcher->dispatch(EVENT_READ_KALMAN, new int(ALTITUDE_MAIN));

}

void testPilotAndMainFail() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "=Starting test with Pilot and Main, both should fail the two attempts=" << std::endl;
    std::cout << "======================================================================" << std::endl;

    Dispatcher* dispatcher = init();

    defineReadingForPin(PIN_MAIN_CONTINUITY_IN, true);
    defineReadingForPin(PIN_MAIN_CONTINUITY_IN_ALT, true);

    defineReadingForPin(PIN_PILOT_CONTINUITY_IN, true);
    defineReadingForPin(PIN_PILOT_CONTINUITY_IN_ALT, true);

    std::cout << "Dispatch EVENT_APOGEE" << std::endl;
    dispatcher->dispatch(EVENT_APOGEE, new int(1600));
    std::cout << "Dispatch EVENT_READ_KALMAN with ALTITUDE_MAIN" << std::endl;
    dispatcher->dispatch(EVENT_READ_KALMAN, new int(ALTITUDE_MAIN));

}

int main() {

    testPilotAndMainSuccess();

    Dispatcher::reset();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

    testPilotAndMainFail();

}