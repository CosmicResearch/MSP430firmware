/*
    Bondar avionics
    Copyright (C) 2017  Associaci√≥ Cosmic Research
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

#include "DebugPrinter.h"
#include "GPS.h"
#include "Accelerometer.h"

DebugPrinter::DebugPrinter(uint8_t id) {
    this->started = false;
    this->id = id;
}

error_t DebugPrinter::start() {
    if (!started) {
#ifndef __DEBUG__
        Debug.begin();
#endif
        Debug.println("Started DebugPrinter");
        this->started = true;
    }
    return SUCCESS;
}

error_t DebugPrinter::stop() {
    Debug.end();
    this->started = false;
    return SUCCESS;
}

bool DebugPrinter::isStarted() {
    return this->started;
}

void DebugPrinter::execute(Event event, Variant data) {
    Debug.print("Im").println((int)id);
    if (!this->started) {
        return;
    }
    Debug.print("Received event").println(event);

    switch(event) {

        case EVENT_READ_GPS: {
            Debug.println("Received GPS data");
            gps_data_t gps_data = data.toGPSData();
            Debug.print("Latitude: ").print(gps_data.latitude/1000000.0f).println(gps_data.latitudeChar);
            Debug.print("Longitude: ").print(gps_data.longitude/1000000.0f).println(gps_data.longitudeChar);
            Debug.print("Altitude: ").print(gps_data.altitude/100.0f).println(" m");
            Debug.print("Fix: ").println(gps_data.fix);
            Debug.print("Time: ").print(gps_data.hour).print(":").print(gps_data.minute).print(":").println(gps_data.seconds);
            Debug.print("Type: ").println(gps_data.type);
            break;
        }
        case EVENT_READ_ACCELEROMETER : {
            Debug.println("Received accelerometer data");
            accel_data_t accel_data = data.toAccelData();
            Debug.print("x: ").print(accel_data.x*0.1f*9.80665f).print("m/s^2 y: ").print(accel_data.y*0.1*9.80665f).print("m/s^2 z: ").print(accel_data.z*0.1f*9.80665f).println("m/s^2");
            break;
        }
        case EVENT_READ_GYROSCOPE: {
            Debug.println("Received gyro data");
            int16_t x, y, z;
            gyro_data_t data_t = data.toGyroData();
            x = data_t.x;
            y = data_t.y;
            z = data_t.z;
            Debug.print("X axis: ").println(x);
            Debug.print("Y axis: ").println(y);
            Debug.print("Z axis: ").println(z);
            break;
        }
        case EVENT_READ_MAGNETOMETER: {
            Debug.println("Received mag data");
            int16_t xhi, yhi, zhi;
            mag_data_t data_t = data.toMagData();
            xhi = data_t.x;
            yhi = data_t.y;
            zhi = data_t.z;
            Debug.print("x: ").println(xhi);
            Debug.print("y: ").println(yhi);
            Debug.print("z: ").println(zhi);
            Debug.print("Temperature: ").print(data_t.u_temp).println("C∫");
            break;
        }
        case EVENT_READ_KALMAN: {
            Debug.println("Received kalman data");
            kalman_data_t data_t = data.toKalmanData();
            Debug.print("Altitude: ").println(data_t.altitude);
            break;
        }
        case EVENT_SENSOR_INIT: {
            Debug.print("Init of sensor").println((unsigned int)data.toUInt8());
            break;
        }
        case EVENT_ERROR_SENSOR_INIT: {
            Debug.print("Error at init of sensor").println((unsigned int)data.toUInt8());
            break;
        }
        case EVENT_ERROR_SENSOR_READ: {
            Debug.print("Error when reading sensor").println((unsigned int)data.toUInt8());
            break;
        }

    }

}
