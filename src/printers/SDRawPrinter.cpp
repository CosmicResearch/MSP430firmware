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

#include <printers/SDRawPrinter.h>

SDRawPrinter* SDRawPrinter::instance = NULL;
uint32_t SDRawPrinter::blockNumber = FIRST_BLOCK;
bool SDRawPrinter::writing = false;
bool SDRawPrinter::started = false;
bool SDRawPrinter::waitingToStart = false;

SDRawPrinter::SDRawPrinter(SensSDVolume* volume, Resource* resource) {
    this->volume = volume;
    this->resource = resource;
    SDRawPrinter::instance = this;
    this->bufferInUse = waitBuff1;
    this->buffPos = 0;
}

error_t SDRawPrinter::start() {
    if (waitingToStart || started) {
        return EALREADY;
    }
    volume->attachOnInitCardDone(onInitCardDone);
    volume->attachOnWriteBlockDone(onWriteBlockDone);
    error_t ret = volume->initCard();
    if (ret==SUCCESS) {
        Debug.println("Initcard success");
        waitingToStart = true;
    }
    else {
        Debug.println("Initcard fail");
    }
    return ret;
}

bool SDRawPrinter::isStarted() {
    return started;
}

error_t SDRawPrinter::stop() {
    started = false;
    return SUCCESS;
}

void SDRawPrinter::print(Event e, void* data) {
    if (!started) {
        return;
    }
    uint32_t dataSize = eventDataSize(e, data);
    Debug.print("Event: ").print(e).print("Data size: ").println((int)dataSize);
    if ((dataSize + 5 + buffPos) >= BLOCK_SIZE) {
         writeBlock(bufferInUse);
    }
    char* dataBytes = (char*)data;
    bufferInUse[buffPos] = e;
    bufferInUse[buffPos+1] = dataSize&0x00FF;
    bufferInUse[buffPos+2] = (dataSize>>8)&0x00FF;
    bufferInUse[buffPos+3] = (dataSize>>16)&0x00FF;
    bufferInUse[buffPos+4] = (dataSize>>24)&0x00FF;
    for(int i = 5; i < dataSize+5; ++i) {
        bufferInUse[i+buffPos] = dataBytes[i];
    }
    buffPos += dataSize + 5;
}

void SDRawPrinter::writeBlock(uint8_t* buffer) {
    Debug.print("writeBlock ").println(blockNumber);
    buffPos = 0;
    if (writing) {
        Debug.println("Its writing");
        return;
    }
    uint8_t* bufferToWrite = bufferInUse;
    if (bufferInUse == waitBuff1) {
        bufferInUse = waitBuff2;
    }
    else {
        bufferInUse = waitBuff1;
    }
    writing = true;
    postTask(writeBlockTask, bufferToWrite);
}

void SDRawPrinter::writeBlockTask(void* buffer) {
    Debug.println("writeBlockTask");
    uint8_t* bufferBytes = (uint8_t*) buffer;
    instance->volume->writeBlock(blockNumber, bufferBytes);
}

void SDRawPrinter::onInitCardDone(error_t result) {
    if (result != SUCCESS) {
        Debug.println("Could not init SD");
        return;
    }
    Debug.println("SD started!");
    started = true;
    waitingToStart = false;
}

void SDRawPrinter::onWriteBlockDone(error_t result) {
    Debug.print("writeBlockDone").println((result==SUCCESS)?"SUCCESS":"FAIL");
    writing = false;
    blockNumber++;
    if (blockNumber > LAST_BLOCK) {
        started = false;
    }
}
