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

uint32_t eventDataSize(Event e) {
        uint32_t res = 0;
        switch (e) {

            case EVENT_READ_GPS: {
                res = sizeof(gps_data_t);
                break;
            }
            case EVENT_READ_ACCELEROMETER: {
                res = sizeof(accel_data_t);
                break;
            }

            case EVENT_READ_MAGNETOMETER: {
                res = sizeof(lsm9ds0_data_t);
                break;
            }

            case EVENT_READ_GYROSCOPE: {
                res = sizeof(lsm9ds0gyro_data_t);
                break;
            }

            case EVENT_READ_BAROMETER: {
                res = sizeof(bmp280_data_comp_t);
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
                res = sizeof(kalman_data_t);
                break;
            }
            case EVENT_ERROR_SENSOR_INIT:
            case EVENT_ERROR_SENSOR_READ: {
                res = sizeof(uint8_t);
                break;
            }

        }
        return res;
}

bool packEventData(Event e, void* data, char buff[], size_t& size) {
    bool ret = true;
    buff[0] = (int8_t)e;
    int i = 1;
    switch(e) {

    case EVENT_READ_GPS: {
        gps_data_t gps_data = *((gps_data_t*)data);
        uint8_t data_size = sizeof(gps_data.time) + sizeof(gps_data.altitude) + sizeof(gps_data.latitude) + sizeof(gps_data.longitude) + sizeof(gps_data.latitudeChar) + sizeof(gps_data.longitudeChar);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &gps_data.time, sizeof(gps_data.time));
        i += sizeof(gps_data.time);
        memcpy(&buff[i], &gps_data.altitude, sizeof(gps_data.altitude));
        i += sizeof(gps_data.altitude);
        memcpy(&buff[i], &gps_data.latitude, sizeof(gps_data.latitude));
        i += sizeof(gps_data.latitude);
        memcpy(&buff[i], &gps_data.longitude, sizeof(gps_data.longitude));
        i += sizeof(gps_data.longitude);
        memcpy(&buff[i], &gps_data.latitudeChar, sizeof(gps_data.latitudeChar));
        i += sizeof(gps_data.latitudeChar);
        memcpy(&buff[i], &gps_data.longitudeChar, sizeof(gps_data.longitudeChar));
        i += sizeof(gps_data.longitudeChar);
        memcpy(&buff[i], &gps_data.fix, sizeof(gps_data.fix));
        i += sizeof(gps_data.fix);
        break;
    }

    case EVENT_READ_ACCELEROMETER: {
        accel_data_t accel_data = *((accel_data_t*)data);
        uint8_t data_size = sizeof(accel_data.time) + sizeof(accel_data._chanx) + sizeof(accel_data._chany) + sizeof(accel_data._chanz);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &accel_data.time, sizeof(accel_data.time));
        i += sizeof(accel_data.time);
        memcpy(&buff[i], &accel_data._chanx, sizeof(accel_data._chanx));
        i += sizeof(accel_data._chanx);
        memcpy(&buff[i], &accel_data._chany, sizeof(accel_data._chany));
        i += sizeof(accel_data._chany);
        memcpy(&buff[i], &accel_data._chanz, sizeof(accel_data._chanz));
        i += sizeof(accel_data._chanz);
        break;
    }

    case EVENT_READ_MAGNETOMETER: {
        mag_data_t mag_data = *((mag_data_t*)data);
        uint8_t data_size = sizeof(mag_data.time) + sizeof(mag_data.x) + sizeof(mag_data.y) + sizeof(mag_data.z);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &mag_data.time, sizeof(mag_data.time));
        i += sizeof(mag_data.time);
        memcpy(&buff[i], &mag_data.x, sizeof(mag_data.x));
        i += sizeof(mag_data.x);
        memcpy(&buff[i], &mag_data.y, sizeof(mag_data.y));
        i += sizeof(mag_data.y);
        memcpy(&buff[i], &mag_data.z, sizeof(mag_data.z));
        i += sizeof(mag_data.z);
        break;
    }

    case EVENT_READ_GYROSCOPE: {
        gyro_data_t mag_data = *((gyro_data_t*)data);
        uint8_t data_size = sizeof(mag_data.time) + sizeof(mag_data.x) + sizeof(mag_data.y) + sizeof(mag_data.z);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &mag_data.time, sizeof(mag_data.time));
        i += sizeof(mag_data.time);
        memcpy(&buff[i], &mag_data.x, sizeof(mag_data.x));
        i += sizeof(mag_data.x);
        memcpy(&buff[i], &mag_data.y, sizeof(mag_data.y));
        i += sizeof(mag_data.y);
        memcpy(&buff[i], &mag_data.z, sizeof(mag_data.z));
        i += sizeof(mag_data.z);
        break;
    }

    case EVENT_READ_BAROMETER: {
        bar_data_t mag_data = *((bar_data_t*)data);
        uint8_t data_size = sizeof(mag_data.time) + sizeof(mag_data.pressure) + sizeof(mag_data.temperature) + sizeof(mag_data.altitude);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &mag_data.time, sizeof(mag_data.time));
        i += sizeof(mag_data.time);
        memcpy(&buff[i], &mag_data.pressure, sizeof(mag_data.pressure));
        i += sizeof(mag_data.pressure);
        memcpy(&buff[i], &mag_data.temperature, sizeof(mag_data.temperature));
        i += sizeof(mag_data.temperature);
        memcpy(&buff[i], &mag_data.altitude, sizeof(mag_data.altitude));
        i += sizeof(mag_data.altitude);
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
        kalman_data_t mag_data = *((kalman_data_t*)data);
        uint8_t data_size = sizeof(mag_data.time) + sizeof(mag_data.altitude) + sizeof(mag_data.velocity) + sizeof(mag_data.type);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &mag_data.time, sizeof(mag_data.time));
        i += sizeof(mag_data.time);
        memcpy(&buff[i], &mag_data.altitude, sizeof(mag_data.altitude));
        i += sizeof(mag_data.altitude);
        memcpy(&buff[i], &mag_data.velocity, sizeof(mag_data.velocity));
        i += sizeof(mag_data.velocity);
        buff[i] = (uint8_t)mag_data.type;
        i += sizeof(uint8_t);
        break;
    }

    case EVENT_READ_ORIENTATION: {
        sensfusion_data_t mag_data = *((sensfusion_data_t*)data);
        uint8_t data_size = sizeof(mag_data.time) + sizeof(mag_data.pitch) + sizeof(mag_data.roll) + sizeof(mag_data.heading);
        buff[i] = data_size;
        i += sizeof(data_size);
        memcpy(&buff[i], &mag_data.time, sizeof(mag_data.time));
        i += sizeof(mag_data.time);
        memcpy(&buff[i], &mag_data.pitch, sizeof(mag_data.pitch));
        i += sizeof(mag_data.pitch);
        memcpy(&buff[i], &mag_data.roll, sizeof(mag_data.roll));
        i += sizeof(mag_data.roll);
        memcpy(&buff[i], &mag_data.heading, sizeof(mag_data.heading));
        i += sizeof(mag_data.heading);
        break;
    }

    case EVENT_ERROR_SENSOR_INIT:
    case EVENT_SENSOR_INIT:
    case EVENT_ERROR_SENSOR_READ: {
        uint8_t id = *((uint8_t*)data);
        uint8_t data_size = sizeof(id);
        buff[i] = data_size;
        i += sizeof(data_size);
        buff[i] = id;
        i += sizeof(id);
        break;
    }

    default: {
        ret = false;
        break;
    }


    }
    size = i;
    return ret;
}
