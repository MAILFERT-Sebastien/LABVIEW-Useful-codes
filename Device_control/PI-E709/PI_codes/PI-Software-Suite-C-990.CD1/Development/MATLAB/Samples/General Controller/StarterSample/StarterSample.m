%   (c)2015-2020 Physik Instrumente (PI) SE & Co. KG
%   Software products that are provided by PI are subject to the General Software License Agreement of
%   Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components. 
%   For more information, please read the General Software License Agreement and the Third Party Software Note linked below.
%
%   General Software License Agreement :
%   http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
%   Third Party Software Note :
%   http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   IMPORTANT NOTICE
%
%   This sample only works with a PI_MATLAB_Driver version 1.5.0 and newer.
% 
%   You must always initialize the system first. The steps necessary depend on the type of mechanics (e.g., positioner, actuator) 
%   and electronics (e.g., controller, drive electronics). 
%   It may be necessary to load positioner parameters from a database and/or perform a homing procedure (e.g., auto zero procedure, reference move).
%   Refer to the manuals of your mechanics and electronics for details.
%
%   For the initialization use PIMikroMove on Windows or PITerminal on Linux.
%
%   - Load positioner parameters via "Start Up Controller" in PIMikroMove 
%   or via the CST command in PITerminal. 
%
%   - Execute the homing procedure in PIMikroMove via "Start Up Controller" -> "Start up axes".
%   Alternatively, use the corresponding sample (AutoZeroSample, ReferenceSample, ReferenceWithEAXSample). 
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Description:
%
%   This sample demonstrates how to
%   - load the PI_MATLAB_Driver_GCS2
%   - get the PI_MATLAB_Driver_GCS2 Version Number
%   - unload the PI_MATLAB_Driver_GCS2
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Load PI_MATLAB_Driver_GCS2

isWindows   = any (strcmp (mexext, {'mexw32', 'mexw64'}));

if (isWindows)
    matlabDriverPath = getenv ('PI_MATLAB_DRIVER');
    if (~exist(matlabDriverPath,'dir'))
        error('The PI MATLAB Driver GCS2 was not found on your system. Probably it is not installed. Please run PISoftwareSuite.exe to install the driver.');
    else
        addpath(matlabDriverPath);
    end
else
    if (~exist('/usr/local/PI/pi_matlab_driver_gcs2','dir'))
        error('The PI MATLAB Driver GCS2 was not found on your system. If you need the MATLAB driver for Linux please contact the service.');
    else
        addpath ('/usr/local/PI/pi_matlab_driver_gcs2');
    end
end

% Load PI_GCS_Controller if not already loaded
if(~exist('Controller','var'))
    Controller = PI_GCS_Controller();
end
if(~isa(Controller,'PI_GCS_Controller'))
    Controller = PI_GCS_Controller();
end

%% Get the PI_MATLAB_Driver_GCS2 Version Number
if (isWindows)
    Controller.GetVersionNumber()
end
disp ('If this message is shown PI MATLAB driver is set up properly!');


%% if you want to unload the dll and destroy the class object
Controller.Destroy;
clear Controller;