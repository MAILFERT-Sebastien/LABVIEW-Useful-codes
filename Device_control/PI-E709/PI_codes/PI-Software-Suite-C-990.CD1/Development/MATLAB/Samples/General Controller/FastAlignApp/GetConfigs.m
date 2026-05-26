% function GetConfigs()
% This code is provided by Physik Instrumente (PI) SE & Co. KG.
% You may alter it corresponding to your needs.
% Comments and Corrections are very welcome.
% Please contact us by mailing to support-software@pi.ws. Thank you.

function [configs] = GetConfigs()

% E-712
config.ConnectionType = 'tcpip';
config.IpAddress = '172.16.50.64';
config.PortNr = 50000;
config.ComPort = 1;
config.BaudRate = 115200;
config.ControllerSerialNumber = '1234567890';
config.RoutineStrings = {
    'FDR 2 1 1.000000000e+002 2 1.000000000e+002 L 0.10000000e+000 A 1 F 1.000000000e+001 V 4.000000000e+001 MP1 5.000000000e+001 MP2 5.000000000e+001 TT 0 CM 0 MIIL 1.000000000e+000 MAIL 99.000000000e+000 ST 0', ...
    'FDR 3 1 1.000000000e+002 2 1.000000000e+002 L 0.10000000e+000 A 1 F 1.000000000e+001 V 4.000000000e+001 MP1 5.000000000e+001 MP2 5.000000000e+001 TT 0 CM 0 MIIL 1.000000000e+000 MAIL 99.000000000e+000 ST 0', ...
    'FDL 1 1 1.000000000e+002 2 1.000000000e+002 A 1 C 1 S 100 MT 5.000000000e+00 MP1 5.000000000e+01 MP2 5.000000000e+01 CP1 0.000000000e+00 CP2 0.000000000e+00'
    };
config.Routines = {};
config.doConnect = 1;
configs{1} = config;

% Hexapod 1
config.ConnectionType = 'tcpip';
config.IpAddress = '172.16.50.11';
config.PortNr = 50000;
config.ComPort = 1;
config.BaudRate = 115200;
config.ControllerSerialNumber = '1234567890';
config.RoutineStrings = {
    'FDR 2 X 1.000000 Y 0.020000 L -1.000000 A 5 F 15.000000 V 5.000000 MP1 4.669026 MP2 4.025528 TT 2 CM 0 MIIL 10.000000 MAIL 80.000000 ST 0', ...
    'FDR FINE X 0.100000 Y 0.001000 L -3.000000 A 5 F 15.000000 V 5.000000 MP1 4.655700 MP2 4.013400 TT 2 CM 0 MIIL 10.000000 MAIL 80.000000 ST 0'
    };
config.Routines = {};
config.doConnect = 0;
configs{2} = config;

% Hexapod 2
config.ConnectionType = 'tcpip';
config.IpAddress = '172.16.50.27';
config.PortNr = 50000;
config.ComPort = 1;
config.BaudRate = 115200;
config.ControllerSerialNumber = '1234567890';
config.RoutineStrings = {
    'FDR 2 X 1.000000 Y 0.020000 L -1.000000 A 5 F 15.000000 V 5.000000 MP1 4.669026 MP2 4.025528 TT 2 CM 0 MIIL 10.000000 MAIL 80.000000 ST 0', ...
    'FDR FINE X 0.100000 Y 0.001000 L -3.000000 A 5 F 15.000000 V 5.000000 MP1 4.655700 MP2 4.013400 TT 2 CM 0 MIIL 10.000000 MAIL 80.000000 ST 0'
    };
config.Routines = {};
config.doConnect = 0;
configs{3} = config;
end