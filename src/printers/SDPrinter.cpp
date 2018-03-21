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

#include <printers/SDPrinter.h>

SDPrinter* SDPrinter::instance = NULL;
uint32_t SDPrinter::blockNumber = FIRST_BLOCK;
bool SDPrinter::writing = false;
bool SDPrinter::started = false;

SDPrinter::SDPrinter(SensSDVolume* volume, Resource* resource) {
    this->volume = volume;
    this->resource = resource;
    SDPrinter::instance = this;
    this->bufferInUse = waitBuff1;
    this->buffPos = 0;
}

error_t SDPrinter::start() {
    volume->attachOnInitCardDone(onInitCardDone);
    volume->attachOnWriteBlockDone(onWriteBlockDone);
    volume->initCard();
    return SUCCESS;
}

error_t SDPrinter::stop() {
    return SUCCESS;
}

void SDPrinter::print(Event e, void* data) {
    if (!started) {
        return;
    }
    uint32_t dataSize = eventDataSize(e, data);
    if ((dataSize + 2 + buffPos) >= BLOCK_SIZE) {
         writeBlock(bufferInUse);
    }
    char* dataBytes = (char*)data;
    bufferInUse[buffPos] = e;
    bufferInUse[buffPos+1] = dataSize;
    for(int i = 4; i < dataSize; ++i) {
        bufferInUse[i+buffPos] = dataBytes[i];
    }
    buffPos += dataSize + 2;
}

void SDPrinter::writeBlock(uint8_t* buffer) {
    buffPos = 0;
    if (writing) {
        return;
    }
    uint8_t* bufferToWrite = bufferInUse;
    if (bufferInUse == waitBuff1) {
        bufferInUse = waitBuff2;
    }
    else {
        bufferInUse = waitBuff2;
    }
    writing = true;
    postTask(writeBlockTask, bufferToWrite);
}

void SDPrinter::writeBlockTask(void* buffer) {
    uint8_t* bufferBytes = (uint8_t*) buffer;
    instance->volume->writeBlock(blockNumber, bufferBytes);
}

void SDPrinter::onInitCardDone(error_t result) {
    if (result != SUCCESS) {
        return;
    }
    started = true;
}

void SDPrinter::onWriteBlockDone(error_t result) {
    writing = false;
    blockNumber++;
}
