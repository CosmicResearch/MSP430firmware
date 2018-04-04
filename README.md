# Bondar Avionics Firmware

Firmware for the Bondar's avionic system. It has been built around a Senscape board using SensOS. Senscape is an [Irideon](http://irideon.eu/) product.

You can download SensOS and its system libraries from [here](https://github.com/CosmicResearch/BondarAvionics/releases/download/0.0.1/libs.zip).

## Build instructions

* Import the project to Code Composer Studio IDE
* Download and install the [GNU GCC MSP430 4.6.3 toolchain](http://gnutoolchains.com/msp430/)
* Right click on the project -> Properties -> General
* In the tool-chain section select 'More...' -> Add, and select the folder where you have installed the toolchain
* Apply -> OK, and select the GNU v4.6.3 from the 'Compiler version' dropdown.
* Go to the Build section -> Steps
* In the post-build steps insert `${CG_TOOL_ROOT}/bin/msp430-objcopy -O ihex -R .eeprom ${BuildArtifactFileName} ${BuildArtifactFileBaseName}.hex`
* Now you can compile it

### Known errors

* The compiler gives an 'unrecognized command line option' error

For some reason the Code Composer Studio introduces some default flags to the compiler that are not recognized by the GNUGCC toolchain. In the project properties go to Build > GNU Compiler > Edit flags... and remove these flags ` -mlarge -mhwmult=16bit -mcode-region=none -mdata-region=none`

* ./src/main.o:(.rodata.port_to_sel0+0x2): undefined reference to 'P1SEL' or similars

For some reason the Code Composer Studio introduces a default search directory from its own compiler (TI GCC). In the project properties go to Build > GNU Compiler > Directories and delete the entry `${CCS_BASE_ROOT}/msp430/include_gcc`

* Other undefined references

Make sure that you have the MSP430f2617_core.a and TIMER_lib.a files in your project root. This files are distributed by Irideon under Irideon's license and aren't free software.

## General License

Copyright 2018, Associació Cosmic Research.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## Irideon License

Copyright (c) 2014-2017, Irideon SL. All rights reserved.

All code provided by Irideon is under Irideon's End-User License Agreement.
