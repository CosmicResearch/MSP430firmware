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

    this->dispatcher->start();

    if (this->gps != NULL) {
        error_t ret = this->gps->start();
        if (ret != SUCCESS) {
            return ret;
        }
    }
    if (this->magne != NULL) {
        error_t ret = this->magne->start();
        if (ret != SUCCESS) {
            return ret;
        }
    }
    if (this->accel != NULL) {
        error_t ret = this->accel->start();
        if (ret != SUCCESS) {
            return ret;
        }
    }
    if (this->gyro != NULL) {
        error_t ret = this->gyro->start();
        if (ret != SUCCESS) {
            return ret;
        }
    }
    if (this->bar != NULL) {
        error_t ret = this->bar->start();
        if (ret != SUCCESS) {
            return ret;
        }
    }

    this->gps->attachReadDone(onGPSRead);
    this->magne->attachReadDone(onMagnetometerRead);
    this->accel->attachReadDone(onAccelerometerRead);
    this->gyro->attachReadDone(onGyroscopeRead);
    this->bar->attachReadDone(onBarometerRead);

    this->timer->attachCallback(tick);
    this->timer->startPeriodic(this->interval);

#ifdef DEBUG

    Debug.begin();

#endif

    this->started = true;
    return SUCCESS;
}

void Poller::tick() {
    Poller::instance->readSensors();
}

void Poller::readSensors() {
    if (this->gps != NULL) {
        this->gps->read();
    }
    if (this->accel != NULL) {
        this->accel->read();
    }
    if (this->magne != NULL) {
        this->magne->read();
    }
    if (this->gyro != NULL) {
        this->gyro->read();
    }
    if (this->bar != NULL) {
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
    Poller::instance->dispatchData(EVENT_READ_GPS, data);
}

void Poller::onAccelerometerRead(sensor_data_t* data, error_t error) {
    Poller::instance->dispatchData(EVENT_READ_ACCELEROMETER, data);
}

void Poller::onBarometerRead(sensor_data_t* data, error_t error) {

}

void Poller::onGyroscopeRead(sensor_data_t* data, error_t error) {

}

void Poller::onMagnetometerRead(sensor_data_t* data, error_t error) {

}

void Poller::dispatchData(int event, void* data) {
    this->dispatcher->dispatchData(event, data);
}

void Poller::dispatchError(int event) {
    this->dispatcher->dispatchError(event);
}