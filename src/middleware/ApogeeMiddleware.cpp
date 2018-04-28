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

#include "ApogeeMiddleware.h"

ApogeeMiddleware::ApogeeMiddleware() {

    this->started = false;
    this->apogeeDetected = false;
    this->gpsValid = false;
    this->accelValid = false;

}

error_t ApogeeMiddleware::start() {

    if (this->started) {
        return EALREADY;
    }

    this->apogeeDetection = new SensApogee(new Altitude_KF(Q_ACCEL, R_ALTITUDE, PROCESS_NOISE), new Kalman(PROCESS_NOISE, SENSOR_NOISE, ESTIMATED_ERROR, INITIAL_VALUE));

    this->lastAccelData = new adxl377_data_t;
    this->lastGPSData = new gps_data_t;

    this->started = true;

    return SUCCESS;
}

error_t ApogeeMiddleware::stop() {

    if (!this->started) {
        return ERROR;
    }

    this->started = false;
    return SUCCESS;
}

bool ApogeeMiddleware::isStarted() {
    return this->started;
}

void ApogeeMiddleware::execute(Event e, void* data) {

    if (!this->started || this->apogeeDetection == NULL) {
        return;
    }

    bool apogee = false;
    kalman_data_t* kalman_data = new kalman_data_t;
    kalman_data->time = localTimeMillis();

    if (e == EVENT_READ_GPS) {

        *(this->lastGPSData) = *((gps_data_t*)data);

        if (this->lastGPSData->altitude == -1) { //GPS altitude is not valid
            this->gpsValid = false;
            return;
        }

        this->gpsValid = true;

        apogee = this->apogeeDetection->apogeeDetectionSingleKF(this->lastGPSData->altitude);

        kalman_data->altitude = this->apogeeDetection->getCorrectedAltitudeSingleKF();
        kalman_data->type = singleP;
        kalman_data->velocity = 0.0f;

    }

    else if (e == EVENT_READ_ACCELEROMETER) {

        *(this->lastAccelData) = *((adxl377_data_t*)data);
        this->accelValid = true;

    }

    else {
        delete kalman_data;
        return;
    }

    if (this->gpsValid && this->accelValid) {

        apogee = this->apogeeDetection->apogeeDetectionDoubleKF(this->lastGPSData->altitude, this->lastAccelData->_chany, POLL_INTERVAL);

        kalman_data->altitude = this->apogeeDetection->getCorrectedAltitudeDoubleKF();
        kalman_data->type = doubleP;
        kalman_data->velocity = this->apogeeDetection->getVelocityDoubleKF();

        this->gpsValid = false;
        this->accelValid = false;

    }

    if (apogee && !this->apogeeDetected) {
        this->apogeeDetected = true;

        Dispatcher::instance().dispatch(EVENT_APOGEE, kalman_data);
    }

    Dispatcher::instance().dispatch(EVENT_READ_KALMAN, kalman_data);

}

