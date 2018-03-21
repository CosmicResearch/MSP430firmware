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

#include <stdint.h>
#include "Listener.h"
#include "Senscape.h"
#include "GPS.h"
#include "Accelerometer.h"
#include "Barometer.h"
#include "Gyroscope.h"
#include "Magnetometer.h"
#include "Kalman.h"

typedef int8_t Event;


/**
 * One process is a group of tasks, and process an event and its data through the tasks.
 * Every task is linked to a `Listener`.
 */

struct process_t {
    Event event;
    void* data;
    uint8_t n_running;
};

struct event_task_t {
    uint8_t process_id;
    Listener* listener;
};

#define N_MAX_EVENTS 64
#define N_PER_EVENT 5

#define EVENT_UNDEFINED 0

/**
 * SENSOR READINGS & FILTERS 1-15
*/

#define EVENT_READ_MIN 1
#define EVENT_READ_MAX 15

#define EVENT_READ_GPS 1
#define EVENT_READ_ACCELEROMETER 2
#define EVENT_READ_MAGNETOMETER 3
#define EVENT_READ_GYROSCOPE 4
#define EVENT_READ_BAROMETER 5
#define EVENT_READ_KALMAN 6

/**
 * NOTIFICATIONS 16-31
*/

#define EVENT_NOTIFICATION_MIN 16
#define EVENT_NOTIFICATION_MAX 31

#define EVENT_APOGEE  16
#define EVENT_MAIN_FIRED 17
#define EVENT_DROGUE_FIRED 18
#define EVENT_PILOT_FIRED 18 //PILOT == DROGUE
#define EVENT_SENSOR_INIT 19
#define EVENT_LIFTOFF 20
#define EVENT_SUPERSONIC_STARTED 21
#define EVENT_SUPERSONIC_ENDED 22

/**
 * ERRORS < 32-63
*/

#define EVENT_ERROR 32
#define EVENT_ERROR_SENSOR_INIT 33
#define EVENT_ERROR_PRINTER_INIT 34
#define EVENT_ERROR_ACTUATOR_INIT 35
#define EVENT_ERROR_MIDDLEWARE_INIT 36
#define EVENT_ERROR_SENSOR_READ 37
#define EVENT_ERROR_PILOT 38
#define EVENT_ERROR_MAIN 39

/**
 * SENSORS
*/

#define SENSOR_GPS 0
#define SENSOR_ACCELEROMETER 1
#define SENSOR_GYROSCOPE 2
#define SENSOR_MAGNETOMETER 3
#define SENSOR_BAROMETER 4

void safeDeleteEventData(Event e, void* data);
uint32_t eventDataSize(Event e, void* data);

#endif
