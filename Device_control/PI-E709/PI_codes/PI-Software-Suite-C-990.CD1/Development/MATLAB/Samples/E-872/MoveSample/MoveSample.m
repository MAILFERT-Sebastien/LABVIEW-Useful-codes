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
%   To check if the PI_MATLAB_Driver works properly, run the sample 
%   BeginWithThisSampleForTestingMatlabDriver.m
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   !! MOTION WARNING !!
%
%   Description:
%
%   This sample loads the MATLAB driver and connects a E-872 controller using TCP/IP, USB or RS232.
%   It shows the connected actuator and moves two axis of the actuator.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Setup configuration

% You MUST EDIT AND ACTIVATE the parameters to make your system run
% properly. 
% 1. Activate the connection type
% 2. Set the connection settings

% Connection type
use_TCPIP_Connection                = true;
use_USB_Connection                  = false;

% Connection settings
% USB
controllerSerialNumber = 'Enter valid serial number here, e.g. "123456789"'; % Use "devicesUsb = Controller.EnumerateUSB('')" to get all PI controller connected to your PC. Or look at the label of the case of your controller
% TCP/IP
ip = 'XXX.XXX.XXX.XX';  % Use "devicesTcpIp = Controller.EnumerateTCPIPDevices('')" to get all PI controller available on the network
port = 50000;           % Is 50000 for almost all PI controllers

%% Load PI MATLAB Driver GCS2
isWindows   = any (strcmp (mexext, {'mexw32', 'mexw64'}));

if(~isdeployed) % Determine whether code is running in deployed or MATLAB mode
    if (isWindows)
        addpath (getenv ('PI_MATLAB_DRIVER'));
    else
        addpath ( '/usr/local/PI/pi_matlab_driver_gcs2' );
    end
end

% Load PI_GCS_Controller if not already loaded
if(~exist('Controller','var'))
    Controller = PI_GCS_Controller();
end
if(~isa(Controller,'PI_GCS_Controller'))
    Controller = PI_GCS_Controller();
end

%% Start connection
% (if not already connected)
try
    boolPIdeviceConnected = false; 
    if ( exist ( 'PIdevice', 'var' ) ), if ( PIdevice.IsConnected ), boolPIdeviceConnected = true; end; end
    if ( ~(boolPIdeviceConnected ) )

    % USB
        if(use_USB_Connection)
            PIdevice = Controller.ConnectUSB ( controllerSerialNumber );
        end
    % TCP/IP
        if(use_TCPIP_Connection)
            PIdevice = Controller.ConnectTCPIP ( ip, port ) ;
        end

    end

    % query controller identification string
    connectedControllerName = PIdevice.qIDN();

    % initialize PIdevice object for use in MATLAB
    PIdevice = PIdevice.InitializeController ();


    %% Show connected stages

    % query names of all controller axes
    availableAxes = PIdevice.qSAI_ALL

    % show for all axes: which stage is connected to which axis
    for idx = 1 : length ( availableAxes )
        % qCST gets the name of the stages
        stageName = PIdevice.qCST ( availableAxes { idx } );
        disp ( [ 'Axis ', availableAxes{idx}, ': ', stageName ] );
    end

    % if the stages listed are wrong use one of the following solutions:
    % - PIMikroMove (recommended, but MS windows only)
    % - PITerminal  (use VST, qCST, CST and WPA command as described in the Manual)
    % - Use function "PI_ChangeConnectedStage"


    %% Move axis 1 and axis 2

    axis = [1, 2];
    commandedStepsForward = 1000;
    commandedStepsBackward = -1000;


    for idx = 1 : length ( axis )

        disp ( ['move axis ', axis(idx), 'forward']);
        PIdevice.OSM ( axis(idx), commandedStepsForward );

        % wait for motion to stop
        disp('missing steps:')
        while(0 ~= PIdevice.IsMoving ( num2str(axis(idx)) ) )
            pause ( 0.1 );
            missingSteps = PIdevice.qOSN ( axis(idx) );
            fprintf('%d\n', missingSteps);
        end

        disp ( ['move axis ', axis(idx), 'backward']);
        PIdevice.OSM ( axis(idx), commandedStepsBackward );

        % wait for motion to stop
        disp('missing steps:')
        while(0 ~= PIdevice.IsMoving ( num2str(axis(idx)) ) )
            pause ( 0.1 );
            missingSteps = PIdevice.qOSN ( axis(idx) );
            fprintf('%d\n', missingSteps);
        end
    end


    %% If you want to close the connection
    PIdevice.CloseConnection;


    %% If you want to unload the PI MATLAB Driver GCS2
    Controller.Destroy;
    clear Controller;
    clear PIdevice;
catch
    Controller.Destroy;
    clear Controller;
    clear PIdevice;
    rethrow(lasterror);
end