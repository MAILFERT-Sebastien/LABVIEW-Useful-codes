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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
%   The E-712 systems provide routines for fast alignment of one or more senders and receivers. 
%   The objective of the routines is to align each sender and receiver 
%   so that the maximum intensity of the emitted signal is measured on the receiver side.
%
%   The following types of fast alignment routines are provided:
%   - “FDL Area scan”: Fast routine for first light detection
%   - “FDR Area scan”: Spiral or raster scan to find the position of the global intensity maximum of the measured signal
%   - “Gradient search”: Circular scan with gradient formation to find the maximum intensity value of the measured signal
%   Typically, the end position of an area scan routine is used as the start position for a gradient search routine.
%   The maximum number of routines is identical to the number of motion axes that are present in the system (E-712 controller only).
%   Multiple gradient search routines can run synchronously for the axes on both the sender and receiver side (E-712 controller only).
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Setup configuration

% You MUST EDIT AND ACTIVATE the parameters of the scan definitions 
% to make your system run properly.
% 1. Activate the scan type(s) you want to define
% 2. Activate the connection type
% 3. Set the connection settings

%Scan type (multiple selections possible)
use_FDGScan                         = false;
use_FDLScan                         = false;
use_FDRScan                         = false;

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
ip = 'xxx.xxx.xxx.xxx'; % Use "devicesTcpIp = Controller.EnumerateTCPIPDevices('')" to get all PI controller available on the network
port = 50000;          % Is 50000 for almost all PI controllers

%==========================================================================

disp('                 ')
disp('|--------------------|')
disp('| FA routines sample |')
disp('|--------------------|')

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


