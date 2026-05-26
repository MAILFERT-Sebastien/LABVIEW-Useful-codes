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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   IMPORTANT NOTICE
%
%   This sample only works with a PI_MATLAB_Driver version 1.5.0 and newer.
% 
%   You MUST ALWAYS initialize the system first. The steps necessary depend on the type of mechanics (e.g., positioner, actuator) 
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   !! MOTION WARNING !!
% 
%   Description:
%
%   This sample demonstrates how to run the Fast Alignment Application
%   Sample.
%   The following types of fast alignment routines are supported:
%   - “FDL Area scan” (FDL): Fast routine for first light detection
%   - “FDR Area scan” (FDR): Spiral or raster scan to find the position of the global intensity maximum of the measured signal
%   - “Gradient search” (FDG): Circular scan with gradient formation to find the maximum intensity value of the measured signal
% 
%   You MUST EDIT the parameters of the scan definitions depending on your setup to make your system run properly.
%   Routine parameters can be edited accordingly in GetConfigs.m and/or by using the
%   corresponding application menue. Be aware that routines defined in GetConfigs.m will be synchronized with
%   the controller upon application start and will overwrite existing routines with the same
%   name.
%
%   In a multi system setup, this sample application can connect to
%   multiple devices (i.e. additional sender and receiver hexapods) with FA support. Adapt the doConnect flags 
%   of the corresponding configurations along with the routine parameters in GetConfigs.m
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Fast Alignment Application Sample


%% Add PI MATLAB Driver to path
addpath ( [getenv( 'public' ), '\PI\PI_MATLAB_Driver_GCS2'] );


%% Start connection
% (if not already connected)

if ( ~exist ( 'PI_MATLAB_Driver', 'var' ) )
    PI_MATLAB_Driver = PI_GCS_Controller();
end

connected = false;
if ( exist ( 'PIdevices', 'var' ) )
    connected = CheckConnections(PIdevices);
end

% load configs from GetConfigs.m
configs = GetConfigs();
configs = ParseParameters(configs);

if ~connected
    [PIdevices, currentDeviceID] = Connect(PI_MATLAB_Driver, configs);
    connected = CheckConnections(PIdevices);
end

configs = UpdateConfigs(PIdevices, configs);


%% Main
ShowMenuMain (PIdevices, configs, currentDeviceID, '');

% Clear Workspace
currentDeviceID = 0;
connected = CheckConnections(PIdevices);
PI_MATLAB_Driver.Destroy;
clear PIdevices;
clear PI_MATLAB_Driver;




%% Application

function connected = CheckConnections(PIdevices)
    connections = zeros(1,length(PIdevices));
    for idx = 1 : length(PIdevices)
        connections(idx) = PIdevices{idx}.IsConnected();
    end
    connected = any(connections);
end
    
function DisconnectControllers (PIdevices)
    disp('Disconnecting controllers...');
    for idx = 1 : length(PIdevices)
        PIdevices{idx}.CloseConnection();
    end
    disp('Done.')
end


function configs = ParseParameters(configs)
for idx = 1 : length(configs)
    config = configs{idx};
    for idx2 = 1 : length(config.RoutineStrings)
        configs{idx}.Routines{idx2} = ParseRoutineParameters(config.RoutineStrings{idx2});
    end
end
end


function ShowMenuMain (PIdevices, configs, currentDeviceID, errorMessage)

exit = 0;
while ~exit   
    PIdevice = PIdevices{currentDeviceID};
    config = configs{currentDeviceID};
    PrintHeaderStaticText();
    controllerName = GetControllerName(PIdevices{currentDeviceID}, config);
    fprintf('Current connection: %s\n\n', controllerName);
    errorMessage = HandleErrorMessage(errorMessage);

    try
        % Print menu
        disp('(0) Select active device');
        disp('(1) List/edit/add routines');
        disp('(2) Change routine type');
        disp('(3) Start single routine with data recorder');
        disp('(4) Start multiple routines without data recorder');
        disp('(5) Show intensity values');
        disp('(6) Move axes');
        disp('(7) Exit application');
        disp(' ');
        numberOfValidOptions = 7;

        [selectedOption, noNumberEntered ] = GetUserInputMenu (numberOfValidOptions);
        if noNumberEntered
            continue;
        end

        switch selectedOption
            case 0
                [currentDeviceID] = SelectDevice (PIdevices, currentDeviceID, configs);
            case 1
                configs{currentDeviceID} = ManageRoutines(PIdevice, config);
            case 2
                configs{currentDeviceID}.Routines = ...
                    ChangeRoutineType(PIdevice, config, config.Routines);
            case 3
                StartSingleRoutine(PIdevice, config, config.Routines);
            case 4
                StartMultipleRoutines(PIdevice, config, config.Routines);
            case 5
                ShowIntensityValues(PIdevice);
            case 6
                ShowMenuMoveAxes(PIdevice, config)
            case 7
                DisconnectControllers (PIdevices);
                exit = 1;            
        end
        
    catch err
        errorMessage = HandleError(err);
        exit = 1;
    end
