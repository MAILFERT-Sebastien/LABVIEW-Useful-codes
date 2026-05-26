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

    This sample connects to a E-872 controller using a connection dialog. It shows how to execute an open-loop motion on
    two channels (channels 1 and 2). The controller and the two positioners have to be initialized with PIMikroMove before starting this sample.
**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

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


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);
void ReportError (int iD);
