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
%   This sample connects to a controller using TCP/IP, USB or RS232. The connection type can be easily changed.
%   Then the function PI_UseDataRecorder is called to configure the datarecorder and samples of a step are captured.
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function DataRecorderSample
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


    % Please choose the right configuration for your setup, there is one common configuration
    % working with most of PI controllers and one configuration for the Hexapod controller C-887

    axesSettings = 'Common'; % 'Common' 'C-887'
    switch axesSettings
        case 'Common'
            axis = '1';
        case 'C-887'
            axis = 'X';
    end
    
    % data recorder settings
    config.iTableIds          = [ 1 2 ]; % record table to be used
    config.iOption            = [ 1 2 ]; % record options (1 = commanded position, 2 = measured position)
    config.iTableRate         = 1;       % record table rate (equal to output rate of wave generator)
    config.iTriggerSource     = 1;       % 0 - default, data recording is triggered with IMP; 1 - any command changing target position or open-loop control value
    config.sTriggerOption     = '0';     % trigger option is set to a dummy value, only used for certain trigger sources (see manual for details)

    stepsizeOfRelativeMove = 1;

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

        % query controller identification string
        connectedControllerName = PIdevice.qIDN();

        % initialize PIdevice object for use in MATLAB
        PIdevice = PIdevice.InitializeController ();


        %% Startup stage
        switchOn = 1;
        PIdevice.SVO ( axis, switchOn );

        %% Start data recorder

        PI_UseDataRecorder (PIdevice, axis, stepsizeOfRelativeMove, config);
        %% Switch servo off
        switchOff    = 0;
        PIdevice.SVO ( axis, switchOff );

        %% If you want to close the connection
        PIdevice.CloseConnection ();

        %% If you want to unload the dll and destroy the class object
        Controller.Destroy ();
        clear Controller;
        clear PIdevice;
    catch
        Controller.Destroy;
        clear Controller;
        clear PIdevice;
        rethrow(lasterror);
    end
end 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  PI_UseDataRecorder
%
%  This sample function shows how to use the data recorder. The data
%  recorder is described in detail in the user manual. Please refer to it to
%  get the configuration you need.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function PI_UseDataRecorder ( PIdevice, axis, stepsizeOfRelativeMove, config )
    %% Setup Data Recorder

    % configure Record Table Rate
    disp('Configuring data recorder...');
    PIdevice.RTR( config.iTableRate );

    % configure which data is recorded by the data recorder
    PIdevice.DRC ( config.iTableIds, axis, config.iOption );


    % configure which event triggers the data recorder (starts the data acquisition):
    % for all controllers, except a few, the RecordChannelIdArray for DRT starts with '0'
    % 0 - default, data recording is triggered with IMP
    % 1 - any command changing target position or open-loop control value

    supportedDRTTableId = -1;
    for drtTableId=0:1
    try
        PIdevice.DRT ( drtTableId, config.iTriggerSource, config.sTriggerOption );
        supportedDRTTableId = drtTableId;
    catch exception
        disp (exception.message);
    end

    if(supportedDRTTableId ~= -1)
        break;
    end
    end

    if(supportedDRTTableId == -1)
    error('Unable to get supported record channel id for DRT');
    end


    %% Perform move

    if ( PIdevice.qTMX ( axis ) < ( PIdevice.qPOS ( axis ) + stepsizeOfRelativeMove ) )  % make sure the relative move will not exceed the travel range
    stepsizeOfRelativeMove = stepsizeOfRelativeMove * ( -1 );
    end

    pause ( 0.1 );
    PIdevice.MVR (axis, stepsizeOfRelativeMove)


    %% Read data from controller

    % wait until stage has finished motion. Afterwards get the number of Points
    % recorded while stage has moved.
    while ( 0 ~= PIdevice.IsMoving ( axis ) )
    pause ( 0.1 );
    end


    try
    numberOfPoints  = PIdevice.qDRL ( 1 ); 
    
    catch exception
    disp(exception.message);
    disp('Command qDRL is not supported. Set numberOfPoints to 200')
    numberOfPoints = 200;
    end
    % % Set a predefined number of points you want to retrieve from controller.
    % % Wait until controller has recorded this number of points. WARNING: By
    % % setting numberOfPoints to a greater value than the controllers data
    % % recorder length (see controller manual) you will create an infinite loop.
    % numberOfPoints = 1000;
    % while (PIdevice.qDRL(1) < numberOfPoints) 
    %     pause(0.02);                          
    %                                          
    % end


    startPoint      = 1;
    disp('Retrieving data from controller...');
    data = PIdevice.qDRR ( config.iTableIds, startPoint, numberOfPoints ); 
    disp('Retrieving data finished.');

    time                = data ( :, 1 );
    commandedPosition   = data ( :, 2 );
    actualPosition      = data ( :, 3 );

    %%  plot data
    figure(1);

    plot ( time, commandedPosition, time, actualPosition );
    title ( 'closed loop' );
    grid on;

    title ( 'Plot of a relative move' );
    xlabel ( 'time /s' );
    ylabel ( 'position / mm' );
    legend ( 'target position', 'current position' );

end