end

end


function errorMessage = HandleErrorMessage(errorMessage)
if nargin < 1
    errorMessage = '';
end
if isempty(errorMessage)
    disp('Status: No error');
    disp(' ');
else
    warning(errorMessage);
    errorMessage = '';
    disp(' ');
end
end



function errorMessage = HandleError(err)
ErrorSourceFileName = err.stack(1).file;

if contains(ErrorSourceFileName, '@PI_GCS_Controller', 'IgnoreCase', true)
    errorMessage = err.message;
else
    rethrow(err);
end
end


function ShowIntensityValues(PIdevice)
answer = GCSCommandSet(PIdevice, 'TCI?');
disp('Intensity values: ');
disp(answer);
end


function StartSingleRoutine(PIdevice, config, routines)
ShowHeader(PIdevice, config);
disp('Select routine to start: ');
routine = SelectRoutine(routines);
if isempty(routine)
    return;
end

% configure Record Table Rate
recordTableRateFactor       = 1; % If set to 1, a data sample will be recorded within each servo cycle, if you set it to 3, a data sample will be recorded every third servo cycle
PIdevice.RTR( recordTableRateFactor );

% configure which data is recorded by data recorder
recordTableId   = 1;
signalSource    = routine.parameters('2_scan_axis');
recordOption    = 2; % Actual Position
PIdevice.DRC ( recordTableId, signalSource, recordOption );

recordTableId   = 2;
signalSource    = routine.parameters('4_step_axis');
recordOption    = 2; % Actual Position
PIdevice.DRC ( recordTableId, signalSource, recordOption );

recordTableId   = 3;
signalSource    = routine.parameters('A');
recordOption    = 150; % Fast alignment input channel
PIdevice.DRC ( recordTableId, signalSource, recordOption );

% configure which event triggers the data recorder (starts the data acquisition)
dummy                       = 0;
recordOptionId              = 1; % trigger recording if any command changing position is send to the controller
dummyString                 = '0';
PIdevice.DRT ( dummy, recordOptionId, dummyString );

PIdevice.FRS(routine.parameters('1_name'));
ShowMenuSinglerun(PIdevice, config, {routine});
end


function StartMultipleRoutines(PIdevice, config, routines)
ShowHeader(PIdevice, config);
disp('Select routine to start: ');
routines = SelectRoutines(routines);
if isempty(routines)
    return;
end
for routine_ = routines
    routine = routine_{1};
    PIdevice.FRS(routine.parameters('1_name'));
end
ShowMenuMultiRun(PIdevice, config, routines);
end


function ShowMenuSinglerun(PIdevice, config, runningRoutines)
ShowHeader(PIdevice, config);
ShowRoutinesRunning(PIdevice, runningRoutines);

disp('(0) Refresh status');
disp('(1) Stop running routines');
disp('(2) Show scan results');
disp('(3) Save scan data');
disp('(4) Show scan data');
disp('(5) Go back');
numberOfValidOptions = 5;
[selectedOption, goBack ] = GetUserInputMenu(numberOfValidOptions);
if goBack
    return;
end

switch selectedOption
    case 0
        % Same action as empty/invalid input
    case 1
        StopRoutines(PIdevice, runningRoutines);
    case 2
        ShowResults(PIdevice, runningRoutines);
    case 3
        SaveScanData(PIdevice, runningRoutines);
    case 4
        ShowScanData(PIdevice, runningRoutines);
    case 5
        return;
