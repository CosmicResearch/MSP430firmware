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

#include <printers/SDFilePrinter.h>
#include <stdio.h>

SDFilePrinter* SDFilePrinter::instance = NULL;

SDFilePrinter::SDFilePrinter(SensSDVolume* volume, const char* filename) {
    this->volume = volume;
    strcpy(this->filename, filename);
    this->started = false;
    this->starting = false;
    this->writing = false;
    instance = this;
}

error_t SDFilePrinter::start() {
    if (started || starting) {
        return EALREADY;
    }
    starting = true;
    file = new SensSDFile(volume);

    volume->attachOnInitVolumeDone(onInitDone);
    file->attachOnOpenRootDone(onOpenRootDone);
    file->attachOnOpenDone(onOpenFileDone);
    file->attachOnWriteDone(onWriteFileDone);
    return volume->initVolume(0);
}

error_t SDFilePrinter::stop() {
    starting = false;
    started = false;
    return file->close();
}

void SDFilePrinter::onInitDone(error_t result) {
    if (result == SUCCESS) {
        instance->file->openRoot();
    }
    else {
        instance->starting = false;
    }
}

void SDFilePrinter::onOpenRootDone(error_t result) {
    if (result == SUCCESS) {
        instance->file->open(instance->filename, O_APPEND | O_WRITE | O_CREAT);
    }
    else {
        instance->starting = false;
    }
}

void SDFilePrinter::onOpenFileDone(error_t result) {
    if (result == SUCCESS) {
        if (!instance->started) {
            instance->started = true;
            instance->starting = false;
        }
    }
    else {
        instance->starting = false;
    }
}

void SDFilePrinter::onWriteFileDone(uint16_t lenght, error_t result) {
    instance->writing = false;
    if (result != SUCCESS) {
    }
}

void SDFilePrinter::execute(Event e, void* data) {
    if (!started || writing) {
        return;
    }
    this->writing = true;
    char buff[50];
    snprintf(buff, sizeof(buff), "Received event %d\n", e);
    file->write(buff, strlen(buff));
}

bool SDFilePrinter::isStarted() {
     return this->started;
}


