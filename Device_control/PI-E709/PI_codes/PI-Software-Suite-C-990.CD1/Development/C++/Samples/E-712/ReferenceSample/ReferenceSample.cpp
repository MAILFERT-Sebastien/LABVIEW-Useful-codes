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

    This sample connects to a E-712 controller using the connection dialog and references the stage.
    The controller and the stage has to be initialized with PIMikroMove before starting this sample.
    
**/

#include "ReferenceSample.h"


int main ()
{
    int iD = OpenConnection (Dialog);

    if (iD < 0)
    {
        std::cout << "Unable to connect to " << CONTROLLER_TYPE_C712 << "!";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);
        std::string sAxis = "1";

        SetServoState (iD, sAxis.c_str (), SERVO_OFF);

        // start referencing
        if (!PI_FRF (iD, sAxis.c_str ()))
        {
            throw std::runtime_error ("Unable to reference.");
        }

        // Wait until the reference move is done.
        BOOL bRefDone = FALSE;

        while (!bRefDone)
        {
            if (!PI_IsControllerReady (iD, &bRefDone))
            {
                throw std::runtime_error ("Unable to retrieve controller ready state.");
            }
        }

        BOOL bReferenced;

        // check if axis was referenced
        if (!PI_qFRF (iD, sAxis.c_str (), &bReferenced))
        {
            throw std::runtime_error ("Unable to retrieve referenced state.");
        }

        if (!bReferenced)
        {
            std::cout << "Axis " << sAxis << " is not referenced." << std::endl;
        }
        else
        {
            std::cout << "Axis " << sAxis << " is referenced." << std::endl;
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
            iD = PI_InterfaceSetupDlg (CONTROLLER_TYPE_C712.c_str ());
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
            int noDevices = PI_EnumerateUSB (connectedUsbController, 1024, CONTROLLER_TYPE_C712.c_str ());
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

void SetServoState (int ID, std::string axis, BOOL state)
{
    if (!PI_SVO (ID, axis.c_str (), &state))
    {
        throw std::runtime_error ("SVO failed. Exiting");
    }

    std::cout << "Turned servo " << (state ? "on" : "off") << std::endl;
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
