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

#include "Poller.h"

Poller* Poller::instance = NULL;

Poller* Poller::createInstance(Dispatcher* dispatcher, uint32_t intervalMs = 100) {
    if (Poller::instance == NULL) {
        Poller::instance = new Poller(intervalMs, dispatcher, new SensTimer(TMILLIS));
    }
    return Poller::instance;
}

Poller::Poller(uint32_t intervalMs, Dispatcher* dispatcher, SensTimer* timer) {
    this->interval = intervalMs;
    this->dispatcher = dispatcher;
    this->timer = timer;
    this->started = false;
}

error_t Poller::start() {

    if (this->started) {
        return ERROR;
    }

    error_t ret = this->dispatcher->start();
    if (ret != SUCCESS) {
        return ERROR;
    }

    error_t res = SUCCESS;

    if (this->gps != NULL) {
        error_t ret = this->gps->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GPS));
        }
    }
    if (this->magne != NULL) {
        error_t ret = this->magne->start();
        if (ret != SUCCESS) {
           this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(MAGNETOMETER));
        }
    }
    if (this->accel != NULL) {
        error_t ret = this->accel->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(ACCELEROMETER));
        }
    }
    if (this->gyro != NULL) {
        error_t ret = this->gyro->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GYROSCOPE));
        }
    }
    if (this->bar != NULL) {
        error_t ret = this->bar->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(BAROMETER));
        }
    }

    this->gps->attachReadDone(onGPSRead);
    this->magne->attachReadDone(onMagnetometerRead);
    this->accel->attachReadDone(onAccelerometerRead);
    this->gyro->attachReadDone(onGyroscopeRead);
    this->bar->attachReadDone(onBarometerRead);

    this->gps->attachStartDone(onGPSStartDone);
    this->magne->attachStartDone(onMagnetometerStartDone);
    this->accel->attachStartDone(onAccelerometerStartDone);
    this->gyro->attachStartDone(onGyroscopeStartDone);
    this->bar->attachStartDone(onBarometerStartDone);

    this->timer->attachCallback(tick);
    this->timer->startPeriodic(this->interval);

    this->started = true;
    return res;
}

void Poller::tick() {
    Poller::instance->readSensors();
}

void Poller::readSensors() {
    if (this->gps != NULL && this->gps->isStarted()) {
        this->gps->read();
    }
    if (this->accel != NULL && this->accel->isStarted()) {
        this->accel->read();
    }
    if (this->magne != NULL && this->magne->isStarted()) {
        this->magne->read();
    }
    if (this->gyro != NULL && this->gyro->isStarted()) {
        this->gyro->read();
    }
    if (this->bar != NULL && this->bar->isStarted()) {
        this->bar->read();
    }
}

void Poller::attachGPS(GPS* gps) {
    this->gps = gps;
}

void Poller::attachAccelerometer(Accelerometer* accel) {
    this->accel = accel;
}

void Poller::attachBarometer(Barometer* bar) {
    this->bar = bar;
}

void Poller::attachGyroscope(Gyroscope* gyro) {
    this->gyro = gyro;
}

void Poller::attachMagnetometer(Magnetometer* magne) {
    this->magne = magne;
}

void Poller::onGPSRead(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_READ, new int(GPS));
        return;
    }
    Poller::instance->dispatch(EVENT_READ_GPS, data);
}

void Poller::onAccelerometerRead(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_READ, new int(ACCELEROMETER));
        return;
    }
    Poller::instance->dispatch(EVENT_READ_ACCELEROMETER, data);
}

void Poller::onBarometerRead(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_READ, new int(BAROMETER));
        return;
    }
    Poller::instance->dispatch(EVENT_READ_BAROMETER, data);
}

void Poller::onGyroscopeRead(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_READ, new int(GYROSCOPE));
        return;
    }
    Poller::instance->dispatch(EVENT_READ_GYROSCOPE, data);
}

void Poller::onMagnetometerRead(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_READ, new int(MAGNETOMETER));
        return;
    }
    Poller::instance->dispatch(EVENT_READ_MAGNETOMETER, data);
}

void Poller::onGPSStartDone(error_t error) {
    if (error == SUCCESS) {
        Poller::instance->dispatch(EVENT_SENSOR_INIT, new int(GPS));
    }
    else {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GPS));
    }
}

void Poller::onAccelerometerStartDone(error_t error) {
    if (error == SUCCESS) {
        Poller::instance->dispatch(EVENT_SENSOR_INIT, new int(ACCELEROMETER));
    }
    else {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_INIT, new int(ACCELEROMETER));
    }
}

void Poller::onGyroscopeStartDone(error_t error) {
    if (error == SUCCESS) {
        Poller::instance->dispatch(EVENT_SENSOR_INIT, new int(GYROSCOPE));
    }
    else {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GYROSCOPE));
    }
}

void Poller::onMagnetometerStartDone(error_t error) {
    if (error == SUCCESS) {
        Poller::instance->dispatch(EVENT_SENSOR_INIT, new int(MAGNETOMETER));
    }
    else {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_INIT, new int(MAGNETOMETER));
    }
}

void Poller::onBarometerStartDone(error_t error) {
    if (error == SUCCESS) {
        Poller::instance->dispatch(EVENT_SENSOR_INIT, new int(BAROMETER));
    }
    else {
        Poller::instance->dispatch(EVENT_ERROR_SENSOR_INIT, new int(BAROMETER));
    }
}

void Poller::dispatch(Event event, void* data) {
    this->dispatcher->dispatch(event, data);
}