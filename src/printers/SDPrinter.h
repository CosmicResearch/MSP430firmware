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

#ifndef BONDAR_SDPRINTER
#define BONDAR_SDPRINTER

#include "Bondar.h"
#include "SensSDVolume.h"

#define FIRST_BLOCK     (1)
#define LAST_BLOCK      (999)
#define BLOCK_SIZE      (512)

class SDPrinter : public Printer {

private:

    static uint32_t blockNumber;
    uint8_t waitBuff1[512];
    uint8_t waitBuff2[512];
    uint8_t * bufferInUse;
    uint32_t buffPos;
    SensSDVolume* volume;
    Resource* resource;

    static bool writing;
    static bool started;

    static SDPrinter* instance;

    static void onInitCardDone(error_t result);
    static void onWriteBlockDone(error_t result);
    static void writeBlockTask(void* buffer);

    void writeBlock(uint8_t* buffer);

public:
    SDPrinter(SensSDVolume* volume, Resource* resource);

    virtual error_t start();

    virtual error_t stop();

    virtual bool isStarted();

    virtual void print(Event e, void* data);

};

#endif
