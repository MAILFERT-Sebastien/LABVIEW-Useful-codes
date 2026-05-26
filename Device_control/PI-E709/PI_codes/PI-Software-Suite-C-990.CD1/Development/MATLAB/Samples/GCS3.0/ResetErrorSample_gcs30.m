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
%   This sample connects to a GCS 3.0 controller using TCP/IP, USB or RS232.
%   It shows how to determine min/max position limits, how to do absolute moves and how to recover from an axis error.
%   The positioner has to be referenced before starting this sample.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ResetErrorSample_gcs30
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
ip = 'XXX.XXX.XXX.XX';  % Use "devicesTcpIp = Controller.EnumerateTCPIPDevices('')" to get all PI controller available on the network
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
    
    
    %%
    axisContainerUnit = 'AXIS_1';
    open_loop = 1;
    closed_loop = 2;
    %%
    %Startup axis and move to target position
    SetModeOfOperation (PIdevice,axisContainerUnit,closed_loop)
    minimumPosition = GetMinPositionLimit(PIdevice,axisContainerUnit);
    maximumPosition = GetMaxPositionLimit(PIdevice,axisContainerUnit);
    travelRange = ( maximumPosition - minimumPosition );
    targetPosition = minimumPosition + (travelRange * 0.5);
    
    paramPIDAxis1.PTerm = {axisContainerUnit,'CNTR_1','0x120,1'};
    paramPIDAxis1.ITerm = {axisContainerUnit,'CNTR_1','0x121,1'};
    paramPIDAxis1.DTerm = {axisContainerUnit,'CNTR_1','0x122,1'};
    
    lastPIDValues = PrepareForMotionError(PIdevice, paramPIDAxis1);
    
    %do absolute motion to middle of travel range
    MoveToAbsoluteTarget( PIdevice,axisContainerUnit, targetPosition ); %Motion error will not be triggered if movement is too small.
    %let axis "accelerate". Because PID = 0 the axis will stay at current position until motion error is generated.
    pause(2);
    
    %Note that by default, all axes are grouped via the “Group” configuration parameter (address: Axis_x - 0x50).
    %If an axis from a group goes into the “fault” state, all other axes of the group also go into the “fault” state
    %and require also the error handling.
    RecoverFromError ( PIdevice,axisContainerUnit, paramPIDAxis1, lastPIDValues)
    
    SetModeOfOperation (PIdevice, axisContainerUnit,closed_loop)
    MoveToAbsoluteTarget(PIdevice,  axisContainerUnit, targetPosition )
    
    WaitForMotionDone(PIdevice,axisContainerUnit)
    ResetAxis(PIdevice,axisContainerUnit)
    SetModeOfOperation (PIdevice,axisContainerUnit,open_loop)
    
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

function RecoverFromError (PIdevice, axisContainerUnit, paramPIDAxis1, lastPIDValues)
disp('Recovering from the error...')

if(AxisHasError(PIdevice, axisContainerUnit) ~= 1)
    error('axis error not set as expected');
end
disp('Axis is now in error state.');

ResetAxesError(PIdevice);

%Restore PID parameters to allow movement
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.PTerm,lastPIDValues.PTerm)
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.ITerm,lastPIDValues.ITerm)
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.DTerm,lastPIDValues.DTerm)
%Enable axis
switchOn = 1;
PIdevice.EAX(axisContainerUnit, switchOn)
end

function state = AxisHasError(PIdevice, axisContainerUnit)
errorBit = 0;
bitMask = '00000001';
state = CheckAxisStatusValue (PIdevice, axisContainerUnit, errorBit, bitMask);
end

function ResetAxesError(PIdevice)
axes = PIdevice.qSAI();
for axis = axes
    if (AxisHasError (PIdevice, char(axis)) == 1)
        dsmFaultReaction = 3;
        dsmStartBit = 0;
        dsmBitMask = '000000FF';
        while (CheckAxisStatusValue(PIdevice, char(axis), dsmStartBit, dsmBitMask) == dsmFaultReaction) %wait while fault reaction is active
        end
        PIdevice.RES(char(axis))
    end
end
end

function state = CheckAxisStatusValue (PIdevice, axisContainerUnit, startBit, bitMask)
statusBits = bitshift( PIdevice.qSTV(axisContainerUnit), -startBit);
state = bitand(statusBits, hex2dec(bitMask));
end

function SetModeOfOperation (PIdevice,axis, mode)
PIdevice.SAM(axis,mode)
fprintf( 'Set mode of operation %d\n',mode);
end

function MoveToAbsoluteTarget(PIdevice,axisContainerUnit, targetPosition)
disp ('Stage is moving...')
PIdevice.MOV ( axisContainerUnit, targetPosition );
end


function ResetAxis(PIdevice,containerUnitId)
PIdevice.MOV(containerUnitId,0)
while(0 ~= PIdevice.IsMoving () )
    pause ( 0.1 );
end
end

function minimumPosition = GetMinPositionLimit(PIdevice,axis)
minimumPosition = PIdevice.qSPV_Double('RAM',axis,'-','0x121');
end

function maximumPosition = GetMaxPositionLimit(PIdevice,axis)
maximumPosition = PIdevice.qSPV_Double('RAM',axis,'-','0x122');
end

function  WaitForMotionDone(PIdevice,containerUnitId)
while(0 ~= PIdevice.IsMoving () )
    pause ( 0.1 );
    fprintf('.');
end
fprintf('Successfully moved to %f\n',PIdevice.qPOS(containerUnitId));
end

function value = ReadParameterValue(PIdevice,memoryType,Parameter)
value = PIdevice.qSPV_Double(memoryType,Parameter(1,1),Parameter(1,2),Parameter(1,3));
end

function WriteParameterValue (PIdevice,memoryType,Parameter,value)
PIdevice.SPV_Double(memoryType,Parameter(1,1),Parameter(1,2),Parameter(1,3),value)
end


function [lastPIDValues] = PrepareForMotionError(PIdevice, paramPIDAxis1)
%Higher user level is required for writing parameters. See password in your controller manual.
PIdevice.UCL('adv', 'advanced');
disp('Changed user level to ADV');

%Save last PID values
lastPIDValues.PTerm = ReadParameterValue(PIdevice,'RAM',paramPIDAxis1.PTerm);
lastPIDValues.ITerm = ReadParameterValue(PIdevice,'RAM',paramPIDAxis1.ITerm);
lastPIDValues.DTerm = ReadParameterValue(PIdevice,'RAM',paramPIDAxis1.DTerm);

%Set all PID values to zero
value = 0;
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.PTerm,value)
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.ITerm,value)
WriteParameterValue (PIdevice,'RAM',paramPIDAxis1.DTerm,value)

end
