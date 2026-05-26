/**
    (c)2015 Physik Instrumente (PI) SE & Co. KG
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
    Then a trajectory is set up, transferred to the controller and started.
    The positioner has to be intialized and referenced before starting this sample.
    
**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <cctype>

#include "PI_GCS2_DLL.h"


const float M_PI = 3.14159265358979323846f;

const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;

const unsigned int PARAM_ServoUpdateTime = 0xE000200;
const unsigned int PARAM_IDBufferSize = 0x22000020;



enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


bool PrepareTrajectory (int iD, std::string axis, double* trajectory_x, double* trajectory_y, const int trajectoryPointCount);
bool RunTrajectory (int iD, std::string axis, double* trajectory_x, double* trajectory_y, const int trajectoryPointCount);
bool StartTrajectory (int iD, int* trajectoryIDs);

int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

bool WaitForMotionDone (int ID);
void SetServoState (int ID, std::string axis, BOOL state);
void ReportError (int iD);
