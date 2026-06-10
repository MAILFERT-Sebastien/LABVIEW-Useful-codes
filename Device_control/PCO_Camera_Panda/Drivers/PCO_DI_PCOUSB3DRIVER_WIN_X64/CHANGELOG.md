# Excelitas PCO GmbH   -     TECHNICAL  INFORMATION  DOCUMENT


**FOR SOFTWARE/VERSION:**  
PCO USB3 driver installation

**FOR PCO PRODUCT/VERSION:**  
pco.cameras with USB3 interface

**DESCRIPTION:**  
This packages includes a setup, which does copy the required files to 
standard PCO directory and afterwards installs the USB3 Driver in your system.

#VERSION HISTORY

## Version 2.7.0
- new cypress driver 1.3.0.2, pco installation usbdevclass 2.6.0.2
- sc2_usb3 with connection information in logfile
- sc2_usb3 restructered local functions

## Version 2.6.2
- bugfix CancelImages with edge.usb3
- do not change process priority

## Version 2.6.1
- Only added files for leica installer
- No changes in driver

## Version 2.6.0
- with new log_func.lib

## Version 2.5.0
- multi image request added
- process of image data transfers changed
- setting of camera constraints, dependent on camera model and descriptor flags
- new brand
- bugfixes

## Version 2.4.0
- new camera models added

## Version 2.3.0
- get/set_image_transfer_mode function removed
- additional flags  
- bugfixes 

## Version 2.0.7
- direct-io
- workaround for pco.edge DWORD aligned commands, Cancel handling
- image thread priority and process priority can be changed with ini-file
- bugfixes

## Version 2.0.4
- installer script with cypress boot device
- transfer setup changed from fix to round-robin
- improved handling for device disconnect and reconnect
- transfer blocksize can be changed with ini-file
- bugfixes

## Version 2.0.0
- new installer
- new cypress WHQL driver 
  (cyusb3.sys Version 1.2.3.20)
- W10 cameras are listed in class USB devices   
- W7 cameras are listed in class PCO cameras   
- not used functions removed
- bugfixes 

## Version 1.12.0
- new installer

## Version 1.11.0
- device enumeration 
- new cypress lib

## Version 1.9.0
- bugfixes

## Version 1.8.0
- new installer
- new cypress lib
- bugfixes

## Version 1.7.0
- buffer handling with additional threads
- bugfixes 

## Version 1.0.0
- first release
- cypress driver 
  (cyusb3.sys Version 1.2.2.0)

## KNOWN PROBLEMS:
Images will be lost while streaming, if image request is not send in due time.  
Can occure, when image-thread does not get a time slot from OS for longer time.


```
 Excelitas PCO GmbH
 DONAUPARK 11
 93309 KELHEIM / GERMANY
 PHONE +49 (9441) 20050
 FAX   +49 (9441) 200520
 info@pco.de, support@pco.de
 http://www.pco.de
``` 
```
 DISCLAIMER
 THE ORIGIN OF THIS INFORMATION MAY BE INTERNAL OR EXTERNAL TO PCO.
 PCO MAKES EVERY EFFORT WITHIN ITS MEANS TO VERIFY THIS INFORMATION.
 HOWEVER, THE INFORMATION PROVIDED IN THIS DOCUMENT IS FOR YOUR
 INFORMATION ONLY. PCO MAKES NO EXPLICIT OR IMPLIED CLAIMS TO THE
 VALIDITY OF THIS INFORMATION.
-------------------------------------------------------------------
 Any trademarks referenced in this document are the property of
 their respective owners.
```
