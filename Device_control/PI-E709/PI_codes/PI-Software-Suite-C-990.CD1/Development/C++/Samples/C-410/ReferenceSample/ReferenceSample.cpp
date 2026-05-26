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

    This sample connects to a C-410 controller, startup the connected stage and referece it.
    
**/

#include "ReferenceSample.h"


int main ()
{
    int iD = OpenConnection (Dialog);

    if (iD < 0)
    {
        std::cout << "Unable to connect to " << CONTROLLER_TYPE_C410 << "!";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);

        // query controller axes
        char availableAxis[200];

        if (!PI_qSAI_ALL (iD, availableAxis, 199))
        {
            throw new std::runtime_error ("Could not send SAI?");
        }

        std::cout << "Available axis: " << availableAxis << std::endl;

        std::string sAxis = "1";
        const BOOL flag = TRUE;
        sAxis[0] = availableAxis[0];
        sAxis[1] = '\0';

        if (!PI_EAX (iD, sAxis.c_str (), &flag))
        {
            throw new std::runtime_error ("Could not enabled axis.");
        }

        if (!ReferenceIfNeeded (iD, sAxis))
        {
            throw new std::runtime_error ("Not referenced, referencing failed.");
        }

        PI_CloseConnection (iD);
        std::cout << "Closing connection." << std::endl;
    }
    catch (std::runtime_error e)
    {
        CloseConnectionWithComment (iD, e.what ());
        return 1;
    }

    return 0;
}

int OpenConnection (ConnectionType type, std::string hostName)
{
    int iD = 0;
    std::string connType;

    switch (type)
    {
        case Dialog:
            iD = PI_InterfaceSetupDlg (CONTROLLER_TYPE_C410.c_str ());
            connType = "Dialog";
            break;
        case RS232:
            iD = PI_ConnectRS232 (1, 115200);
            connType = "RS232";
            break;
        case TCPIP:
            iD = PI_ConnectTCPIP (hostName.c_str (), 50000);
            connType = "TCPIP";
            break;
        case USB:
            char connectedUsbController[1024];
            int noDevices = PI_EnumerateUSB (connectedUsbController, 1024, CONTROLLER_TYPE_C410.c_str ());
            std::cout << "Found " << noDevices << " controllers connected via USB, connecting to the first one:" << std::endl << connectedUsbController << std::endl;
            char* pos = strchr (connectedUsbController, '\n');

            if (pos)
            {
                *pos = '\0';
            }

            iD = PI_ConnectUSB (connectedUsbController);
            connType = "USB";
            break;
    }

    if (iD < 0)
    {
        ReportError (iD);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return iD;
}

void PrintControllerIdentification (int iD)
{
    char szIDN[200];

    if (!PI_qIDN (iD, szIDN, 199))
    {
        std::runtime_error ("qIDN failed. Exiting.");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void CloseConnectionWithComment (int iD, std::string comment)
{
    std::cout << comment << std::endl;

    ReportError (iD);
    PI_CloseConnection (iD);
}

bool ReferenceIfNeeded (int ID, std::string axis)
{
    BOOL bReferenced;
    BOOL bFlag;

    if (!PI_qFRF (ID, axis.c_str (), &bReferenced))
        return false;

    if (!bReferenced)
    {
        // if needed, reference the axis using the refence switch
        std::cout << "Referencing axis " << axis << "..." << std::endl;

        if (!PI_FRF (ID, axis.c_str ()))
            return false;

        // Wait until the reference move is done.
        bFlag = FALSE;

        while (bFlag != TRUE)
        {
            if (!PI_IsControllerReady (ID, &bFlag))
                return false;
        }

        // check if referencing was successful
        if (!PI_qFRF (ID, axis.c_str (), &bReferenced))
        {
            return false;
        }

        std::cout << "Successfully referenced axis." << std::endl;

    }
    else
    {
        std::cout << "Axis is already referenced." << std::endl;
    }

    return true;
}

void ReportError (int iD)
{
    int err = PI_GetError (iD);
    char szErrMsg[300];

    if (PI_TranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}

