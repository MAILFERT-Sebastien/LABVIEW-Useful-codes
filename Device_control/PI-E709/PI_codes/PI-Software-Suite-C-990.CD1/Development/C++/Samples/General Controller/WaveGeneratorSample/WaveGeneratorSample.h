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
    Then the wave generator is configured and started.
    The positioner has to be referenced before starting this sample.
    
**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "PI_GCS2_DLL.h"


const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;
const int SELECT_ALL_GENERATORS = 0;

const int PARAM_ServoUpdateTime = 234881536;


enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};


struct waveGeneratorSettings
{
    int id;                             // id of wave generator
    int waveTableId;                    // id of wave table
    int startMode;                      // start mode = 1 (start wave generator output synchronized by servo cycle)
    int numberOfCycles;                 // number of wave generator cycles
    double offsetOfWave;                // offset of wave
    double frequency;                   // frequency of wave
    int tableRate;                      // number of servo cycles per point in wave table
    int interpolationType;              // inpterpolation between points, used if piTableRate > 1. 1 = linear interpolation
    int offsetOfFirstPointInWaveTable;  // index of starting point of curve in segment
    int appendWave;                     // 0=clear wave table (1=add wavetable values, 2=append to existing wave table contents)
};


// predefinded wave generator configuration
waveGeneratorSettings commonSettings = {
    1,  // wave generator id
    1,  // wave table id
    1,  // start mode
    5,  // number of cycles
    0,  // wave offset
    2,  // frequency
    2,  // table rate
    1,  // interpolation rate
    0,  // index of starting point of curve in segment
    0   // append wave
};

waveGeneratorSettings c887Settings = {
    1,  // wave generator id
    1,  // wave table id
    1,  // start mode
    5,  // number of cycles
    0,  // wave offset
    2,  // frequency
    10, // table rate
    1,  // interpolation rate
    0,  // index of starting point of curve in segment
    0   // append wave
};


double GetMinPositionLimit (int ID, std::string axis);
double GetMaxPositionLimit (int ID, std::string axis);
double CalculateAmplitude (int ID, std::string axis);
void StartWaveGenerator (int ID, std::string axis, waveGeneratorSettings config);

int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

bool WaitForMotionDone (int ID, std::string axis);
void SetServoState (int ID, std::string axis, BOOL state);
void ReportError (int iD);
