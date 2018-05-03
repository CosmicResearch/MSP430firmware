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

bool packEventData(Event e, Variant data, char buff[], size_t& size) {
    bool ret = true;
    buff[0] = (int8_t)e;
    switch(e) {
        case EVENT_READ_GPS: {
            data.toGPSData().pack(buff, size);
            break;
        }
        case EVENT_READ_ACCELEROMETER: {
            data.toAccelData().pack(buff, size);
            break;
        }
        case EVENT_READ_MAGNETOMETER: {
            data.toMagData().pack(buff, size);
            break;
        }
        case EVENT_READ_GYROSCOPE: {
            data.toGyroData().pack(buff, size);
            break;
        }
        case EVENT_READ_BAROMETER: {
            data.toBarData().pack(buff, size);
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
            data.toKalmanData().pack(buff, size);
            break;
        }
        case EVENT_READ_ORIENTATION: {
            data.toSensFusionData().pack(buff, size);
            break;
        }

        case EVENT_ERROR_SENSOR_INIT:
        case EVENT_SENSOR_INIT:
        case EVENT_ERROR_SENSOR_READ: {
            uint8_t id = data.toUInt8();
            size = sizeof(id);
            buff[1] = (uint8_t)size;
            buff[2] = id;
            break;
        }

        default: {
            ret = false;
            break;
        }
    }
    size += 2;
    return ret;
}