try   
    %% Define fast alignment scan routines
    FAScanDefinitions = struct();

    if use_FDRScan
        disp('defining FDR area scan routine...');
        %% Define FDR Area Scan
        % Warning: given values of this sample can damage your system
        % adjust all values accordingly and remove the following line
        error('adjust FDR values to match your configuration');

        FDRScanParameters.scanGCSCommand = 'FDR';
        FDRScanParameters.routineName = '6';
        FDRScanParameters.scanAxis = '1';
        FDRScanParameters.scanAxisRange = 100.0;
        FDRScanParameters.stepAxis = '2';
        FDRScanParameters.stepAxisRange = 100.0;
        FDRScanParameters.thresholdLevel = 0; % L
        FDRScanParameters.alignmentSignalInputChannel = 1; %A
        FDRScanParameters.frequency = 20.0; % F
        FDRScanParameters.velocity = 50.0; % V
        FDRScanParameters.scanAxisMiddlePosition = 50.0; % MP1
        FDRScanParameters.stepAxisMiddlePosition = 50.0; % MP2
        FDRScanParameters.targetType = 0; % TT
        FDRScanParameters.centroidMethod = 0; % CM
        FDRScanParameters.minimumRelativeFastAlignmentInputLevel = 1.0; % MIIL
        FDRScanParameters.maximumRelativeFastAlignmentInputLevel = 99.0; % ;MAIL
        FDRScanParameters.stopPositionOption = 0; % ST

        FAScanDefinitions.FDRAreaScan.ScanCommand = makeFDRScanCmd(FDRScanParameters);
        FAScanDefinitions.FDRAreaScan.ScanParameters = FDRScanParameters;
        FDRScanParameters = [];
    end

    if use_FDLScan
        disp('defining FDL area scan routine...');
        %% Define FDL Area Scan
        % Warning: given values of this sample can damage your system
        % adjust all values accordingly and remove the following line
        error('adjust FDL values to match your configuration');

        FDLScanParameters.scanGCSCommand = 'FDL';
        FDLScanParameters.routineName = '5';
        FDLScanParameters.scanAxis = '1';
        FDLScanParameters.scanAxisRange = 1.000000000e+02;
        FDLScanParameters.stepAxis = '2';
        FDLScanParameters.stepAxisRange = 1.000000000e+02;
        FDLScanParameters.thresholdSigma = 100; % S
        FDLScanParameters.thresholdCalculation = 0; % C
        FDLScanParameters.alignmentSignalInputChannel = 1; %A
        FDLScanParameters.maximumSearchTime = 1.000000000e+01; % MT
        FDLScanParameters.scanAxisMiddlePosition = 5.000000000e+01; % MP1
        FDLScanParameters.stepAxisMiddlePosition = 5.000000000e+01; % MP2
        FDLScanParameters.scanAxisCalculationPosition = 1.000000000e+01; % CP1
        FDLScanParameters.stepAxisCalculationPosition = 1.000000000e+01; % CP2

        FAScanDefinitions.FDLAreaScan.ScanCommand = makeFDLScanCmd(FDLScanParameters);
        FAScanDefinitions.FDLAreaScan.ScanParameters = FDLScanParameters;
        FDLScanParameters = [];
    end

    if use_FDGScan
        disp('defining FDG gradient scan routine...');
        %% Define Gradient Search
        % Warning: given values of this sample can damage your system
        % adjust all values accordingly and remove the following line
        error('adjust FDG values to match your configuration');

        FDGScanParameters.scanGCSCommand = 'FDG';
        FDGScanParameters.routineName = '3';
        FDGScanParameters.scanAxis = '1';
        FDGScanParameters.stepAxis = '2';
        FDGScanParameters.stopLevel = 0.01; %ML
        FDGScanParameters.alignmentSignalInputChannel = 1; %A
        FDGScanParameters.minRadius = 1.0; %MIA
        FDGScanParameters.maxRadius = 4.0; %MAA
        FDGScanParameters.frequency = 23.0; %F
        FDGScanParameters.speedFactor = 20.0; %SP
        FDGScanParameters.maxVelocity = 20.0; %V
        FDGScanParameters.maxDirectionChanges = 6; %MDC
        FDGScanParameters.speedOffset = 0.05; %SPO

        FAScanDefinitions.FDGGradientScan.ScanCommand = makeFDGScanCmd(FDGScanParameters);
        FAScanDefinitions.FDGGradientScan.ScanParameters = FDGScanParameters;
        FDGScanParameters = [];
    end

    %% Start connection
    % Connect (if not already connected)
    try 
        disp('Connecting to device..');
        boolPIdeviceConnected = false; 
        if ( exist ( 'PIdevice', 'var' ) ), if ( PIdevice.IsConnected ), boolPIdeviceConnected = true; end; end

        if ( ~(boolPIdeviceConnected ) )
            connectedControllerName = '';
            if ~any([use_RS232_Connection,use_TCPIP_Connection,use_USB_Connection])
                error('Activate the desired connection.')
            end
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
                if any(contains(ip,'x'))
                    error('Specify IP address.')
                end
                PIdevice = Controller.ConnectTCPIP ( ip, port ) ;
            end
        end
        % query controller identification string
        connectedControllerName = PIdevice.qIDN();
        disp(['Connection to ' connectedControllerName 'established.']);
    catch ex
        disp('Connection failed!');
        rethrow(ex);
    end
    % initialize PIdevice object for use in MATLAB
    PIdevice = PIdevice.InitializeController ();

    runMainMenue(PIdevice,FAScanDefinitions);

    %% Cose the connection
    disp(['Close connection to ', connectedControllerName]);
    PIdevice.CloseConnection ();

    %% Unload the dll and destroy the class object
    Controller.Destroy ();
    clear Controller;
    clear PIdevice;
catch ex
    if ~strcmp(connectedControllerName,'')
        disp(['Close connection to ', connectedControllerName]);
    else
        disp('Not connected to a controller.');
    end
    Controller.Destroy ();
    clear Controller;
    clear PIdevice;
    rethrow(ex);
end


%% function definitions

%% Main menue
function runMainMenue(PIdevice,FAScanDefinitions)
    % get available scan routines on controller
    AvailableScans = getScansOnController(PIdevice);
    
    selectedMenueEntry = -1;
    disp('                 ')
    disp('Scans on controller:')
    for i = 1 :length(AvailableScans)
        fprintf('%d - %s "%s": %s\n',i,AvailableScans{i,1},AvailableScans{i,2},AvailableScans{i,3});
    end
    disp('-----------------')
        
    while selectedMenueEntry ~= 0
        selectedMenueEntry = -1;
        disp('                 ')
        disp('MAIN MENUE')
        disp('-----------------')
        disp('1 - List routines')
        disp('2 - Send routines')
        disp('3 - Start routines')
        disp('                 ')
        disp('0 - Exit')
        
        while(~isnumeric(selectedMenueEntry) || selectedMenueEntry > 3 || selectedMenueEntry < 0)
            selectedMenueEntry = input('Select menue entry: ');
        end
        switch selectedMenueEntry
            case 1
                runListRoutinesMenue(PIdevice,FAScanDefinitions);
            case 2
                AvailableScans = runSendRoutineMenue(PIdevice,FAScanDefinitions);
            case 3
                runStartRoutineMenue(PIdevice,AvailableScans);
        end
    end
