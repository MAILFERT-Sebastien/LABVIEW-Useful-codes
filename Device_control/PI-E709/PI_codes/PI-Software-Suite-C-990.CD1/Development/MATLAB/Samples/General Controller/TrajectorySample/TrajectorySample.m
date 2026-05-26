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
%   This sample connects to a controller using TCP/IP, USB or RS232.
%   After that, a trajectory is setup, transferred to the controller and started.
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Setup configuration

% You MUST EDIT AND ACTIVATE the parameters to make your system run
% properly.
% 1. Activate the connection type
% 2. Set the connection settings

% Connection type
use_RS232_Connection                = false;
use_TCPIP_Connection                = true;
use_USB_Connection                  = false;

% Connection settings
% RS232
comPort = 1;          % Look at the Device Manager to get the correct COM Port
baudRate = 115200;    % Look at the manual to get the correct baud rate for your controller
% USB
controllerSerialNumber = 'Enter valid serial number here, e.g. "123456789"'; % Use "devicesUsb = Controller.EnumerateUSB('')" to get all PI controller connected to your PC. Or look at the label of the case of your controller 
% TCP/IP
ip = 'XXX.XXX.XXX.XXX';  % Use "devicesTcpIp = Controller.EnumerateTCPIPDevices('')" to get all PI controller available on the network
port = 50000;            % Is 50000 for almost all PI controllers

    
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

    % RS232
        if (use_RS232_Connection)
            if (isWindows)
                PIdevice = Controller.ConnectRS232 ( comPort, baudRate );
            else
                DeviceName = '/dev/ttyS0';  % Select the serial device
                PIdevice = Controller.ConnectRS232ByDevName ( DeviceName, baudRate );
            end
        end

    % USB
        if(use_USB_Connection)                                      
            PIdevice = Controller.ConnectUSB ( controllerSerialNumber );
        end
    % TCP/IP
        if(use_TCPIP_Connection)
            PIdevice = Controller.ConnectTCPIP ( ip, port ) ;
        end
    end

    %% Init controller

    % query controller identification string
    connectedControllerName = PIdevice.qIDN ()

    % initialize PIdevice object for use in MATLAB
    PIdevice = PIdevice.InitializeController ();

    axesCellArray = PIdevice.qSAI_ALL();

    axes = [axesCellArray{1}, ' ', axesCellArray{2}];
    axis1 = axesCellArray{1};
    axis2 = axesCellArray{2};


    %% Prepare controller
    disp('Prepare controller');

    % switch on servo mode 
    disp('switch on servo mode');

    PIdevice.SVO(axis1, 1);
    PIdevice.SVO(axis2, 1);

    %% Prepare trajectory
    disp('Prepare trajectory');


    % define trajectory parameters
    centerPoint_X =(PIdevice.qTMN(axis1) + PIdevice.qTMX(axis1)) / 2;
    centerPoint_Y =(PIdevice.qTMN(axis2) + PIdevice.qTMX(axis2)) / 2;
    amplitude = 1;
    numberOfTrajectoryPoints = 100;
    period = 5; % period in seconds

    % trajectory IDs
    trajectoryIDs = [1, 2];

    % calculate trajectory values for circular motion
    argumentVektor = (0:numberOfTrajectoryPoints-1)/numberOfTrajectoryPoints;
    trajectory_X = centerPoint_X + amplitude * sin(2*pi*argumentVektor);
    trajectory_Y = centerPoint_Y + amplitude * cos(2*pi*argumentVektor);

    % plot trajectory
    figure(1);
    plot(trajectory_X, trajectory_Y, '.-');
    grid on;
    axis equal;
    xlabel('axis x');
    ylabel('axis y');


    %% Prepare controller
    disp('Prepare controller');

    % move to start position for circular motion
    disp('move to start position for circular motion');

    PIdevice.MOV(axis1, trajectory_X(1));
    PIdevice.MOV(axis2, trajectory_Y(1));

    % wait for axis to reach target position
    while( any(PIdevice.IsMoving ( axes )) )                        
        pause(0.1);     
        fprintf('.');
    end

    % set timing for given period in servo cycles per point
    parameterID = hex2dec('E000200');  % parameter ID is hexadecimal
    servotime = PIdevice.qSPA(axis1, parameterID);

    trajectoryRate = round(period / numberOfTrajectoryPoints / servotime);
    PIdevice.TGT(trajectoryRate);

    % clear all trajectories on controller
    PIdevice.TGC(trajectoryIDs);


    %% Init trajectory
    disp('init trajectory');

    % parameterID = hex2dec('22000020');  % parameter ID is hexadecimal
    % bufferSize = PIdevice.qSPA(axis1, parameterID);
    bufferSize = 50;

    % usually you can not write all values to the controller's buffer
    % initially. So numberOfValuesToWriteInitially will be bufferSize.

    if any(length(trajectory_X) >= bufferSize)
        numberOfValuesToWriteInitially = bufferSize;
    else
        numberOfValuesToWriteInitially = length(trajectory);
    end

    % fill buffer initially
    for idx = 1 : numberOfValuesToWriteInitially
        PIdevice.TGA(trajectoryIDs(1), trajectory_X(idx));
        PIdevice.TGA(trajectoryIDs(2), trajectory_Y(idx));
    end


    %% Run trajectory
    disp('run trajectory');

    % start trajectories after buffer is filled
    disp('start trajectories after buffer is filled');
    PIdevice.TGS(trajectoryIDs);

    while 1
    if idx == length(trajectory_X)
        break;
    end
        % get number of trajectory points currently stored on controller
        valuesInControllerBuffer = PIdevice.qTGL(trajectoryIDs);

        if( any ( valuesInControllerBuffer < bufferSize ) )
            % append points to trajectories on controller
            PIdevice.TGA(trajectoryIDs, [trajectory_X(idx), trajectory_Y(idx)]);
            idx = idx + 1;
        else
            pause (0.01);
        end
    end

    % tell the controller that no more points are added to the trajectories and
    % wait for controller to finish trajectories
    PIdevice.TGF(trajectoryIDs);

    while any(valuesInControllerBuffer)
        valuesInControllerBuffer = PIdevice.qTGL(trajectoryIDs);
        pause(0.1);
    end
    disp('trajectory finished');


    %% If you want to close the connection

    % switch off servo mode 
    disp('switch off servo mode');

    PIdevice.SVO(axis1, 0);
    PIdevice.SVO(axis2, 0);

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