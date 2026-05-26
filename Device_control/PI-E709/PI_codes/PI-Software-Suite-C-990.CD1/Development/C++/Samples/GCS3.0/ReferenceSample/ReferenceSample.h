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

    This sample connects to a GCS 2.1 controller, startup the connected stage and reference it.

**/

#include <windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "PI_GCS2_DLL.h"

enum ConnectionType
{
    Dialog,
    RS232,
    TCPIP,
    USB
};

const int MODE_OF_OPERATION_OPEN = 0;

int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

bool ReferenceIfNeeded (int ID, std::string axis);
void EnableAxis (int ID, std::string axis);

void ReportError (int iD);
