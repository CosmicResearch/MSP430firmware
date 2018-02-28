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

#include <cassert>
#include "../src/Dispatcher.h"
#include "../src/events.h"
#include "../src/actuators/MainActuator.h"
#include "../src/actuators/PilotActuator.h"
#include "Senscape.h"
#include "TestPrinter.h"
#include "AssertListener.h"

AssertListener assertListener;

void assertEvent (Event e, int expected) {

    int actual = assertListener.getNumEvent(e);
    assert(expected == actual);

}

Dispatcher* init() {

    Dispatcher::reset();

    Dispatcher* dispatcher = Dispatcher::createInstance();

    PilotActuator* pilotActuator = new PilotActuator();
    MainActuator* mainActuator = new MainActuator();
    TestPrinter* testPrinter = new TestPrinter();

    assertListener = AssertListener();

    dispatcher->subscribe(EVENT_APOGEE, pilotActuator);
    dispatcher->subscribe(EVENT_APOGEE, mainActuator);
    dispatcher->subscribe(EVENT_READ_KALMAN, mainActuator);

    dispatcher->subscribe(EVENT_APOGEE, testPrinter);
    dispatcher->subscribe(EVENT_READ_KALMAN, testPrinter);
    dispatcher->subscribe(EVENT_MAIN_FIRED, testPrinter);
    dispatcher->subscribe(EVENT_PILOT_FIRED, testPrinter);
    dispatcher->subscribe(EVENT_ERROR_MAIN, testPrinter);
    dispatcher->subscribe(EVENT_ERROR_PILOT, testPrinter);

    dispatcher->subscribe(EVENT_APOGEE, &assertListener);
    dispatcher->subscribe(EVENT_READ_KALMAN, &assertListener);
    dispatcher->subscribe(EVENT_MAIN_FIRED, &assertListener);
    dispatcher->subscribe(EVENT_PILOT_FIRED, &assertListener);
    dispatcher->subscribe(EVENT_ERROR_MAIN, &assertListener);
    dispatcher->subscribe(EVENT_ERROR_PILOT, &assertListener);

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

    assertEvent(EVENT_ERROR_MAIN, 0);
    assertEvent(EVENT_ERROR_PILOT, 0);

    assertEvent(EVENT_MAIN_FIRED, 1);
    assertEvent(EVENT_PILOT_FIRED, 1);

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

    assertEvent(EVENT_ERROR_MAIN, 1);
    assertEvent(EVENT_ERROR_PILOT, 1);

    assertEvent(EVENT_MAIN_FIRED, 0);
    assertEvent(EVENT_PILOT_FIRED, 0);

}

void testPilotSuccessAndMainFail() {
    std::cout << "==========================================================" << std::endl;
    std::cout << "=Starting test with Pilot with SUCCESS and Main with FAIL=" << std::endl;
    std::cout << "==========================================================" << std::endl;

    Dispatcher* dispatcher = init();

    defineReadingForPin(PIN_MAIN_CONTINUITY_IN, true);
    defineReadingForPin(PIN_MAIN_CONTINUITY_IN_ALT, true);

    defineReadingForPin(PIN_PILOT_CONTINUITY_IN, false);
    defineReadingForPin(PIN_PILOT_CONTINUITY_IN_ALT, false);

    std::cout << "Dispatch EVENT_APOGEE" << std::endl;
    dispatcher->dispatch(EVENT_APOGEE, new int(1600));
    std::cout << "Dispatch EVENT_READ_KALMAN with ALTITUDE_MAIN" << std::endl;
    dispatcher->dispatch(EVENT_READ_KALMAN, new int(ALTITUDE_MAIN));

    assertEvent(EVENT_ERROR_MAIN, 1);
    assertEvent(EVENT_ERROR_PILOT, 0);

    assertEvent(EVENT_MAIN_FIRED, 0);
    assertEvent(EVENT_PILOT_FIRED, 1);
}

void testPilotFailSuccessAndMainFail() {
    std::cout << "============================================================================" << std::endl;
    std::cout << "=Starting test with Pilot with one FAIL and one SUCCESS and Main with ERROR=" << std::endl;
    std::cout << "============================================================================" << std::endl;

    Dispatcher* dispatcher = init();

    defineReadingForPin(PIN_MAIN_CONTINUITY_IN, true);
    defineReadingForPin(PIN_MAIN_CONTINUITY_IN_ALT, true);

    defineReadingForPin(PIN_PILOT_CONTINUITY_IN, true);
    defineReadingForPin(PIN_PILOT_CONTINUITY_IN_ALT, false);

    std::cout << "Dispatch EVENT_APOGEE" << std::endl;
    dispatcher->dispatch(EVENT_APOGEE, new int(1600));
    std::cout << "Dispatch EVENT_READ_KALMAN with ALTITUDE_MAIN" << std::endl;
    dispatcher->dispatch(EVENT_READ_KALMAN, new int(ALTITUDE_MAIN));

    assertEvent(EVENT_ERROR_MAIN, 1);
    assertEvent(EVENT_ERROR_PILOT, 0);

    assertEvent(EVENT_MAIN_FIRED, 0);
    assertEvent(EVENT_PILOT_FIRED, 1);
}

void testPilotFailSuccessAndMainFailSuccess() {
    std::cout << "==============================================================================================" << std::endl;
    std::cout << "=Starting test with Pilot with one FAIL and one SUCCESS and Main with one FAIL and one SUCCESS=" << std::endl;
    std::cout << "===============================================================================================" << std::endl;

    Dispatcher* dispatcher = init();

    defineReadingForPin(PIN_MAIN_CONTINUITY_IN, true);
    defineReadingForPin(PIN_MAIN_CONTINUITY_IN_ALT, false);

    defineReadingForPin(PIN_PILOT_CONTINUITY_IN, true);
    defineReadingForPin(PIN_PILOT_CONTINUITY_IN_ALT, false);

    std::cout << "Dispatch EVENT_APOGEE" << std::endl;
    dispatcher->dispatch(EVENT_APOGEE, new int(1600));
    std::cout << "Dispatch EVENT_READ_KALMAN with ALTITUDE_MAIN" << std::endl;
    dispatcher->dispatch(EVENT_READ_KALMAN, new int(ALTITUDE_MAIN));

    assertEvent(EVENT_ERROR_MAIN, 0);
    assertEvent(EVENT_ERROR_PILOT, 0);

    assertEvent(EVENT_MAIN_FIRED, 1);
    assertEvent(EVENT_PILOT_FIRED, 1);
}

int main() {

    testPilotAndMainSuccess();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

    testPilotAndMainFail();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

    testPilotSuccessAndMainFail();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

    testPilotFailSuccessAndMainFail();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

    testPilotFailSuccessAndMainFailSuccess();

    std::cout << std::endl << "----------------------------------------" << std::endl << std::endl;

}