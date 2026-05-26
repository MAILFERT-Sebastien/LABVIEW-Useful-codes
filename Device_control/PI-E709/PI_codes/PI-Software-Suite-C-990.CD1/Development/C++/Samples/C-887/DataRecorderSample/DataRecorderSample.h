/**
    (c)2015-2020 Physik Instrumente (PI) SE & Co. KG
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

    This sample connects to a C-887 controller using the PI Connection Dialog.
    Then the datarecorder is configured and samples of a step are captured.
    They are written to std::cout and also to a file, names "test.dat".
    The stage has to be referenced before starting this sample.
    
**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "PI_GCS2_DLL.h"


const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;


enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


struct dataRecorderSettings
{
    int tableIds[2];        // record table to be used
    int option[2];          // record options (1 = commanded position, 2 = measured position)
    int tableRate;          // record table rate (equal to output rate of wave generator)
    int triggerSource;      // 0 - default, data recording is triggered with IMP; 1 - any command changing target position or open-loop control value
    char* dummyValues;      // dummy value, only user for certain trigger sources (see manual for details)
    double relativeMove;    // set relative move
    int readChannels;       // number of channels to read
    int noOfSamplesToRead;  // number of samples to read
};

const std::string CONTROLLER_TYPE_C887 = "C-887";


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

void DoStep (int ID, std::string axis);
void SetupDataRecorder (int ID, std::string axis, dataRecorderSettings config);
void ReadDataRecorderData (int ID, std::string axis, dataRecorderSettings config);
void WriteDataRecorderData (char gcsHeader[301], int& index, dataRecorderSettings& config, double* dataArray);

bool WaitForControllerReady (int ID);
bool WaitForMotionDone (int ID, std::string axis);
void SetServoState (int ID, std::string axis, BOOL state);
void ReportError (int iD);