end

function AvailableScans = runListRoutinesMenue(PIdevice,FAScanDefinitions)    
    %% List scan routines from definitions and controller
    definedScans = fieldnames(FAScanDefinitions);
    AvailableScans = getScansOnController(PIdevice);
    disp('                 ')
    disp('LIST SCAN ROUTINES')
    disp('-----------------')
    disp('                 ')
    disp('Configured scan routines:')
    if isempty(definedScans)
        disp('No Scan type definitions activated.')
    end
    for i = 1 :length(definedScans)
        fprintf('%d - %s "%s": %s\n',i,definedScans{i},FAScanDefinitions.(definedScans{i}).ScanParameters.routineName,FAScanDefinitions.(definedScans{i}).ScanCommand.commandString);
    end
    disp('                 ')
    disp('Scan routines on controller:')
    for i = 1 :length(AvailableScans)
        fprintf('%d - %s "%s": %s\n',i,AvailableScans{i,1},AvailableScans{i,2},AvailableScans{i,3});
    end
    disp('                 ')
    disp('-----------------')
end

function AvailableScans = runSendRoutineMenue(PIdevice,FAScanDefinitions)    
    %% Send defined scans to controller
    AvailableScans = [];
    selectedScan = -1;
    definedScans = fieldnames(FAScanDefinitions);
    while selectedScan ~= 0
        disp('                 ')
        disp('SEND SCAN ROUTINES')
        disp('------------------')
        disp('                 ')
        disp('Configured scan routines:')
        if isempty(definedScans)
            disp('No Scan type definitions activated.')
        end
        for i = 1 :length(definedScans)
            fprintf('%d - %s "%s": %s\n',i,definedScans{i},FAScanDefinitions.(definedScans{i}).ScanParameters.routineName,FAScanDefinitions.(definedScans{i}).ScanCommand.commandString);
        end
        disp('                 ')
        disp('0 - Main menue')

        while(~isnumeric(selectedScan) || selectedScan > size(definedScans,1) || selectedScan < 0)
            selectedScan = input('Select scan routine to be send to controller: ');
        end

        if selectedScan ~= 0
            % send definition to controller
            try
                routineName = FAScanDefinitions.(definedScans{selectedScan}).ScanParameters.routineName;
                disp('                 ')
                disp(['send ' definedScans{selectedScan} ' "' routineName '" to controller...']);
                PIdevice.(FAScanDefinitions.(definedScans{selectedScan}).ScanParameters.scanGCSCommand)( ...
                      FAScanDefinitions.(definedScans{selectedScan}).ScanParameters.routineName ...
                    , FAScanDefinitions.(definedScans{selectedScan}).ScanCommand.axisParameters{:} ...
                    , FAScanDefinitions.(definedScans{selectedScan}).ScanCommand.optionalParametersString);
                disp(['sending ', definedScans{selectedScan},' "',routineName,'" was successful.']);
                pause(0.5);     
            catch ex
                fprintf(2,'Error sending %s "%s":\n %s',definedScans{selectedScan},routineName,ex.message);
            end
            selectedScan = -1;
        end
        % fetch scans from controller
        AvailableScans = getScansOnController(PIdevice);
    end
end

function runStartRoutineMenue(PIdevice,AvailableScans)
    %% Run scan routines
    selectedScan = -1;
    while selectedScan ~= 0
        disp('                 ')
        disp('-----------------')
        disp('Available scans:')
        for i = 1 :length(AvailableScans)
            fprintf('%d - %s "%s": %s\n',i,AvailableScans{i,1},AvailableScans{i,2},AvailableScans{i,3});
        end
        disp('                 ')
        disp('0 - Main menue')

        while(~isnumeric(selectedScan) || selectedScan > size(AvailableScans,1) || selectedScan < 0)
            selectedScan = input('Select scan to run: ');
        end

        if selectedScan ~= 0
            success = runScan(PIdevice,AvailableScans(selectedScan,:));
            if success
                disp([AvailableScans{selectedScan,1}, ' "', AvailableScans{selectedScan,2}, '" finished.']);
            else
                disp([AvailableScans{selectedScan,1}, ' "', AvailableScans{selectedScan,2}, '" failed to run.']);
            end
            selectedScan = -1;
        end  
    end
