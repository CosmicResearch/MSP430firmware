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
        return EALREADY; //Already running!
    }

    if (!this->dispatcher->isStarted()) {
        error_t ret = this->dispatcher->start();
        if (ret != SUCCESS) {
            return ERROR;
        }
    }

    error_t res = SUCCESS;

    if (this->gps != NULL && !this->gps->isStarted()) {
        error_t ret = this->gps->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GPS));
        }
    }
    if (this->magne != NULL && !this->magne->isStarted()) {
        error_t ret = this->magne->start();
        if (ret != SUCCESS) {
           this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(MAGNETOMETER));
        }
    }
    if (this->accel != NULL && !this->accel->isStarted()) {
        error_t ret = this->accel->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(ACCELEROMETER));
        }
    }
    if (this->gyro != NULL && !this->gyro->isStarted()) {
        error_t ret = this->gyro->start();
        if (ret != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_INIT, new int(GYROSCOPE));
        }
    }
    if (this->bar != NULL && !this->bar->isStarted()) {
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

error_t Poller::stop() {
    if (this->started) {
        this->timer->stop();
        this->started = false;
        return SUCCESS;
    }
    return ERROR;
}

bool Poller::isStarted() {
    return this->started;
}

void Poller::tick() {
    Poller::instance->readSensors();
}

void Poller::readSensors() {
    if (this->gps != NULL && this->gps->isStarted()) {
        error_t error = this->gps->read();
        if (error != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_READ, new int(GPS));
        }
    }
    if (this->accel != NULL && this->accel->isStarted()) {
        error_t error = this->accel->read();
        if (error != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_READ, new int(ACCELEROMETER));
        }
    }
    if (this->magne != NULL && this->magne->isStarted()) {
        error_t error = this->magne->read();
        if (error != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_READ, new int(MAGNETOMETER));
        }
    }
    if (this->gyro != NULL && this->gyro->isStarted()) {
        error_t error = this->gyro->read();
        if (error != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_READ, new int(GYROSCOPE));
        }
    }
    if (this->bar != NULL && this->bar->isStarted()) {
        error_t error = this->bar->read();
        if (error != SUCCESS) {
            this->dispatch(EVENT_ERROR_SENSOR_READ, new int(BAROMETER));
        }
    }
}

void Poller::attachGPS(GPS* gps) {
    this->gps = gps;
    if (this->started) {
        this->stop();
    }
}

void Poller::attachAccelerometer(Accelerometer* accel) {
    this->accel = accel;
    if (this->started) {
        this->stop();
    }
}

void Poller::attachBarometer(Barometer* bar) {
    this->bar = bar;
    if (this->started) {
        this->stop();
    }
}

void Poller::attachGyroscope(Gyroscope* gyro) {
    this->gyro = gyro;
    if (this->started) {
        this->stop();
    }
}

void Poller::attachMagnetometer(Magnetometer* magne) {
    this->magne = magne;
    if (this->started) {
        this->stop();
    }
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

GPS* Poller::getGPS() {
    return this->gps;
}

Accelerometer* Poller::getAccelerometer() {
    return this->accel;
}

Magnetometer* Poller::getMagnetometer() {
    return this->magne;
}

Gyroscope* Poller::getGyroscope() {
    return this->gyro;
}

Barometer* Poller::getBarometer() {
    return this->bar;
}