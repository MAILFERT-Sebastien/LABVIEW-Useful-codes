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

    This sample shows how to load the PI_GCS2_DLL and its methods dynamically, 
    connect to a controller using a connection dialog and reads the device identification.
    
**/

#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <shlobj.h>
#include <time.h>

#include "PI_GCS2_DLL_PF.h"


// Function Pointer Variables
PFPI_InterfaceSetupDlg pInterfaceSetupDlg;
PFPI_qIDN pqIDN;
PFPI_CloseConnection pCloseConnection;
PFPI_GetError pGetError;
PFPI_TranslateError pTranslateError;

static const std::string GCS_TRANSLATOR_SUB_DIRECTORY ("\\PI\\GCSTranslator\\");
static const std::string GCS_DLLNAME ("PI_GCS2_DLL.dll");
static const std::string GCS_FUNC_PREFIX ("PI_");


enum ConnectionType
{
    Dialog
};


HINSTANCE OpenDLL ();
int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void CloseConnectionWithComment (int iD, std::string comment);

std::string GetPathToDLL ();
void ReportError (int iD);
