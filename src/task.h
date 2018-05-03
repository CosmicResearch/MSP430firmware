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

#ifndef BONDAR_TASK
#define BONDAR_TASK

#include "Listener.h"

/**
 * One process is a group of tasks, and process an event and its data through the tasks.
 * Every task is linked to a `Listener`.
 */

struct process_t {
    Event event;
    Variant data;
    uint8_t n_running;
};

struct event_task_t {
    uint8_t process_id;
    Listener* listener;
};

#endif
