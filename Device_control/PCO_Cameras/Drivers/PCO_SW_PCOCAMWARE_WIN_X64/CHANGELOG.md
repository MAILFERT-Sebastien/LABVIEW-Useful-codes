# Excelitas PCO GmbH   -     TECHNICAL  INFORMATION  DOCUMENT

## DESCRIPTION
This package includes a setup, which installs the following files
in your selected application directory.

Windows 10, 11:
all files are copied to "APPDIR"/Camware in case of an administator installation
all files are copied to "USER_APPDIR"/Camware in case of a user installation

Where APPDIR is the selected application directory,
e.g. c:\program files\digital camera toolbox\camware.
Where USER_APPDIR is the selected user application directory,
e.g. C:\Users\<user>\AppData\Roaming\Digital Camera Toolbox\Camware.
It is possible to select between both installation methods.

Registry:
Following registry entries will be created during runtime of the software:
- HKCU\\Software\\PCO\\Camware 
- HKCU\\Software\\PCO\\Camware 4
- HKCU\\Software\\PCO\\pco.camera Serial-Nr: <SN> Type: <camera type>

Where pco.camera... depends on the current connected camera(s).
The 'test' and 'logo' files can be loaded into the application to play around with the converter and
different file types.

The 'Application Related' files are the application itself.

The 'Pseudo Color Lookup Table' files can be used to convert b/w images into pseudo color images.


## Installation Instructions:

Unzip pco_camware_4_.._0_Windows.zip to a distinct directory (e.g. 'PCOTEMP')

1.  Select above directory ( e.g. 'PCOTEMP').
2.  Start 'pco_camware_4_18_0_Windows.exe' to run setup.
3.  The necessary files will be installed automatically.
4.  Now you are ready to run the camera control software.

If you have problems during the installation, please check your firewall and / or disable it.

# VERSION HISTORY

Version 4.20.0:
------------------------------
- Added global Firmware Version Info for modern cameras
- Added averaging 512, 1024
- Added single image auto save during recording for C4/C8
- Bugfixes

Version 4.19.0:
------------------------------
- Added C4/C8 vector correction
- Added Line Scan Mode

Version 4.18.0:
------------------------------
- Finally got it: Menu resides on caption bar now (optionally)
- DS images of Cx can be saved to a single stream
- Enabled synch for zoom (View window - menu) and convert ctrl settings (Histogram - CTRL key)
- Threaded ARM for Cx speeds up processing
- Various performance improvements for Cx
- Number of buffers used for image transfer can be set now (default: 4)

Cx means pco.dicam C4-C8

Version 4.17.0:
------------------------------
- fixed scan for light source
- fixed crash when saving multiple images into one stream
- added setup for number of buffers

Version 4.16.1:
------------------------------
- fixed crash in bitmap creation
- fixed DS in demo mode

Version 4.16.0:
------------------------------
- Auto switch Xcite light source
- Added soft fps
- bugfixes

Version 4.15.0:
------------------------------
- upgraded meas tool
- bugfixes

Version 4.14.0:
------------------------------
- added OpenCL conversion
- bugfixes

Version 4.13.0:
------------------------------
- added 8bit sensor bit depth functionality
- added battery information
- added new time calculation Exp = min Exp + N * Expstep
- added time control dialog
- added sort for cameras based on SN (or current number when sorting occured once)
- added plugin dll for customized calculation
- bugfixes

Version 4.12.0:
------------------------------
- Added Compression mode for dimax
- Hide preview teaser for current session
- Preserve window position using registry
- Added auto arrange of mdi windows
- Added customizable Custom Setup by reg file
- Added Color Coded Image Overlay
- Bugfixes

Version 4.11.0:
------------------------------
- Website check for new pco.camware version
- Material number info added to camera properties
- Metadata added to overlay
- New convert properties in view window
- Display GPU information
- Preview shows overlay 
- Added modulate mode to camera properties
- Re-worked line tool (exact pos., new hist window)
- Display min, max and mean values in line tool
- Status button added to camera properties
- Enhanced visibility for tracker window
- Double click inside view opens Save Dialog
- Added Reset Settings To Default in cameralistview
- Display sensor color capabilities in Status
- Display information when tracker is active
- Bugfix

Version 4.10.0:
------------------------------
- Show temperatures in a graph
- Enabled live transfer of exposure and delay settings during record
- Show histogram in a tool window
- Slider for time settings
- E-key issues 'Auto Exposure'; Can be controlled by option 'Auto Exp. Target' (range 10-75%)
- Double click into recorder toolbar stops building thumbnails
- Improved support for pco.dicam Cx
- Dark room mode
- Show 'Estimated Record Time' in properties (based on current timing, size of recording memory and image size)
- Show enhanced LUT dialog
- Load convert settings from a single file (when opening)
- Show an 'Acquire Button' in Recorder Tools if appropriate
- Added some file types to Direct-Record-To-File
- Added DCM file format
- Enhanced Zoom to 64 and show a grid above zoom 8
- Added scale factor to show distances in 'Line Profile'
- Added option (dialog) to keep 'Microscope Save Mode' throughout the whole session
- Setup for GUI color when recording (can be switched off or change the color)
- Fan/LED settings are now available in 'Camera Properties'
- Maximize/Restore of undocked view windows (teared off)
- ROI can be forced to be symmetrical
- Color when Recordstate=ON can be changed
- Bugfix

Version 4.7.0:
------------------------------
- C4 bugfix release

Version 4.6.0:
------------------------------
- Common Settings for identical cameras (more than one)
- Added functionality for pco.dicam C1/C4
- Added Fan/LED Dialog
- Tear off for MDI windows
- Added support for KAYA grabber
- Bugfix

Version 4.5.0:
------------------------------
- Overlapping readout of images during file save
- HDSDI interface setup for dimax CS
- Force Camware to demo mode
- Auto restart record when Autosave is active
- New file types for Direct Record To File
- Preview with Ext Signals
- List File Save ROI in rec file
- Lens Control Dialog
- Bugfix

Version 4.4.0:
------------------------------
- Added Status Information to camera properties
- Multi Window
- Math Toolbar
- Segment switcher
- Flim camera setup dialog
- Control key and left mouse click sets to either first or last image, right mouse and CTRL sets to in/out
- Added Camware related image numbering
- Added stop condition, which breaks out of the scan loop when a distinct number of cameras is active (hit e.g. '2' during scan)
- View window properties
- Added file numbering to auto save
- Bugfix

Known issues:
- Multiwindow does not show images in a proper alternating way.
- T0 position is not read back correctly when opening a previously saved sequence.

Version 4.3.0:
------------------------------
- Bugfix Release

Version 4.2.0:
------------------------------
- Master Setting for multi camera operation
- Crosshair
- Rotate Left/Right 90
- Extract T0 from recorded sequence
- Added library support to file dialog
- Options Dialog
- Auto-Save Dialog
- Bugfixing

Version 4.1.0:
------------------------------
- added PNG file format for export
- added Settings Overview
- added Infotext window for Rec file
- added Image Overlay
- added Reverse Play
- added Damping
- added Averaging
- added Low Light Preview mode
- bugfixes

Version 4.0.0:
------------------------------
- first release

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
