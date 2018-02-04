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

#ifndef BONDAR_EVENTS
#define BONDAR_EVENTS

#include <cstdint>

typedef int8_t Event;


// postTask() only allows 1 parameter, this struct was created to circumvent this limitation

struct event_t {
    Event event;
    void* data;
};

#define EVENT_UNDEFINED 0

/**
 * SENSOR READINGS 1-15
*/

#define EVENT_READ_MIN 1
#define EVENT_READ_MAX 15

#define EVENT_READ_GPS 1
#define EVENT_READ_ACCELEROMETER 2
#define EVENT_READ_MAGNETOMETER 3
#define EVENT_READ_GYROSCOPE 4
#define EVENT_READ_BAROMETER 5

/**
 * NOTIFICATIONS 16-31
*/

#define EVENT_NOTIFICATION_MIN 16
#define EVENT_NOTIFICATION_MAX 31

#define EVENT_APOGEE  16
#define EVENT_MAIN_FIRED 17
#define EVENT_DROGUE_FIRED 18
#define EVENT_SENSOR_INIT 19

/**
 * ERRORS < 0
*/

#define EVENT_ERROR -1
#define EVENT_ERROR_SENSOR_INIT -2
#define EVENT_ERROR_PRINTER_INIT -3
#define EVENT_ERROR_ACTUATOR_INIT -4
#define EVENT_ERROR_MIDDLEWARE_INIT -5
#define EVENT_ERROR_SENSOR_READ -6

/**
 * SENSORS
*/

#define GPS 0
#define ACCELEROMETER 1
#define GYROSCOPE 2
#define MAGNETOMETER 3
#define BAROMETER 4

#endif