end

ShowMenuSinglerun(PIdevice, config, runningRoutines);
end


function ShowMenuMultiRun(PIdevice, config, runningRoutines)
ShowHeader(PIdevice, config);
ShowRoutinesRunning(PIdevice, runningRoutines);

disp('(0) Refresh status')
disp('(1) Stop running routines')
disp('(2) Show scan results')
disp('(3) Go back')
numberOfValidOptions = 3;
[selectedOption, goBack ] = GetUserInputMenu(numberOfValidOptions);
if goBack
    return;
end

switch selectedOption
    case 0
        % Same action as empty/invalid input
    case 1
        StopRoutines(PIdevice, runningRoutines);
    case 2
        ShowResults(PIdevice, runningRoutines);
    case 3
        return;
end
ShowMenuMultiRun(PIdevice, config, runningRoutines)
end


function StopRoutines(PIdevice, routines)
for routine_ = routines
    routine = routine_{1};
    if 0 ~= PIdevice.qFRP(routine.parameters('1_name'))
        PIdevice.FRP(routine.parameters('1_name'), 0);
    end
end
end


function data = ReadDataRecorder(PIdevice, routine)
inverted = true;
resultID = GetResultIDs(inverted);

string = PIdevice.GCSCommandSet(['FRR? ', routine.parameters('1_name'),' ', num2str(resultID('Scan time'))]);
scanTime = str2double(RemoveRedundantInformationFromqFRR(string));
servoTime = PIdevice.qSPA('1', hex2dec('E000200'));
recordRate = PIdevice.qRTR();

dataRecorderLength = round((scanTime + 0.1) / servoTime * recordRate);
fprintf('Read %d data sets from controller...\n', dataRecorderLength);
while (dataRecorderLength >PIdevice.qDRL ( 1 ))
    pause(0.1);
end
tableIDs = [1 2 3];
offset = 1;
data = PIdevice.qDRR(tableIDs, offset, dataRecorderLength);
end


function ShowScanData(PIdevice, routines)
routine = routines{1}; % Using first element, since data recorder is triggered for single run only
data = ReadDataRecorder(PIdevice, routine);
xAxisPosition = data(:, 2);
yAxisPosition = data(:, 3);
alignmentInputChannel = data(:, 4);

figure(1)
plot3(xAxisPosition, yAxisPosition, alignmentInputChannel, '.-');
title('Recorded scan data');
xlabel(['Position of axis ', routine.parameters('2_scan_axis')]);
ylabel(['Position of axis ', routine.parameters('4_step_axis')]);
zlabel(['Signal intensity ', routine.parameters('A')]);
grid on;
end


function ShowRoutinesRunning(PIdevice, routines)
runningRoutines = {};
for routine_ = routines
    routine = routine_{1};
    if 2 == PIdevice.qFRP(routine.parameters('1_name'))
        runningRoutines(end+1) = {routine.parameters('1_name')};
    end
end
runningRoutines = strjoin(runningRoutines,', ');
fprintf('Current routines running: %s\n\n', runningRoutines);
end


function [ selectedOption, goBackInMenu ] = GetUserInputMenu (numberOfValidOptions)
selectedOption = input('--> ');

% Check for invalid options
if ( any( [ 0 > selectedOption, ...
        numberOfValidOptions < selectedOption, ...
        isempty(selectedOption), ~isnumeric(selectedOption) ] ))
    goBackInMenu = true;
    return;
end

goBackInMenu = false;
end


function [ options, goBackInMenu ] = GetUserInputMenus (numberOfValidOptions)
selectedOption = input(sprintf('(separate options with space)\n-->   '), 's');
if isempty(selectedOption)
    goBackInMenu = true;
    options = {};
    return;
end
optionsCellArray=strsplit(selectedOption, ' ');
options = [];
for option = optionsCellArray
    options(end+1)=str2double(option{1});
end

% Check for invalid options
if ( any( [ 0 > options, ...
        numberOfValidOptions <= options, ...
        isempty(options) ] ))
    goBackInMenu = true;
    return;
    
end

goBackInMenu = false;
end


function routines = ChangeRoutineType(PIdevice, config, routines)
ShowHeader(PIdevice, config);
disp('Select routine to change type:');
routine = SelectRoutine(routines);
if isempty(routine)
    return;
