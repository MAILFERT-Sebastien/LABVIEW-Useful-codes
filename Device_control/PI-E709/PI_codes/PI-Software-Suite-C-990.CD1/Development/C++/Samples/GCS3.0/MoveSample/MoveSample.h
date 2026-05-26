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
    It shows how to determine min/max position limits, and how to do absolute and relative moves.
    The positioner has to be referenced before starting this sample.

**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "PI_GCS2_DLL.h"


const int MODE_OF_OPERATION_OPEN_LOOP_POSITION = 1;
const int MODE_OF_OPERATION_CLOSED_LOOP_POSITION = 2;


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

double GetMinPositionLimit (int ID, std::string axis);
double GetMaxPositionLimit (int ID, std::string axis);
void RelativeMove (int ID, std::string axis, double offset);
bool WaitForMotionDone (int ID, std::string axis);
void SetModeOfOperation (int ID, std::string axis, int mode);
void ReportError (int iD);