end

function FDRAreaScanCommand = makeFDRScanCmd(FDRScanParams)
    try
        FDRAreaScanCommand.scanGCSCommand = FDRScanParams.scanGCSCommand;
        FDRAreaScanCommand.axisParameters = { ...
              FDRScanParams.scanAxis ...
            , FDRScanParams.scanAxisRange ... 
            , FDRScanParams.stepAxis ...
            , FDRScanParams.stepAxisRange};
        
        % PIdevice.FDR needs some parameters as plain text string. The following
        % lines build this string from the parameters in struct 'FDRScanParams'.
        FDRAreaScanCommand.optionalParametersString = ...
            [  'L ', num2str(FDRScanParams.thresholdLevel) ...
            , ' A ', num2str(FDRScanParams.alignmentSignalInputChannel) ...
            , ' F ', num2str(FDRScanParams.frequency) ...
            , ' V ', num2str(FDRScanParams.velocity) ...
            , ' MP1 ', num2str(FDRScanParams.scanAxisMiddlePosition) ...
            , ' MP2 ', num2str(FDRScanParams.stepAxisMiddlePosition) ...
            , ' TT ', num2str(FDRScanParams.targetType) ...
            , ' CM ', num2str(FDRScanParams.centroidMethod) ...
            , ' MIIL ', num2str(FDRScanParams.minimumRelativeFastAlignmentInputLevel) ...
            , ' MAIL ', num2str(FDRScanParams.maximumRelativeFastAlignmentInputLevel) ...
            , ' ST ', num2str(FDRScanParams.stopPositionOption)];
        
        axisParametersCell = cellfun(@num2str,FDRAreaScanCommand.axisParameters,'UniformOutput',false);
        axisParametersString = sprintf('%s ',axisParametersCell{:});
        FDRAreaScanCommand.commandString = [FDRAreaScanCommand.scanGCSCommand,' ',FDRScanParams.routineName,' ',axisParametersString,FDRAreaScanCommand.optionalParametersString];
    catch
        error('Invalid FDR parameters!')
    end
end

function FDLAreaScanCommand = makeFDLScanCmd(FDLScanParams)
    try
        FDLAreaScanCommand.scanGCSCommand = FDLScanParams.scanGCSCommand;
        FDLAreaScanCommand.axisParameters = { ...
              FDLScanParams.scanAxis ...
            , FDLScanParams.scanAxisRange ... 
            , FDLScanParams.stepAxis ...
            , FDLScanParams.stepAxisRange};
        
        % PIdevice.FDL needs some parameters as plain text string. The following
        % lines build this string from the parameters in struct 'FDLScanParams'.
        FDLAreaScanCommand.optionalParametersString = ...
            [  'S ', num2str(FDLScanParams.thresholdSigma) ...
            , ' C ', num2str(FDLScanParams.thresholdCalculation) ...
            , ' A ', num2str(FDLScanParams.alignmentSignalInputChannel) ...
            , ' MT ', num2str(FDLScanParams.maximumSearchTime) ...
            , ' MP1 ', num2str(FDLScanParams.scanAxisMiddlePosition) ...
            , ' MP2 ', num2str(FDLScanParams.stepAxisMiddlePosition) ...
            , ' CP1 ', num2str(FDLScanParams.scanAxisCalculationPosition) ...
            , ' CP2 ', num2str(FDLScanParams.stepAxisCalculationPosition)];
        
        axisParametersCell = cellfun(@num2str,FDLAreaScanCommand.axisParameters,'UniformOutput',false);
        axisParametersString = sprintf('%s ',axisParametersCell{:});
        FDLAreaScanCommand.commandString = [FDLAreaScanCommand.scanGCSCommand,' ',FDLScanParams.routineName,' ',axisParametersString,FDLAreaScanCommand.optionalParametersString];    
    catch
        error('Invalid FDL parameters!')
    end
end