end
routine = ChangeType(routine);
routines = UpdateRoutines(PIdevice, config, routine);
end


function routine = SelectRoutine(routines)
DispRoutines(routines);
[selectedOption, invalidSelection] = GetUserInputMenu (length(routines));
if invalidSelection
    routine = [];
    return;
end
routine = routines{selectedOption+1};
end


function routines = SelectRoutines(routines)
DispRoutines(routines);
[selectedOptions, invalidSelection] = GetUserInputMenus (length(routines));
if invalidSelection
    routines = [];
    return;
end
routines = routines(selectedOptions+1);
end


function DispRoutines(routines)
for idx = 1 : length(routines)
    routineString = RoutineToString(routines{idx});
    fprintf('(%d) %s\n', (idx-1), routineString);
end
end


function config = ManageRoutines(PIdevice, config)
ShowHeader(PIdevice, config);
disp('Select routine to edit. Renaming will add a routine:');
routine = SelectRoutine(config.Routines);
if isempty(routine)
    return
end
UserInput(routine);

config.Routines = UpdateRoutines(PIdevice, config, routine);
end


function PrintHeaderStaticText()
fprintf('\n----------------------------------------------------------------------\n');
fprintf('Fast Alignment Sample Application\n');
fprintf('Physik Instrumente (PI) SE & Co. KG\n\n');
end


function ShowHeader(PIdevice, config)
PrintHeaderStaticText();
controllerName = GetControllerName(PIdevice, config);
fprintf('Current connection: %s\n\n', controllerName);
HandleErrorMessage();
end


function currentDeviceID = SelectDevice (PIdevices, currentDeviceID, configs)
% ShowHeader(PIdevice, config);
% Shows a list of all controllers connected
ShowConnectedControllers (PIdevices, configs);

option = input('Please choose one of the available controllers list\n--> ');
if isempty(option)
    return;
end
if ( ~any(option == 0:length(PIdevices)-1) )
    error('Choosen controller is not available')
end
currentDeviceID = option + 1;
disp(PIdevices{currentDeviceID}.qIDN());
end


function ShowConnectedControllers (PIdevices, configs)
for idx = 1 : length(PIdevices)
    controllerName = GetControllerName (PIdevices{idx}, configs{idx});
    fprintf('Controller %d: %s\n', idx-1, controllerName);
end
end


function [PIdevices, currentDeviceID] = Connect(PI_MATLAB_Driver, configs)
for idx = 1 : length(configs)
    config = configs{idx};
    if config.doConnect
        if strcmpi('RS232', config.ConnectionType)
            PIdevices{idx} = PI_MATLAB_Driver.ConnectRS232(config.ComPort, config.BaudRate);
        elseif strcmpi('USB', config.ConnectionType)
            PIdevices{idx} = PI_MATLAB_Driver.ConnectUSB(config.ControllerSerialNumber);
        elseif strcmpi('TCPIP', config.ConnectionType)
            PIdevices{idx} = PI_MATLAB_Driver.ConnectTCPIP(config.IpAddress, config.PortNr);
        else
            error(['unknown communication type ', config.ConnectionType]);
        end
        fprintf('Connected: %s \n', GetControllerName(PIdevices{idx}, configs{idx}) ); % todo FAcontrollerIdentification(), config.communicationType
    end
end

% Connect to first controller available
currentDeviceID = 1;
end


function alias = GetControllerAliasWithTrailingSpace(PIdevice)
alias = '';
try
answer = GCSCommandSet(PIdevice, 'SPA? 1 0X0D000700');
catch
    return;
end
controllerType = answer(find((answer=='='),1)+1:end); % erase redundant information
controllerType = strtrim(controllerType);
answer = GCSCommandSet(PIdevice, 'SPA? 1 0X0D001000');
controllerDescription = answer(find((answer=='='),1)+1:end); % erase redundant information
controllerDescription = strtrim(controllerDescription);
alias = [controllerType, ' ', controllerDescription, ' '];
end


function controllerName = GetControllerName (PIdevice, config)
controllerIdentificationString = PIdevice.qIDN();
controllerIdnArray = strsplit(controllerIdentificationString, ',');
controllerName = strtrim(controllerIdnArray{2});
alias = GetControllerAliasWithTrailingSpace(PIdevice);
controllerName = sprintf('%s %s(%s)', controllerName, alias, config.IpAddress);
end


