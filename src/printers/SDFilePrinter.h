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

#ifndef BONDAR_SDFILE
#define BONDAR_SDFILE

#include "Bondar.h"
#include "SensSDVolume.h"
#include "SensSDFile.h"

class SDFilePrinter : public Printer {
public:
    SDFilePrinter(SensSDVolume* volume, const char* filename);

    virtual error_t start();

    virtual error_t stop();

    virtual bool isStarted();

    virtual void print(Event e, void* data);
private:
    SensSDVolume* volume;
    SensSDFile* file;
    char filename[50];

    bool started;
    bool starting;
    bool writing;

    static void onInitDone(error_t result);
    static void onOpenRootDone(error_t result);
    static void onOpenFileDone(error_t result);
    static void onWriteFileDone(uint16_t lenght, error_t result);

    static SDFilePrinter* instance;
};

#endif
