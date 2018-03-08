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

#include "events.h"

void safeDeleteEventData(Event e, void* data) {
    if (data == NULL) {
        return;
    }
    switch (e) {

        case EVENT_READ_GPS: {
            gps_data_t* gps_data = (gps_data_t*)data;
            delete gps_data;
            break;
        }
        case EVENT_READ_ACCELEROMETER: {
            adxl377_data_t* accel_data = (adxl377_data_t*) data;
            delete accel_data;
            break;
        }

        case EVENT_READ_MAGNETOMETER: {
            lsm9ds0_data_t* mag_data = (lsm9ds0_data_t*) data;
            delete mag_data;
            break;
        }

        case EVENT_READ_GYROSCOPE: {
            lsm9ds0gyro_data_t* gyro_data = (lsm9ds0gyro_data_t*) data;
            delete gyro_data;
            break;
        }

        case EVENT_READ_BAROMETER: {
            bmp280_data_comp_t* bar_data = (bmp280_data_comp_t*) data;
            delete bar_data;
            break;
        }
        case EVENT_READ_KALMAN:
        case EVENT_APOGEE:
        case EVENT_MAIN_FIRED:
        case EVENT_PILOT_FIRED:
        case EVENT_LIFTOFF:
        case EVENT_SUPERSONIC_STARTED:
        case EVENT_SUPERSONIC_ENDED:
        case EVENT_ERROR_PILOT:
        case EVENT_ERROR_MAIN: {
            kalman_data_t* kalman_data = (kalman_data_t*) data;
            delete kalman_data;
            break;
        }
        case EVENT_ERROR_SENSOR_INIT:
        case EVENT_ERROR_SENSOR_READ: {
            uint8_t* uid = (uint8_t*) data;
            delete uid;
            break;
        }

    }
}
