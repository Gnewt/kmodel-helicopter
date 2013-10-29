K-Model Helicopter Control
==========================

Welcome! This code was initially published to supplement an MIT Maker Portfolio as part of MIT's admissions process. In the spirit of its purpose, this code is licensed under the MIT License.

This project allows you to control the "K-Model" R/C helicopter (found randomly in a mall near you...) via infrared. The command protocol uses 28 bits of command data + a 4 bit checksum, making for 32 bits in total.

Included are:
* An Arduino sketch that sends commands to control the K-Model with an IR LED
* A Python library to talk to the Arduino over serial

This project does not yet cover 100% of helicopter functionality. At the moment the following features are lacking and will be implemented in the future:
* Precise control of pitch (currently just forward or backward at 100%)
* Control of yaw
* Support for channels other than 0
  * Support for initialization is there, but will be overwritten by commands called after initialization

A note about checksums
----------------------
The checksums used by this helicopter are fairly simple. The checksum is equal to the sum of every 4-bit chunk of the command, truncated to 4 bits.

For example, take the following command (full throttle, no other changes):
    1111111000001000000000000000 (length is 28 bits)

To get the checksum:  
1. Split the command into 4-bit chunks
        1111 1110 0000 1000 0000 0000 0000
2. Add these chunks
        0b1111 + 0b1110 + 0b0000 + 0b1000 + 0b0000 + 0b0000 + 0b0000 = 0b100101
3. Truncate the result
        0b100101 & 0b1111 = 0b0101

Ta-da! 0101 is the checksum, making our whole command:
    11111110000010000000000000000101 (length is 32 bits)