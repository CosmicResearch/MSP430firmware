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
#include <printers/DebugPrinter.h>
#include "Bondar.h"
#include "Poller.h"
#include "actuators/PilotActuator.h"
#include "actuators/MainActuator.h"
#include "middleware/ApogeeMiddleware.h"
#include "middleware/SensorFusionMiddleware.h"

PilotActuator pilotActuator = PilotActuator();
MainActuator mainActuator = MainActuator();
//ApogeeMiddleware apogeeMiddleware = ApogeeMiddleware();
//SensorFusionMiddleware sensorFusion = SensorFusionMiddleware();

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

Resource sd_resource = Resource(ARBITER_USCIB_1);
SensSDVolume VOLUME(&Spi1, &sd_resource);

SDRawPrinter sdPrinter = SDRawPrinter(&VOLUME);
DebugPrinter debugPrinter(1);

GPS gps = GPS(&Serial0, 9600);
Accelerometer accel = Accelerometer(&adcx, &adcy, &adcz);
Magnetometer mag = Magnetometer(&Spi0, &magResource, M_SCALE_2GS, M_ODR_125);
Gyroscope gyro = Gyroscope(&Spi0, &gyroResource, LSM9DS0_GYROSCALE_245DPS, G_ODR_95_BW_125);

void setup(void) {

#ifdef __DEBUG__
    Debug.begin();
    Debug.println("START");
#endif

    Dispatcher::instance().subscribe(EVENT_READ_GPS, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_GYROSCOPE, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_MAGNETOMETER, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_KALMAN, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_ORIENTATION, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_APOGEE, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_MAIN_FIRED, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_PILOT_FIRED, &debugPrinter);
    Dispatcher::instance().subscribe(EVENT_LIFTOFF, &debugPrinter);


    Dispatcher::instance().subscribe(EVENT_APOGEE, &pilotActuator);
    Dispatcher::instance().subscribe(EVENT_APOGEE, &mainActuator);

    //Dispatcher::instance().subscribe(EVENT_READ_GPS, &apogeeMiddleware);
    //Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, &apogeeMiddleware);

    //Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, &sensorFusion);
    //Dispatcher::instance().subscribe(EVENT_READ_MAGNETOMETER, &sensorFusion);

    /*TODO: attach sensors to the poller*/
    Poller::instance().attachGPS(&gps); //Baudrate should be 115200 and 10Hz freq
    Poller::instance().attachAccelerometer(&accel);
    Poller::instance().attachGyroscope(&gyro);
    Poller::instance().attachMagnetometer(&mag);
    Poller::instance().start();

#ifdef __DEBUG__
        Debug.println("setup end");
#endif
}

// Nothing to do here
void loop(void) {}
