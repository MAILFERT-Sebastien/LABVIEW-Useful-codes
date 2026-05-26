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
    Then the data recorder is configured and samples of a step response are captured.
    The recorded data is written to std::cout and also to a file, named "test.dat".
    The positioner has to be referenced before starting this sample.

**/

#include <windows.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

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

struct TriggerStettings
{
    std::string source;     // trigger source
    std::string option1;    // first trigger option, only used for certain trigger sources (see manual for details)
    std::string option2;    // second trigger option, only used for certain trigger sources (see manual for details)
};

struct DataRecorderSettings
{
    std::string recorderId;    // recorder id to be used
    int traceIds[2];           // record traces to be used
    Parameter parameters[2];   // parameters to be recorder
    int recordRate;             // record table rate (equal to output rate of wave generator)
    TriggerStettings trigger;
    std::string format;         // format of data to be read
    int noOfTracesToRead;       // number of channels to read
    int noOfSamplesToRead;      // number of samples to read
};


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

void DoStep (int ID, std::string axis, const double amplitude);
void SetupDataRecorder (int ID, DataRecorderSettings config);
void ReadDataRecorderData (int ID, DataRecorderSettings config);
void WriteDataRecorderData (char gcsHeader[301], int& index, DataRecorderSettings& config, double* dataArray);
void WaitForRecordingToFinished (int ID, DataRecorderSettings config);

bool WaitForMotionDone (int ID, std::string axis);
void SetModeOfOperation (int ID, std::string axis, int mode);
void ReportError (int iD);
