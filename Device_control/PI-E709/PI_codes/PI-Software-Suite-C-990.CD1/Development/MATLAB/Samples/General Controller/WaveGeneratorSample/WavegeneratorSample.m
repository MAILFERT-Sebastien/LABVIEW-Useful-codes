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
%   Then the wavegenerator is configured and started.
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function WavegeneratorSample
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
    ip = 'XXX.XXX.XX.XX';   % Use "devicesTcpIp = Controller.EnumerateTCPIPDevices('')" to get all PI controller available on the network
    port = 50000;           % Is 50000 for almost all PI controllers


    % Please choose the right configuration for your setup, there is one common configuration
    % working with most of PI controllers and one configuration for the Hexapod controller C-887,
    % and anoter configuration for E-727
    
    % wave generator default configuration 

    wgConfig.iId                             = 1;       % id of wave generator
    wgConfig.iWaveTableID	      	         = 1;       % id of wave table
    wgConfig.iStartMode                      = 1;       % start mode = 1 (start wave generator output synchronized by servo cycle)
    wgConfig.iNumCycles                      = 5;       % number of wave generator cycles
    wgConfig.dOffsetOfWave					 = 0.0;     % offset of wave
    wgConfig.ifrequencyOfWave                = 2;       % frequency of wave
    wgConfig.iInterpolationType              = 1;       % interpolation between points, used if piTableRate > 1. 1 = linear interpolation
    wgConfig.iOffsetOfFirstPointInWaveTable	 = 0;       % index of starting point of curve in segment
    wgConfig.iAddAppendWave					 = 0;       % 0=clear wave table (1=add wavetable values, 2=append to existing wave table contents)
    wgConfig.bE727                           = false;   % false, if you are not using an E-727 Controler
    
    waveTableSettings = 'Common'; % 'Common' 'C-887' 'E-727'
    switch waveTableSettings
        case 'Common'
            axis = '1';
            wgConfig.iTableRate  = 2;                   % number of servo cycles per point in wave table
            
        case 'C-887'
            axis = 'X';          
            wgConfig.iTableRate  = 10;                  % number of servo cycles must be an integer multiple of 10 for C-887

        case 'E-727'
            axis = '1';
            wgConfig.iTableRate  = 2;    
            wgConfig.bE727       = true;                % true, if you are using an E-727 Controler
    end

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


        %% Using the wavegenerator       

        % check if controller has a wave generator
        noWaveGenerators = PIdevice.qTWG();
        if (noWaveGenerators < 1)
            error('Your currently connected controller has no wave generators.')
        end 

        % switch on servo mode 
        disp('switch on servo mode');
        PIdevice.SVO(axis, 1);

        PI_UseWaveGenerator (PIdevice, axis, wgConfig);


        %% Disconnecting 

        % switch off servo mode 
        disp('switch off servo mode');
        PIdevice.SVO(axis, 0);

        PIdevice.CloseConnection;


        %% Unloading the PI_MATLAB_Driver_GCS2
        Controller.Destroy;
        clear Controller;
        clear PIdevice;
       
    catch
        Controller.Destroy;
        clear Controller;
        clear PIdevice;
        rethrow(lasterror);
    end
end


%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  PI_UseWaveGenerator.m
%
%  This sample function shows how to use the wave generator. The wave
%  generator is described in detail in the user manual. Please refer to it to
%  get the configuration you need.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function PI_UseWaveGenerator(PIdevice, axis, wgConfig)
    % calculate amplitude
    minPosLimit = PIdevice.qTMN (axis);
    maxPosLimit = PIdevice.qTMX (axis);
    dAmplitudeOfWave = (abs(minPosLimit) + abs(maxPosLimit))/4;

    % query servo cycle time
    PARAM_ServoUpdateTime = 234881536;
    servoCycleTime = PIdevice.qSPA (axis,PARAM_ServoUpdateTime);
    
    
    % calculate number of point in wavetable from given frequency
    iNumberOfPoints = int32 (1/(servoCycleTime * wgConfig.ifrequencyOfWave));

    % wavetable contains one segment
    iSegmentLength = iNumberOfPoints;

    % curve center point is the middle of the segment
    iCenterPointOfWave = iNumberOfPoints / 2;
    
    % send wave table to controller
    PIdevice.WAV_SIN_P(wgConfig.iWaveTableID,... 
                       wgConfig.iOffsetOfFirstPointInWaveTable,... 
                       iNumberOfPoints,...
                       wgConfig.iAddAppendWave,...
                       iCenterPointOfWave,...
                       dAmplitudeOfWave,...
                       wgConfig.dOffsetOfWave,...
                       iSegmentLength);

    % link wave table to wave generator
    PIdevice.WSL(wgConfig.iId, wgConfig.iWaveTableID);

    % set wave table rate
    if(~wgConfig.bE727)
        PIdevice.WTR (wgConfig.iId, wgConfig.iTableRate, wgConfig.iInterpolationType);
    else
        PIdevice.WTR (0, wgConfig.iTableRate, wgConfig.iInterpolationType);     % for this command the WaveGenerator ID of the E727 Controller has to be 0
    end
    
    % set wave generator output cycles
    PIdevice.WGC(wgConfig.iId, wgConfig.iNumCycles);

    
    disp('Move to start position');
    % move to start position of sine-waveform
    % !! Adjust start-position when changing waveform or assignment !!
    PIdevice.MOV(axis, wgConfig.dOffsetOfWave);
    % wait for motion to stop
    while(0 ~= PIdevice.IsMoving())
        pause(0.1);
    end

    pause(1)

    % restart recording as soon as wave generator output starts running
    PIdevice.WGR ();

    % start wave generator output, data recorder starts simultaneously
    disp('Start wavegenerator');
    PIdevice.WGO(wgConfig.iId, wgConfig.iStartMode);

    % wait for Wave Generator to finish
    ret = true;
    while ( (ret == true)) 
        ret = PIdevice.IsGeneratorRunning(wgConfig.iId);
        pause(0.1);
    end

    disp('done')
end


