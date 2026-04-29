# Excelitas PCO GmbH   -     TECHNICAL  INFORMATION  DOCUMENT

# VERSION HISTORY

## Version 5.6.0
- Distinguish between  warning and error
- Configuration for PCO_GetAcquireModeEx
- Record_flags: dpcore, doubleimg split
- Removed separated recording modes for dpcore, is now in the record flags
- Trigger function
- Bugfixes

## Version 5.5.0
- Added XCite example
- Bugfix in resetting current image number
- Add check for buffer errors in blocking sequence mode
- Add descriptor flag for trigger mode extexpctrl
- Update metadata struct
- Check for descriptor reload flag in arm camera

## Version 5.4.0
- Added xcite class and example to control XCite light sources
- Refactor project file and folder structures
- Added support for changing pixel foramat and handle cameras with 8 bit raw data

## Version 5.3.1:
- Updated sdk libraries
- Bugfixes for calling images() during record
- images() with ring buffer during record no longer supported

## Version 5.3.0:
- Added new functionalities
  - Setting HWIO configurations
  - Open a camera by serial number
  - Adding auto exposure functionality
  - Added binning mode setting
  - Added additional convert flags

## Version 5.2.1:
- Updated windows CameraLink driver to avoid opening the same CL camera twice

## Version 5.2.0:
- Added camram functionality to camera class
- Added Camram examples
- Bugfixes
- Advanced testing using unittest framework
+ Known Issues:
  - Strange color images for non-imaq on laptops/surface tablets
  - Sporadic errors when saving bmp images in the non-imaq saveMean example

## Version 5.1.1
- Bugfix for opening multiple cameras on the same interface

## Version 5.1.0
- Removed dependencies to the NI Vision Common Resources
- Fixed broken connection in PCO_GetMetadataExtern.vi
- Added checking color limits for 8 bit color conversion (similar to pco.camware)

## Version 5.0.0:
- Completely restructured to high level camera class
- Image outputs optimized for converting to IMAQ images
- New set of examples
- New manual
- Old workflows are still possible

## Version 4.5.1:
- Moved to patch release 2.4.1 of pco.recorder because of problems with PCO_RecorderSetCompressionParams in 32Bit LabVIEW

## Version 4.5.0:
- Added initial version of pco.camera Camera class as new module
  + Encapsulation of sdk, recorder and convert SDKs for a very simple camera setup and image acquisition
  + Including examples for pco.camera module  
    These have mainly identical functionality than the existing ones,  
    but the block diagram is much simpler due to the use of the camera class
- Moved all examples to Example subfolder
- Updated convert sdk due to new dll version
- Added AI Denoiser option for color conversion
- Updated color and pseudocolor examples due to new PCO_Convert
- Switched to pco.sdk version 126
- Switched to pco.recorder version 204

## Version 4.4.0:
- Added DLL paths check for running in LabVIEW and running as executable
- Switched to pco.sdk version 125
- Switched to pco.recorder version 203
- Added DICOM and MULTIDICOM file type for pco_example.vi
- Combined 32 and 64 Bit DLLs, it is now possible to switch between 32/64 Bit without changing anything
  --> Added singel and multi Dicom file format for file mode of recorder
  --> Added function to save single images as files for various formats
  --> Added function to create and save an RGB image from an overlay of 3 monochrome images
  --> Added PCO_RecorderCopyAverageImage and PCO_RecorderCopyImageCompressed functions
- Added new VIs for file save functions
- Added new VIs for new copy image functions
- Bugfixes and optimizations in examples

## Version 4.3.0:
- Added pco.dicam functions sdk functions
- Added Get/SetFanControl sdk functions
- Adapted some controls and indicators to view hexadecimal values
- Bugfixes in examples

## Version 4.2.0:
- Switched to pco.recorder version 202 because of bugfixes

## Version 4.1.0:
- Switched to pco.recorder version 201 because of bugfixes
- Bugfixes in pco.labview_Example.vi

## Version 4.0.0:
- Integration of pco.convert into LabVIEW
- Switched to pco.sdk version 124
- Switched to pco.recorder version 200
  --> Added support of cameras with internal memory
  --> Added support of auto exposure functionality
- Added several new example VIs for color conversion
- Added several new example VIs for cameras with internal memory
- Library paths now specified in block diagram to avoid location warnings

## Version 3.0.0:
- First new version
- Completely recreated

```
 Excelitas PCO GmbH
 DONAUPARK 11
 93309 KELHEIM / GERMANY
 PHONE +49 (9441) 20050
 FAX   +49 (9441) 200520
 pco@excelitas.com
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