function ShowResults(PIdevice, runningRoutines)
qFRRstring = GCSCommandSet(PIdevice, 'FRR?');
qFRRstructure = ParseqFRRanswer(qFRRstring);

% show qFRRstructure
for key_ = runningRoutines
    key = key_{1}.parameters('1_name'); % --> Name
    fprintf('Routine: %s\n', key);
    
    resultsToShow = {'Success', 'Maximum value', 'Maximum position', ...
        'Scan time', 'Reason of abort'};
    routine = qFRRstructure(key);
    
    for key2_ = resultsToShow
        key2 = key2_{1};
        fprintf('\t%s: %s\n', key2, routine(key2));
    end
    fprintf('\n'); % create newline
end

end


function SaveScanData(PIdevice, routines)
routine = routines{1}; % Using first element, since data recorder is triggered for single run only
data = ReadDataRecorder(PIdevice, routine);
defaultFileName = 'fastalignapp.csv';
fileName = input(sprintf('Save data to (leave empty for default name "%s")\n--> ', defaultFileName), 's');
if isempty(fileName)
    fileName = defaultFileName;
end
csvwrite(fileName, data);
end


function ShowMenuMoveAxes(PIdevice, config)
ShowHeader(PIdevice, config);

disp('(0) Move axis absolute')
disp('(1) Move axis relative')
disp('(2) Go back')
numberOfValidOptions = 2;
[selectedOption, goBack ] = GetUserInputMenu(numberOfValidOptions);
if goBack
    return;
end

switch selectedOption
    case 0
        MoveAxisAbsolute(PIdevice);
        ShowPositions(PIdevice);
    case 1
        MoveAxisRelative(PIdevice, config);
    case 2
        return;
end
ShowMenuMoveAxes(PIdevice, config)
end


function ShowPositions(PIdevice)
availableAxes = PIdevice.qSAI();
disp(' ');
disp('Positions');
disp(PIdevice.qPOS(strjoin(availableAxes, ' ')));
end


function MoveAxisAbsolute(PIdevice)
availableAxes = PIdevice.qSAI();
for axis_ = availableAxes
    axis = axis_{1};
    newPosition = input(sprintf('%s (%s): ', axis, PIdevice.qPOS(axis)), 's');
    if isempty(newPosition)
        continue;
    end
    newPosition = str2double(newPosition);
    if ~isempty(newPosition)
        PIdevice.MOV(axis, newPosition);
    end
end
end


function MoveAxisRelative(PIdevice, config)
availableAxes = PIdevice.qSAI();
fprintf('Available axes: %s\n', strjoin(availableAxes ,' '));
axis = input('Axis to move: ', 's');
stepSize = input('Step size: ');

while true
    ShowHeader(PIdevice, config);
    ShowPositions(PIdevice);
    
    disp('Select direction:');
    disp('(0) Positive step');
    disp('(1) Negative step');
    disp('(2) Go back');
    numberOfValidOptions = 2;
    [selectedOption, goBack ] = GetUserInputMenu(numberOfValidOptions);
    if goBack
        return;
    end
    
    switch selectedOption
        case 0
            PIdevice.MVR(axis, stepSize);
        case 1
            PIdevice.MVR(axis, -stepSize);
        case 2
            return;
    end
end
end


%% FADevice
% This section contains all functions related to FA controller functions
function configs = UpdateConfigs(PIdevices, configs)
for idx = 1 : length(PIdevices)
    for idx2 = 1 : length(configs{idx}.Routines)
        SendRoutine(PIdevices{idx}, configs{idx}.Routines{idx2});
    end
    configs{idx}.Routines = UpdateRoutines(PIdevices{idx}, configs{idx});
end
end


function SendRoutine(PIdevice, routine)
parameterString = CreateRoutinePamString(routine);
pams = routine.parameters;

if strcmpi('FDR', routine.type)
    PIdevice.FDR( pams('1_name'), ...
        pams('2_scan_axis'), ...
        str2double(pams('3_scan_axis_range')), ...
        pams('4_step_axis'), ...
        str2double(pams('5_step_axis_range')), ...
        parameterString );
