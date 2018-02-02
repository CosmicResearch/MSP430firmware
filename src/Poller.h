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

#ifndef BONDAR_POLLER
#define BONDAR_POLLER

#include <cstdint>
#include "Senscape.h"
#include "GPS.h"
#include "Accelerometer.h"
#include "SensTimer.h"
#include "events.h"

class Poller {

private:

    Poller(uint32_t intervalMs, Dispatcher* dispatcher, SensTimer* timer);

    static Poller* instance;

    GPS* gps;
    Accelerometer* accel;
    Magnetometer* magne;
    Gyroscope* gyro;
    Barometer* bar;

    int32_t interval;
    Dispatcher* dispatcher;
    SensTimer* timer;

    bool started;

    static void tick();

    static void onGPSRead(sensor_data_t* data, error_t error);
    static void onAccelerometerRead(sensor_data_t* data, error_t error);
    static void onMagnetometerRead(sensor_data_t* data, error_t error);
    static void onGyroscopeRead(sensor_data_t* data, error_t error);
    static void onBarometerRead(sensor_data_t* data, error_t error);

    static void onGPSStartDone(error_t error);
    static void onAccelerometerStartDone(error_t error);
    static void onMagnetometerStartDone(error_t error);
    static void onGyroscopeStartDone(error_t error);
    static void onBarometerStartDone(error_t error);

    void dispatch(Event event, void* data);

    void readSensors();

public:

    static Poller createInstance(Dispatcher* dispatcher, uint32_t intervalMs = 100);

    error_t start();

    void attachGPS(GPS* gps);
    void attachAccelerometer(Accelerometer* accel);
    void attachMagnetometer(Magnetometer* magne);
    void attachGyroscope(Gyroscope* gyro);
    void attachBarometer(Barometer* bar);


};

#endif