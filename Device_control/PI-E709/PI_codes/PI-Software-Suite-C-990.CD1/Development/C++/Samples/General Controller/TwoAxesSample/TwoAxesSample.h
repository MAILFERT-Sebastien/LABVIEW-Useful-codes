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

	This sample connects to a two-axis controller.
	Afterwards, it moves the two axes forward and backward repeatedly.
	The two axes have to be referenced before starting this sample.
	Note: The example does not check whether the target position is within the permissible working space of the positioner.
    
**/

#include <windows.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <string>

#include "PI_GCS2_DLL.h"


const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;
const int NUMBER_OF_AXES = 2;

const std::string commonAxesIdentifier = "1 2";
const std::string c887AxesIdentifier = "X Y";

enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int id);
void CloseConnectionWithComment (int iD, std::string comment);

std::vector<double> GetCurrentPosition (int ID, std::string axes);
std::vector<double> GetMinPositionLimit (int ID, std::string axes);
std::vector<double> GetMaxPositionLimit (int ID, std::string axes);
std::vector<double> CalculateTravelRange (std::vector<double> minPosLimits, std::vector<double> maxPosLimits);
bool IsNotNearLimit (std::string axes, std::vector<double> maxPositionLimits, std::vector<double> currentPositions);

void MoveAbsoluteToNearMinimum(int ID, std::string axes, std::vector<double> minPosLimits, std::vector<double> travelRange);
void RelativeMoves (int ID, std::string axes, std::vector<double> maxPosLimits, std::vector<double> travelRange);
void RelativeMove(int ID, std::string axis, double offset);

bool WaitForMotionDone (int ID, std::string axes);
void SetServoState (int ID, std::string axis, BOOL state);
void ReportError (int iD);
