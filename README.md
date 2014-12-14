433 trans/receiver
==================

# About
Simple readings Arduino sender/Raspberry Pi receiver over 443MHz module. For now I've only support DS18B20. But soon I'll add support for DHT22 and MQ-5 (gas sensor).

I started this project to learn more about Arduino, ATtiny, 1wire and C/C++, and someday I may drop this project.

# Compilation on Raspberry Pi
First you need to install sqlite3 and libsqlite3-dev, keep in mind that on Raspian in default is installed version 2.x, not 3.x.

```
sudo aptitude install sqlite3 libsqlite3-dev
```
Also, you have to install wiringPi, sources and information from http://wiringpi.com/download-and-install/

Create databse ``sqlite3 readings.sqlite``, and create table ``create table readings(created_at DATETIME DEFAULT CURRENT_TIMESTAMP, trans_id INTEGER, value TEXT, type TEXT);``

Then just ``make`` it, and run ``sudo ./rpi_receiver``

# Sender (Arduino/ATtiny)
I'm using this https://github.com/damellis/attiny/tree/ide-1.5.x to build it for 

```
Sketch uses 5,828 bytes (71%) of program storage space. Maximum is 8,192 bytes.
Global variables use 285 bytes of dynamic memory.
```

# TODO
* Support for DHT22/11
* Support MQ-5 gas sensor
* Add web display of gathered data with statistics
* Power saving on ATtiny, so I could get more working time on single CR2032
* CRC8 checking, because I've sometimes got faulty transmission. Now I added some lame code
* Info about building and uploading data to ATtiny, there is some tuts, but I found that they are pretty incomplete
* Example ATtiny build
