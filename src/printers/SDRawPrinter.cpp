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

SDRawPrinter::SDRawPrinter(SensSDVolume* volume) {
    this->volume = volume;
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
#ifdef __DEBUG__
        Debug.println("Initcard success");
#endif
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

void SDRawPrinter::execute(Event e, Variant data) {
    if (!started) {
        return;
    }
    char buff[100] = {0};
    size_t dataSize;
    if (!packEventData(e, data, buff, dataSize)) {
        return; //Data cannot be packed
    }
#ifdef __DEBUG__
    Debug.print("Event: ").print(e).print(" size: ").print(dataSize).println("Packed data:");
    for(size_t i = 0; i < dataSize; ++i) {
        Debug.print((int)buff[i]).print(" ");
    }
    Debug.println();
    for(size_t i = 0; i < dataSize; ++i) {
        Debug.print(buff[i]).print(" ");
    }
    Debug.println();
#endif
    for(size_t i = 0; i < dataSize; ++i) {
        bufferInUse[i+buffPos] = buff[i];
    }
    if ((dataSize + buffPos) >= BLOCK_SIZE) {
         writeBlock(bufferInUse);
    }
    for(size_t i = 0; i < dataSize; ++i) {
        bufferInUse[i+buffPos] = buff[i];
    }
    buffPos += dataSize;
}

void SDRawPrinter::writeBlock(uint8_t* buffer) {
#ifdef __DEBUG__
    Debug.print("writeBlock ").println(blockNumber);
#endif
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
#ifdef __DEBUG__
    Debug.println("writeBlockTask");
#endif
    uint8_t* bufferBytes = (uint8_t*) buffer;
    instance->volume->writeBlock(blockNumber, bufferBytes);
}

void SDRawPrinter::onInitCardDone(error_t result) {
    if (result != SUCCESS) {
#ifdef __DEBUG__
        Debug.println("Could not init SD");
#endif
        return;
    }
#ifdef __DEBUG__
    Debug.println("SD started!");
#endif
    started = true;
    waitingToStart = false;
}

void SDRawPrinter::onWriteBlockDone(error_t result) {
#ifdef __DEBUG__
    Debug.print("writeBlockDone").println((result==SUCCESS)?"SUCCESS":"FAIL");
#endif
    writing = false;
    blockNumber++;
    if (blockNumber > LAST_BLOCK) {
        started = false;
    }
}
