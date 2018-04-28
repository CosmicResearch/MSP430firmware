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

#include "SensorFusionMiddleware.h"

SensorFusionMiddleware::SensorFusionMiddleware() {
    this->started = false;
    this->accelRead = false;
    this->magRead = false;
}

error_t SensorFusionMiddleware::start() {
    this->started = true;
    return SUCCESS;
}

error_t SensorFusionMiddleware::stop() {
    this->started = false;
    return SUCCESS;
}

bool SensorFusionMiddleware::isStarted() {
    return this->started;
}

void SensorFusionMiddleware::execute(Event event, void* data) {
    if (event == EVENT_READ_ACCELEROMETER) {
        this->accelData = *((accel_data_t*)data);
        this->accelRead = true;
    }
    else if (event == EVENT_READ_MAGNETOMETER) {
        this->magData = *((mag_data_t*)data);
        this->magRead = true;
    }
    if (accelRead && magRead) {

        sensfusion_data_t* data = new sensfusion_data_t;
        data->time = localTimeMillis();

        sensorFusion.accelGetOrientation(&accelData, data);
        sensorFusion.magTiltCompensation(SENSOR_AXIS_X, &magData, &accelData);
        sensorFusion.magTiltCompensation(SENSOR_AXIS_Y, &magData, &accelData);
        sensorFusion.magTiltCompensation(SENSOR_AXIS_Z, &magData, &accelData);

        sensorFusion.magGetOrientation(SENSOR_AXIS_X, &magData, data);

        Dispatcher::instance().dispatch(EVENT_READ_ORIENTATION, data);

        accelRead = false;
        magRead = false;
    }
}
