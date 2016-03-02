// I2C device class (I2Cdev) demonstration Arduino sketch for MPU9150
// 1/4/2013 original by Jeff Rowberg <jeff@rowberg.net> at https://github.com/jrowberg/i2cdevlib
//          modified by Aaron Weiss <aaron@sparkfun.com>
//
// Changelog:
//     2011-10-07 - initial release
//     2013-1-4 - added raw magnetometer output

/* ============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

//Import libraries
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <SPI.h>;

//Define IMU
MPU6050 accelgyro;

//Define axes
int16_t ax, ay, az;
int16_t gx, gy, gz;

#define chipSelectPin 10


void setup() {
    // join I2C bus
    Wire.begin();

    //Start the SPI library
    SPI.begin();

    // initialize serial communication
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    pinMode(chipSelectPin, OUTPUT);
}
//TODO remove frequency logic
uint8_t count = 0;
unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long time_taken = 0;
void loop() {
    end_time = micros();
    time_taken = end_time-start_time;
    //----------------------------Get data from IMU----------------------------------
    accelgyro.getAcceleration(&ax, &ay, &az);
    accelgyro.getRotation(&gx, &gy, &gz);
    //-------------------------------------------------------------------------------

    //TODO Remove prints
    Serial.print("Freq: ");
    Serial.print(1000000/time_taken);
    Serial.print(" - - ");
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz); Serial.print("\t");

    //---------------------------Calculate parity bit--------------------------------
    int bitSum = 0;
    
    bitSum += readBitSum(ax);
    bitSum += readBitSum(ay);
    bitSum += readBitSum(az);
    bitSum += readBitSum(gx);
    bitSum += readBitSum(gy);
    bitSum += readBitSum(gz);

    int parity = bitSum & 0x01;
    //-------------------------------------------------------------------------------
     

    //-------------------------------Send via SPI------------------------------------
    
    byte address = 0x00; //TODO correct the address
    
    SPI.beginTransaction(SPISettings(2000, MSBFIRST, SPI_MODE0));
    digitalWrite(chipSelectPin, LOW);
    //Actually send packages
    SPI.transfer(address, ax);
    SPI.transfer(address, ay);
    SPI.transfer(address, az);
    SPI.transfer(address, gx);
    SPI.transfer(address, gy);
    SPI.transfer(address, gz);
    SPI.transfer(address, bitRead(parity, 0)); //Tror denne returnerer int uansett
    
    digitalWrite(chipSelectPin, HIGH);
    SPI.endTransaction();
    //-------------------------------------------------------------------------------

    start_time = end_time;
    count++;
}

int readBitSum(int16_t value){
    int bitSum = 0;
    for (int i = 0; i < 16; i++){
        bitSum += bitRead(value, i);
    }
    return bitSum;
}

