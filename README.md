# Kamstrup-Radius Interface
 ESP8266 interface for Kamstrup (Radius provided) electricity meter

Please also have a look the ESPHome component based on this code and working on the HAN module:\
https://github.com/Asbjoern/ESPHome-Kamstrup-Radius

This project interfaces with the Kampstrup electricity meter provided by Radius in Denmark.\
It decrypts the messages pushed on the HAN port and publishes the readings on MQTT.

There is also a proof of concept Python3 script (Kamstrup reader.py). Reads serialport connected to meter, decrypts, parses and publishes result to mqtt.  
Based on Python example provided by Radius.

## Project contains:
* HAN module for the Radius provided Kamstrup Omnipower electricity meters
* Arduino based software for a ESP8266 module. For receiving, decrypting and parsing push messages from meter.
* Publish data to MQTT

## Project uses: 
* Relevant files from mbed TLS library https://tls.mbed.org/
* Modified mbusparser library from Jalla2000's norwegian project https://github.com/jalla2000/kamstrup-mqtt
* Use Wifimanager library (or similar) to get captive portal and configuration

