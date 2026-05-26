/**
    (c)2015-2021 Physik Instrumente (PI) SE & Co. KG
    Software products that are provided by PI are subject to the General Software License Agreement of
    Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components.
    For more information, please read the General Software License Agreement and the Third Party Software Note linked below.

    General Software License Agreement :
    http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
    Third Party Software Note :
    http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf
**/

/**
    !! MOTION WARNING !!

    This sample connects to a controller using a connection dialog.
    The value of the parameter for the maximum velocity is read and
    the value is modified and set as velocity of the axis.

**/

#include <windows.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "PI_GCS2_DLL.h"


enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


struct Parameter
{
    std::string containerUnitId;
    std::string functionUnitId;
    std::string ParameterId;
};


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

double ReadParameterValue (int iD, const std::string& memoryType, const Parameter& parameter);
void WriteParameterValue (int iD, const std::string& memoryType, const Parameter& parameter, double value);

void ReportError (int iD);
