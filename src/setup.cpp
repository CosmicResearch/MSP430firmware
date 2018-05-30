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

PilotActuator pilotActuator;
MainActuator mainActuator;
ApogeeMiddleware apogeeMiddleware;
SensorFusionMiddleware sensorFusion;

Resource accelRes = Resource(ARBITER_ADC);

SensADC adcx(&accelRes,
        ADC_CHANNEL_4,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcy(&accelRes,
        ADC_CHANNEL_3,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);
SensADC adcz(&accelRes,
        ADC_CHANNEL_2,
        REFERENCE_AVcc_AVss,
        REFVOLT_LEVEL_NONE,
        SHT_SOURCE_ACLK,
        SHT_CLOCK_DIV_1,
        SAMPLE_HOLD_4_CYCLES,
        SAMPCON_SOURCE_SMCLK,
        SAMPCON_CLOCK_DIV_1);

Resource magResource(ARBITER_USCIB_0);
Resource gyroResource(ARBITER_USCIB_0);
SensTimer timer(TMILLIS);

Resource sd_resource(ARBITER_USCIB_1);
SensSDVolume VOLUME(&Spi1, &sd_resource);

SDRawPrinter sdPrinter(&VOLUME);
//DebugPrinter debugPrinter(1);

GPS gps(&Serial0, 9600);
Accelerometer* accel = Accelerometer::getInstance(&adcx, &adcy, &adcz);
Magnetometer mag(&Spi0, &magResource, M_SCALE_2GS, M_ODR_125);
Gyroscope gyro(&Spi0, &gyroResource, LSM9DS0_GYROSCALE_245DPS, G_ODR_95_BW_125);

void setup(void) {

/*#ifdef __DEBUG__
    Debug.begin();
    Debug.println("START");
#endif*/

    Dispatcher::instance().subscribe(EVENT_READ_GPS, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_GYROSCOPE, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_MAGNETOMETER, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_KALMAN, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_READ_ORIENTATION, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_APOGEE, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_MAIN_FIRED, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_PILOT_FIRED, &sdPrinter);
    Dispatcher::instance().subscribe(EVENT_LIFTOFF, &sdPrinter);


    Dispatcher::instance().subscribe(EVENT_APOGEE, (Listener*)&pilotActuator);
    Dispatcher::instance().subscribe(EVENT_APOGEE, (Listener*)&mainActuator);

    Dispatcher::instance().subscribe(EVENT_READ_GPS, (Listener*)&apogeeMiddleware);
    Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, (Listener*)&apogeeMiddleware);

    Dispatcher::instance().subscribe(EVENT_READ_ACCELEROMETER, (Listener*)&sensorFusion);
    Dispatcher::instance().subscribe(EVENT_READ_MAGNETOMETER, (Listener*)&sensorFusion);

    /*TODO: attach sensors to the poller*/
    Poller::instance().attachGPS(&gps); //Baudrate should be 115200 and 10Hz freq
    Poller::instance().attachAccelerometer(accel);
    Poller::instance().attachGyroscope(&gyro);
    Poller::instance().attachMagnetometer(&mag);
    Poller::instance().start();

/*#ifdef __DEBUG__
        Debug.println("setup end");
#endif*/
}

// Nothing to do here
void loop(void) {}