elseif strcmpi('FDL', routine.type)
    PIdevice.FDL( pams('1_name'), ...
        pams('2_scan_axis'), ...
        str2double(pams('3_scan_axis_range')), ...
        pams('4_step_axis'), ...
        str2double(pams('5_step_axis_range')), ...
        parameterString );
elseif strcmpi('FDG', routine.type)
    PIdevice.FDG( ...
        pams('1_name'), pams('2_scan_axis'), pams('4_step_axis'), parameterString);
end    
end


function routines = UpdateRoutines(PIdevice, config, routine)
if nargin > 2
    fprintf('%s send: %s\n', GetControllerName(PIdevice, config), RoutineToString(routine));
    SendRoutine(PIdevice, routine);
end
routineIDs = GetAvailableRoutineIDs(PIdevice);
if isempty(routineIDs)
    routines = {};
    return;
end
for idx = 1 : length(routineIDs)
    string = PIdevice.GCSCommandSet(['FRR? ', routineIDs{idx},' 4']);
    string = RemoveRedundantInformationFromqFRR(string);
    string = strtrim(string);
    routines{idx} = ParseRoutineParameters(string);
end
end


function string = RemoveRedundantInformationFromqFRR(string)
string = string(find((string=='='),1)+1:end); % erase redundant information
end


%% FARoutine
function routine = UserInput(routine)
definition = GetRoutineDefinition(routine.type);
for key_ = routine.parameters.keys
    key = key_{1};
    value = input(sprintf('%s (%s):', definition(key), routine.parameters(key)), 's');
    if ~isempty(value)
        routine.parameters(key) = value;
    end
end
fprintf('\n\n');
end


function routineString = RoutineToString(routine)
routineString = routine.type;
for key_ = routine.parameters.keys
    key = key_{1};
    if ismember(key, {'1_name', '2_scan_axis', '3_scan_axis_range', '4_step_axis', '5_step_axis_range'})
        routineString = [routineString, ' ', routine.parameters(key)];
    else
        routineString = [routineString, ' ', key, ' ', routine.parameters(key)];
    end
end
end


function pamString = CreateRoutinePamString(routine)
pamString = '';
for key = routine.parameters.keys
    if ~ismember(key{1}, {'1_name', '2_scan_axis', '3_scan_axis_range', '4_step_axis', '5_step_axis_range'})
        pamString = [pamString, ' ', key{1}, ' ', routine.parameters(key{1}), ' '];
    end
end
pamString = pamString(1:end-1); % remove trailing whitespace
end


function [routine] = ParseRoutineParameters(routineString)

routineDefinition = GetRoutineParameterDefinitions();

routineArray = strsplit(routineString, ' ');

routine = struct( ...
    'parameters',   containers.Map(), ...
    'type',         routineArray{1});
    
routineArray(1) = []; % first element is unused

if strcmpi('FDR', routine.type)
    routineKeys = routineDefinition.FDR.keys;

    routine.parameters('1_name') = routineArray{1};
    routine.parameters('2_scan_axis') = routineArray{2};
    routine.parameters('3_scan_axis_range') = routineArray{3};
    routine.parameters('4_step_axis') = routineArray{4};
    routine.parameters('5_step_axis_range') = routineArray{5};
    routineArray(1:5) = []; % delete processed elements
elseif strcmpi('FDL', routine.type)
    routineKeys = routineDefinition.FDL.keys;

    routine.parameters('1_name') = routineArray{1};
    routine.parameters('2_scan_axis') = routineArray{2};
    routine.parameters('3_scan_axis_range') = routineArray{3};
    routine.parameters('4_step_axis') = routineArray{4};
    routine.parameters('5_step_axis_range') = routineArray{5};
    routineArray(1:5) = []; % delete processed elements
elseif strcmpi('FDG', routine.type)
    routineKeys = routineDefinition.FDG.keys;
    
    routine.parameters('1_name') = routineArray{1};
    routine.parameters('2_scan_axis') = routineArray{2};
    routine.parameters('4_step_axis') = routineArray{3};
    routineArray(1:5) = []; % delete processed elements
else
    error (['unknown routineFA name', routineArray{1}]);
end

for idx = 1:2:length(routineArray)
    routine.parameters(routineArray{idx}) = routineArray{idx + 1};
