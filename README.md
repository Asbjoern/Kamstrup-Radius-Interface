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

Development in Development branch. Working releases in master branch.
