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

#ifndef BONDAR_VARIANT
#define BONDAR_VARIANT

#include <stdint.h>
#include "Senscape.h"
#include "GPS.h"
#include "Accelerometer.h"
#include "Barometer.h"
#include "Gyroscope.h"
#include "Magnetometer.h"
#include "Kalman.h"
#include "sensorfusion/SensorFusion.h"

class Variant {

public:

    Variant() {
        this->type = NONE;
        this->data.unknown_data = NULL;
    }

    Variant(gps_data_t data);
    Variant(accel_data_t data);
    Variant(mag_data_t data);
    Variant(gyro_data_t data);
    Variant(bar_data_t data);
    Variant(kalman_data_t data);
    Variant(sensfusion_data_t data);
    Variant(uint8_t data);
    Variant(void* data);

    gps_data_t toGPSData(bool* ok = NULL);
    accel_data_t toAccelData(bool* ok = NULL);
    mag_data_t toMagData(bool* ok = NULL);
    gyro_data_t toGyroData(bool* ok = NULL);
    bar_data_t toBarData(bool* ok = NULL);
    kalman_data_t toKalmanData(bool* ok = NULL);
    sensfusion_data_t toSensFusionData(bool* ok = NULL);
    uint8_t toUInt8(bool* ok = NULL);
    void* toVoidPointer(bool* ok = NULL);

private:

    enum {
        NONE,
        GPS_DATA,
        ACCEL_DATA,
        MAG_DATA,
        GYRO_DATA,
        BAR_DATA,
        KALMAN_DATA,
        SENSFUSION_DATA,
        UINT8,
        UNKNOWN
    } type;

    union u_data {

        u_data() {
            memset(this, 0, sizeof(u_data));
        }

        gps_data_t gps_data;
        accel_data_t accel_data;
        mag_data_t mag_data;
        gyro_data_t gyro_data;
        bar_data_t bar_data;
        kalman_data_t kalman_data;
        sensfusion_data_t sensfusion_data;
        uint8_t uint8;
        void* unknown_data;
    } data;
};

#endif
