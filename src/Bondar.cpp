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

#include <printers/SDRawPrinter.h>
#include "Bondar.h"
#include "Poller.h"
#include "printers/DebugPrinter.h"
#include "actuators/PilotActuator.h"
#include "actuators/MainActuator.h"

Poller* poller;
Dispatcher* dispatcher;
DebugPrinter* printer = new DebugPrinter(0);
PilotActuator pilotActuator = PilotActuator();
MainActuator mainActuator = MainActuator();

Resource accelRes = Resource(ARBITER_ADC);

SensADC adcx = SensADC(&accelRes,
        ADC_CHANNEL_4,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcy = SensADC(&accelRes,
        ADC_CHANNEL_3,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcz = SensADC(&accelRes,
        ADC_CHANNEL_2,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);

Resource magResource = Resource(ARBITER_USCIB_0);
Resource gyroResource = Resource(ARBITER_USCIB_0);
SensTimer timer = SensTimer(TMILLIS);

const float_t SEA_LEVEL_PRESSURE = 101325.0f;

Resource sd_resource = Resource(ARBITER_USCIB_1);
SensSDVolume VOLUME(&Spi1, &sd_resource);

SDRawPrinter* sdPrinter = new SDRawPrinter(&VOLUME);

GPS gps = GPS(&Serial0, 9600);
Accelerometer accel = Accelerometer(&adcx, &adcy, &adcz);
Magnetometer mag = Magnetometer(&Spi0, &magResource, M_SCALE_2GS, M_ODR_125);
Gyroscope gyro = Gyroscope(&Spi0, &gyroResource, LSM9DS0_GYROSCALE_245DPS, G_ODR_95_BW_125);

void setup(void) {
#ifdef __DEBUG__
    Debug.begin();
    Debug.println("====STARTING====");
#endif
    dispatcher = Dispatcher::createInstance();
    /*TODO: subscribe actuators and printers to the dispatcher*/
    //dispatcher->subscribe(EVENT_READ_GPS, printer);
    //dispatcher->subscribe(EVENT_READ_ACCELEROMETER, printer);
    /*dispatcher->subscribe(EVENT_READ_GYROSCOPE, printer);
    dispatcher->subscribe(EVENT_READ_MAGNETOMETER, printer);
    dispatcher->subscribe(EVENT_READ_KALMAN, printer);
    dispatcher->subscribe(EVENT_READ_ORIENTATION, printer);
    dispatcher->subscribe(EVENT_SENSOR_INIT, printer);
    dispatcher->subscribe(EVENT_ERROR_SENSOR_READ, printer);*/

    dispatcher->subscribe(EVENT_READ_GPS, sdPrinter);
    dispatcher->subscribe(EVENT_READ_ACCELEROMETER, sdPrinter);
    dispatcher->subscribe(EVENT_READ_GYROSCOPE, sdPrinter);
    dispatcher->subscribe(EVENT_READ_MAGNETOMETER, sdPrinter);
    dispatcher->subscribe(EVENT_READ_KALMAN, sdPrinter);
    dispatcher->subscribe(EVENT_READ_ORIENTATION, sdPrinter);
    dispatcher->subscribe(EVENT_SENSOR_INIT, sdPrinter);
    dispatcher->subscribe(EVENT_ERROR_SENSOR_READ, sdPrinter);


    dispatcher->subscribe(EVENT_APOGEE, &pilotActuator);
    dispatcher->subscribe(EVENT_APOGEE, &mainActuator);

    poller = Poller::createInstance(dispatcher, POLL_INTERVAL);
    /*TODO: attach sensors to the poller*/
    poller->attachGPS(&gps); //Baudrate should be 115200 and 10Hz freq
    poller->attachAccelerometer(&accel);
    poller->attachGyroscope(&gyro);
    poller->attachMagnetometer(&mag);
    poller->start();
}

// Nothing to do here
void loop(void) {}