end

% check if all keys are correct by comparing them with "routineKeys"
if(~all(ismember(routine.parameters.keys, routineKeys)))
    error('wrong');
end
end


function newRoutine = ChangeType(routine)
routineType = input('New scan type\n--> ', 's');
definition = GetRoutineDefinition(routineType);
newRoutine.type = routineType;
newRoutine.parameters = containers.Map();

disp('Transfered parameters');
for key_ = intersect(definition.keys, routine.parameters.keys)
    key = key_{1};
    newRoutine.parameters(key) = routine.parameters(key);
    disp([key, ' ', routine.parameters(key)]);
end

for key_ = setdiff(definition.keys, routine.parameters.keys)
    key = key_{1};
    value = input(sprintf('Enter value for parameter "%s"\n--> ', definition(key)), 's');
    newRoutine.parameters(key) = value;
    disp([key, ' ', newRoutine.parameters(key)]);
end

disp('');
end


function definition = GetRoutineDefinition(routineType)
routineDefinitions = GetRoutineParameterDefinitions();
if strcmpi('FDR', routineType)
    definition = routineDefinitions.FDR;
elseif strcmpi('FDL', routineType)
    definition = routineDefinitions.FDL;
elseif strcmpi('FDG', routineType)
    definition = routineDefinitions.FDG;
else
    error('"%s" is not a valid routine type.', routineType); 
end
end


function routineDefinition = GetRoutineParameterDefinitions()
FDR.keys = {'1_name', '2_scan_axis', '3_scan_axis_range', '4_step_axis', '5_step_axis_range', ...
    'L', 'A', 'F', 'V', 'MP1', 'MP2', 'TT', 'CM', 'MIIL', 'MAIL', 'ST'};
FDR.strings = {
	'routineFA name', ...
	'Scan axis', ...
	'Scan axis range', ...
	'Step axis', ...
	'Step axis range', ...
	'Threshold level', ...
	'Alignment signal input channel', ...
	'Frequency', ...
	'Velocity', ...
	'Scan axis middle position', ...
	'Step axis middle position', ...
	'Target Type', ...
	'Centroid method', ...
	'Minimum relative fast alignment input level', ...
	'Maximum relative fast alignment input level', ...
	'Stop position option' };
routineDefinition.FDR = containers.Map(FDR.keys, FDR.strings);
FDL.keys = {'1_name', '2_scan_axis', '3_scan_axis_range', '4_step_axis', '5_step_axis_range', ...
    'S', 'C', 'A', 'MT', 'MP1', 'MP2', 'CP1', 'CP2'};
FDL.strings = {
	'routineFA name', ...
	'Scan axis', ...
	'Scan axis range', ...
	'Step axis', ...
	'Step axis range', ...
	'Threshold sigma', ...
	'Threshold calculation', ...
    'Alignment signal input channel', ...
	'Maximum search time', ...
	'Scan axis middle position', ...
	'Step axis middle position', ...
	'Scan axis calculation position', ...
	'Step axis calculation position' };
routineDefinition.FDL = containers.Map(FDL.keys, FDL.strings);
FDG.keys = {'1_name', '2_scan_axis', '4_step_axis', ...
    'ML', 'A', 'MIA', 'MAA', 'F', 'SP', 'V', 'MDC', 'SPO'};
FDG.strings = { ...
	'routineFA name', ...
	'Scan axis', ...
	'Step axis', ...
	'Stop level', ...
	'Alignment signal input channel', ...
	'Minimum radius', ...
	'Maximum radius', ...
	'Frequency', ...
	'Speed factor', ...
	'Velocity', ...
	'Max direction changes', ...
	'Speed offset' };
routineDefinition.FDG = containers.Map(FDG.keys, FDG.strings);
end


function resultIDs = GetResultIDs(inverted)
if nargin < 1
    inverted = false;
end
keys = {1, 2, 3, 4, 5, 6, 7, 8, 9};
values = {'Success', 'Maximum value', 'Maximum position', 'Routine definition', ...
    'Scan time', 'Reason of abort', 'Radius', 'Number of direction changes', 'Gradient length'};
if inverted
resultIDs = containers.Map(values, keys);    
else
resultIDs = containers.Map(keys, values);    
end
end