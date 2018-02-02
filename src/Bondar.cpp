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
#include "Dispatcher.h"

Poller* poller;
Dispatcher* dispatcher;

void setup(void) {
    dispatcher = Dispatcher::createInstance();
    //TODO: subscribe actuators and printers to the dispatcher
    poller = Poller::createInstance(dispatcher, 100);
    //TODO: attach sensors to the poller
    poller->start();
}

// Nothing to do here (づ◔ ͜ʖ◔)づ
void loop(void) {}