# Kamstrup-Radius Interface
 ESP8266 interface for Kamstrup (Radius provided) electricity meter

Very early development project. Barely begun.  
Proof of concept Python3 script (Kamstrup reader.py). Reads serialport connected to meter, decrypts, parses and publishes result to mqtt.  
Based on Python example provided by Radius.

## Project contains:
* Developing HAN module for the Radius provided Kamstrup Omnipower electricity meters
* Develop case for fitting into HAN module.
* Develop Arduino based software for a ESP8266 module. For receiving, decrypting and parsing push messages from meter.
* Publish data to MQTT

## Plan: 
* Use relevant files from mbed TLS library https://tls.mbed.org/
* Use modified mbusparser library from Jalla2000's norwegian project https://github.com/jalla2000/kamstrup-mqtt
* Use Wifimanager library (or similar) to get captive portal and configuration

## Tasks:
- [x] Proof of concept Python prototype (Meter to MQTT)
- [x] Get decryption working
- [x] Get parser working
- [ ] Implement MQTT client
- [ ] Add Wifimanager configuration
- [ ] Design power supply for limited current and voltage from meter. (Supercapacitor buffer?)
- [ ] Read serial port and connect to meter
- [ ] Design PCB to specifications in *5512-2584_GB_A2_11-2019 OMNIPOWER HAN interface specification.pdf*
- [ ] Implement functionality for status LED's on PCB
- [ ] Design case for PCB to specifications in *5512-2584_GB_A2_11-2019 OMNIPOWER HAN interface specification.pdf*


Development in Develop branch. Working releases in master branch.
