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
    Is also shows how to recover from an axis error.
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
const int PI_ERROR_AXIS_RUNTIME_ERROR = -1117;
const int PI_ERROR_MOT_CMD_FAULT_REACTION_ACTIVE = 49174;

const unsigned int START_BIT_DSM = 0;    // Start bit drive state machine
const unsigned int START_BIT_MOD = 8;    // Start bit mode of operation
const unsigned int DSM_BIT_MASK = 0xFF;
const unsigned int DSM_ERROR_BIT = 1;
const unsigned int DSM_FAULT_REACTION_BITS = 3;

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

struct PID
{
    double P = 0.0;
    double I = 0.0;
    double D = 0.0;
};

int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int id);
void CloseConnectionWithComment (int id, std::string comment);
double ReadParameterValue (int id, const std::string& memoryType, const Parameter& parameter);
void WriteParameterValue (int id, const std::string& memoryType, const Parameter& parameter, double value);

double GetPosition (int id, std::string axis);
void SetPosition (int id, std::string axis, double position);
double GetMinPositionLimit (int id, std::string axis);
double GetMaxPositionLimit (int id, std::string axis);
bool WaitForMotionDone (int id, std::string axis);
void SetModeOfOperation (int id, std::string axis, int mode);
void ReportError (int id);
PID PrepareMotionError (int id, std::string axis, std::string pid);
void RecoverFromError (int id, std::string axis, std::string pidAxis, PID oldPID);
unsigned int CheckAxisStatusValue(int id, std::string axis, unsigned int startBit, unsigned int bitMask);
bool AxisHasError (int id, std::string axis);
void ResetAxesError (int id);
std::vector<std::string> ReadAllAxesNames (int id);
