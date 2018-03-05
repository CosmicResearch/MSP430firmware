/*
    Bondar avionics
    Copyright (C) 2017  AssociaciÃ³ Cosmic Research
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

#include "Bondar.h"
#include "Poller.h"
#include "printers/DebugPrinter.h"
#include "GPS.h"
#include "SensADC.h"
#include "actuators/PilotActuator.h"

Poller* poller;
Dispatcher* dispatcher;
DebugPrinter* printer = new DebugPrinter();
PilotActuator mainActuator = PilotActuator();

Resource resx = Resource(ARBITER_ADC);
Resource resy = Resource(ARBITER_ADC);
Resource resz = Resource(ARBITER_ADC);

SensADC adcx = SensADC(&resx,
        ADC_CHANNEL_3,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcy = SensADC(&resy,
        ADC_CHANNEL_2,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcz = SensADC(&resz,
        ADC_CHANNEL_1,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);

Resource magResource = Resource(ARBITER_USCIB_0);
Resource barResource = Resource(ARBITER_USCIB_0);
SensTimer timer = SensTimer(TMILLIS);

const float_t SEA_LEVEL_PRESSURE = 101325.0f;

GPS gps = GPS(&Serial0, 9600);
Accelerometer accel = Accelerometer(&adcx, &adcy, &adcz);

void setup(void) {
#ifdef __DEBUG__
    Debug.begin();
    Debug.println("====STARTING====");
#endif
    dispatcher = Dispatcher::createInstance();
    /*TODO: subscribe actuators and printers to the dispatcher*/
    dispatcher->subscribe(EVENT_READ_GPS, printer);
    dispatcher->subscribe(EVENT_READ_ACCELEROMETER, printer);
    dispatcher->subscribe(EVENT_SENSOR_INIT, printer);
    dispatcher->subscribe(EVENT_ERROR_SENSOR_READ, printer);
    dispatcher->subscribe(EVENT_APOGEE, &mainActuator);
    poller = Poller::createInstance(dispatcher, POLL_INTERVAL);
    /*TODO: attach sensors to the poller*/
    poller->attachGPS(&gps); //Baudrate should be 115200 and 10Hz freq
    poller->attachAccelerometer(&accel);
    poller->start();
}

// Nothing to do here
void loop(void) {}
