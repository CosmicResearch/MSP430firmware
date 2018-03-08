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

void DebugPrinter::print(Event event, void* data) {
    Debug.print("Im").println((int)id);
    if (!this->started) {
        return;
    }
    Debug.print("Received event").println(event);

    switch(event) {

        case EVENT_READ_GPS: {
            Debug.println("Received GPS data");
            gps_data_t gps_data = *((gps_data_t*)data);
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
            adxl377_data_t accel_data = *((adxl377_data_t*)data);
            Debug.print("x: ").print(accel_data._chanx*0.1f*9.80665f).print("m/s^2 y: ").print(accel_data._chany*0.1*9.80665f).print("m/s^2 z: ").print(accel_data._chanz*0.1f*9.80665f).println("m/s^2");
            break;
        }
        case EVENT_SENSOR_INIT: {
            Debug.print("Init of sensor").println(*((int*) data));
            break;
        }
        case EVENT_ERROR_SENSOR_INIT: {
            Debug.print("Error at init of sensor").println(*((int*) data));
            break;
        }
        case EVENT_ERROR_SENSOR_READ: {
            Debug.print("Error when reading sensor").println(*((int*) data));
            break;
        }

    }

}
