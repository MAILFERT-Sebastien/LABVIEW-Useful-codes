%   (c)2021 Physik Instrumente (PI) SE & Co. KG
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
%   This sample only works with a PI_MATLAB_Driver version 1.6.0 and newer.
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
%   This sample connects to a GCS 3.0 controller using TCP/IP, USB or RS232. The connection type can be easily changed.
%   Then the data recorder is configured and samples of a step are captured.
%   The stage has to be referenced before starting this sample.
%   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function DataRecorderSample_gcs30
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


    %% Load PI MATLAB Driver GCS3
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
        
        %% Configurations of the data recorder

        recorderId = 'REC_1';
        dataFormat = 'ASCII';
        noOfTraceToRead = 1;
        noOfSamplesToRead = 200;
        containerUnitId = 'AXIS_1';
        closed_loop = 2;
        open_loop = 1;
        recordRate = 30;
        traceIDs = [1,2];
        Parameter1 = {'AXIS_1','-','0x103'};
        Parameter2 = {'AXIS_1','-','0x102'};
        TriggerSettings = {'MOV',containerUnitId,'0'}; 
                             
        SetupDataRecorder(PIdevice,recorderId,recordRate,traceIDs,Parameter1,Parameter2,TriggerSettings)       
        SetModeOfOperation (PIdevice,containerUnitId, closed_loop)        
        DoStep(PIdevice,containerUnitId,1)        
        WaitForRecordingToFinish(PIdevice,recorderId,noOfSamplesToRead)                    
        ResetAxis(PIdevice,containerUnitId)        
        SetModeOfOperation (PIdevice,containerUnitId, open_loop)
        ReadDataRecorderData(PIdevice,recorderId,dataFormat,noOfSamplesToRead,noOfTraceToRead)
                
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  PI_UseDataRecorder
%
%  This sample function shows how to use the data recorder. The data
%  recorder is described in detail in the user manual. Please refer to it to
%  get the configuration you need.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function SetupDataRecorder(PIdevice,recorderId,recordRate,traceIDs,Parameter1,Parameter2,TriggerSettings)
    disp('Configuring data recorder...')
    PIdevice.REC_RATE(recorderId,recordRate)
    PIdevice.REC_TRACE(recorderId,traceIDs(1,1),Parameter1(1,1),Parameter1(1,2),Parameter1(1,3))
    PIdevice.REC_TRACE(recorderId,traceIDs(1,2),Parameter2(1,1),Parameter2(1,2),Parameter2(1,3))     
    PIdevice.REC_TRG(recorderId,TriggerSettings(1,1),TriggerSettings(1,2),TriggerSettings(1,3))   
    disp('Starting data recorder...')
    PIdevice.REC_START(recorderId)
end

function SetModeOfOperation (PIdevice,axis, mode)
    PIdevice.SAM(axis,mode)
    fprintf( 'Set mode of operation %d\n',mode);
end

function DoStep(PIdevice,containerUnitId,amplitude)
    currentTarget = PIdevice.qMOV(containerUnitId);
    targetPosition = currentTarget + amplitude;
    PIdevice.MOV(containerUnitId,targetPosition)
    fprintf( 'Step with amplitude of %d completed.\n',amplitude);
end

function  WaitForRecordingToFinish(PIdevice,recorderId,noOfSamplesToRead)
    disp('Reading...')
    while(noOfSamplesToRead > PIdevice.qREC_NUM(recorderId) )
        pause ( 0.1 );  
    end
    disp('Stoping data recorder...')
    PIdevice.REC_STOP(recorderId)
end

function  ReadDataRecorderData(PIdevice,recorderId,dataFormat,noOfSamplesToRead,noOfTraceToRead)
    [~,dataValues,gcsArrayHeaderBuffer] = PIdevice.qREC_DAT(recorderId,dataFormat,1,noOfSamplesToRead,noOfTraceToRead); 
    disp (gcsArrayHeaderBuffer)
    disp('Recorded data points:')
    disp (dataValues)
end

function ResetAxis(PIdevice,containerUnitId)
    PIdevice.MOV(containerUnitId,0)
    while(0 ~= PIdevice.IsMoving () )
        pause ( 0.1 );
    end
end
