The MATLAB sample files will not run until you have installed the PI MATLAB Driver GCS2.

----------------------------------------------------------------------------------------------------------
Linux:

For Linux operating systems, a beta version of the PI MATLAB Driver GCS2 is available on request. Contact support@pi.ws if you need it.

----------------------------------------------------------------------------------------------------------

Windows:

Proceed as follows to use the PI MATLAB Driver GCS2 on Windows systems:

1. Read:	The manual for the PI MATLAB Driver GCS2.
			> Download links: 
                          https://www.physikinstrumente.com/fileadmin/user_upload/physik_instrumente/files/Compact_Downloads_for_Software_CD/PI_MATLAB_Driver_GCS2_Manual_SM155D.pdf
                          https://www.physikinstrumente.com/fileadmin/user_upload/physik_instrumente/files/Compact_Downloads_for_Software_CD/PI_MATLAB_Driver_GCS2_Manual_SM155E.pdf

2. Install from your PI Software CD:     PISoftwareSuite.exe
			> This installs all important PI software to your PC (recommended).
                        > If you do not want to perform the complete installation, you must run at least the following setup programs:
                          PIAPIComponents.exe
                          PIBaseComponents.exe
                          PIUSBDriversWin32/64.exe

3. Update:	Update the PI MATLAB Driver GCS2 via PIUpdateFinder.
			> This is recommended but not required.

4. Run: 	BeginWithThisSampleForTestingMatlabDriver.m
			> Test if everything is installed properly. This is recommended but not required.

5. Initialize your system:  The steps necessary depend on the electronics (e.g., controller, drive electronics) and the connected mechanics (e.g., positioners, actuators).
                        > Use PIMikroMove. It may be necessary to load positioner parameters from a database and/or perform a homing procedure (e.g., auto zero procedure, reference move). Refer to the manuals of your mechanics, electronics, and PIMikroMove for details.
                        > Close the connection in PIMikroMove before running any samples.

6. Open:        Open any sample in MATLAB.
                        > Read the manual of your electronics to find out which functions are supported (e.g., trajectories for motion paths, wave generator).			               
                        > Except in special cases you can use the samples from the "General Controller" folder for all electronics.
                        > If a sample is located in the "<YourControllerType>" folder as well as in the "General Controller" folder, use the sample from the "<YourControllerType>" folder.
                        > If you want to edit the sample, you should copy it to your PC.  
	  
7. Connect:	Make sure that you select the correct connection type in the MATLAB script.
			> Configure the connection settings (RS-232, USB, TCP/IP).

8. Try:         Now you can run the sample.
			