function FDGGradientScanCommand = makeFDGScanCmd(FDGScanParams)
    try
        FDGGradientScanCommand.scanGCSCommand = FDGScanParams.scanGCSCommand;
        FDGGradientScanCommand.axisParameters = { ...
              FDGScanParams.scanAxis ... 
            , FDGScanParams.stepAxis};
        % PIdevice.FDG needs some parameters as plain text string. The following
        % lines build this string from the parameters in struct 'FDGScanParams'.
        % parameters 
        FDGGradientScanCommand.optionalParametersString = ...
            [  'ML ', num2str(FDGScanParams.stopLevel) ...
            , ' A ', num2str(FDGScanParams.alignmentSignalInputChannel) ...
            , ' MIA ', num2str(FDGScanParams.minRadius) ...
            , ' MAA ', num2str(FDGScanParams.maxRadius) ...
            , ' F ', num2str(FDGScanParams.frequency) ...
            , ' SP ', num2str(FDGScanParams.speedFactor) ...
            , ' V ', num2str(FDGScanParams.maxVelocity) ...
            , ' MDC ', num2str(FDGScanParams.maxDirectionChanges) ...
            , ' SPO ', num2str(FDGScanParams.speedOffset)];
        
        axisParametersCell = cellfun(@num2str,FDGGradientScanCommand.axisParameters,'UniformOutput',false);
        axisParametersString = sprintf('%s ',axisParametersCell{:});
        FDGGradientScanCommand.commandString = [FDGGradientScanCommand.scanGCSCommand,' ',FDGScanParams.routineName,' ',axisParametersString,FDGGradientScanCommand.optionalParametersString];
    catch
        error('Invalid FDG parameters!')
    end
end

function scansOnController = getScansOnController(PIdevice)
   scansOnControllerString = PIdevice.qFRR([],-1);
   scansOnControllerCell = splitlines(scansOnControllerString);
   routineNames = cell(size(scansOnControllerCell,1),1);
   for i=1:size(scansOnControllerCell,1)
       routineNames{i} = scansOnControllerCell{i}(1);  
   end
   uniqueRoutineNames = unique(routineNames);
   [scanTypes, scanRoutineCmds] = deal(cell(size(uniqueRoutineNames)));
   for i=1:size(uniqueRoutineNames,1)
       scanRoutineCmd = PIdevice.qFRR(uniqueRoutineNames{i},4);
       switch scanRoutineCmd(5:7)
           case 'FDR'
               scanType = 'FDRAreaScan';
           case 'FDG'
               scanType = 'FDGGradientScan';
           case 'FDL'
               scanType = 'FDLAreaScan';
           otherwise
               scanType = '';
       end
       scanTypes{i} = scanType;
       scanRoutineCmds{i} = scanRoutineCmd(5:end);
   end
   scansOnController = [scanTypes,uniqueRoutineNames,scanRoutineCmds];    
end

function success = runScan(PIdevice,scan)
    try
        %% Run Scan
        disp('                         ')
        disp('-------------------------');
        disp(['start scan routine: ', scan{1}, ' "',scan{2}, '"']);
        PIdevice.FRS(scan{2});
        tStart = tic;
        reverseStr = '';
        while ( PIdevice.qFRP(scan{2}) )
            elapsedTime = toc(tStart);
            msg = sprintf('waiting for scan routine to finish... %.3f s',elapsedTime);
            disp([reverseStr, msg]);
            reverseStr = repmat(sprintf('\b'), 1, length(msg) + 1);
            pause(0.1);
        end
        disp(strcat("finished with error code: ", string(PIdevice.qERR)));
        disp('                         ')
        disp('-------------------------');
        disp(['results for scan routine: ', scan{2}]);
        for resultId = 1 : 3
        %     1 : successful bit
        %     2 : signal level at maximum
        %     3 : axis positions of maximum signal
           result = PIdevice.qFRR(scan{2}, resultId);
           
           disp(['result ID: ', num2str(resultId)]);
           if strcmp(result,[scan{2} ' 1=0'])
               disp([result '  (unsuccessful)']);
           elseif strcmp(result,[scan{2} ' 1=1'])
               disp([result '  (successful)']);
           else
               disp(result);
           end
        end
        success = 1;
    catch
        success = 0;
    end
    
    disp('-------------------------');
    disp('done')
end