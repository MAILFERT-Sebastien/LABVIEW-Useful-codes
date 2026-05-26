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
    It shows how to determine min/max position limits, and how to do absolute and relative moves.
    The positioner has to be referenced before starting this sample.

**/

#include "MoveSample.h"


int main ()
{
    int iD = OpenConnection (Dialog);

    if (iD < 0)
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);

        std::string sAxis = "AXIS_1";

        SetModeOfOperation (iD, sAxis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);

        double minPosLimit = GetMinPositionLimit (iD, sAxis);
        double maxPosLimit = GetMaxPositionLimit (iD, sAxis);

        std::cout << "Successfully determined min (" << minPosLimit << ") and max (" << maxPosLimit << ") position limits." << std::endl;

        double range = maxPosLimit - minPosLimit;
        double targetPos = minPosLimit + (range * 0.1);

        // do absolute motions
        if (!PI_MOV (iD, sAxis.c_str (), &targetPos))
        {
            throw std::runtime_error ("Unable to approach minimum position limit.");
        }

        WaitForMotionDone (iD, sAxis.c_str ());
        std::cout << "Successfully approached minimum position limit, to " << targetPos << std::endl;

        targetPos = maxPosLimit - (range * 0.1);

        if (!PI_MOV (iD, sAxis.c_str (), &targetPos))
        {
            throw std::runtime_error ("Unable to approach maximum position limit.");
        }

        WaitForMotionDone (iD, sAxis.c_str ());
        std::cout << "Successfully approached maximum position limit, to " << targetPos << std::endl;

        double offset = range * 0.5 * -1;
        RelativeMove (iD, sAxis, offset);
        WaitForMotionDone (iD, sAxis.c_str ());
        std::cout << "Successfully executed relative move backward by " << offset << " units" << std::endl;

        offset = offset * -1;
        RelativeMove (iD, sAxis, offset);
        WaitForMotionDone (iD, sAxis.c_str ());
        std::cout << "Successfully executed relative move forward by " << offset << " units" << std::endl;

        SetModeOfOperation (iD, sAxis.c_str (), MODE_OF_OPERATION_OPEN_LOOP_POSITION);
        PI_CloseConnection (iD);
        std::cout << "Closing connection." << std::endl;
    }
    catch (const std::runtime_error& e)
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
            iD = PI_InterfaceSetupDlg ("");
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
            int noDevices = PI_EnumerateUSB (connectedUsbController, 1024, "");
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
        throw std::runtime_error ("qIDN failed. Exiting.");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void CloseConnectionWithComment (int iD, std::string comment)
{
    std::cout << comment << std::endl;

    ReportError (iD);
    PI_CloseConnection (iD);
}

double GetMinPositionLimit (int ID, std::string axis)
{
    Parameter paramMinPosLimit {axis, "-", "0x121"};
    return ReadParameterValue (ID, "RAM", paramMinPosLimit);
}

double GetMaxPositionLimit (int ID, std::string axis)
{
    Parameter paramMaxPosLimit {axis, "-", "0x122"};
    return ReadParameterValue (ID, "RAM", paramMaxPosLimit);
}

void RelativeMove (int ID, std::string axis, double offset)
{
    double currentTarget = 0.0;
    if (!PI_qMOV(ID, axis.c_str (), &currentTarget))
    {
        throw std::runtime_error ("Cannot read current target.");
    }
    double target = currentTarget + offset;
    if (!PI_MOV (ID, axis.c_str (), &target))
    {
        throw std::runtime_error ("Cannot move to target.");
    }
}

bool WaitForMotionDone (int ID, std::string axis)
{
    BOOL isMoving = TRUE;

    while (isMoving)
    {
        if (!PI_IsMoving (ID, axis.c_str (), &isMoving))
        {
            throw std::runtime_error ("Unable to retrieve motion state.");
        }

        Sleep (100);
    }

    return true;
}

void SetModeOfOperation (int ID, std::string axis, int mode)
{
    if (!PI_SAM (ID, axis.c_str (), mode))
    {
        throw std::runtime_error ("SAM failed. Exiting");
    }

    std::cout << "Set mode of operation " << mode << std::endl;
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

double ReadParameterValue (int iD, const std::string& memoryType, const Parameter& parameter)
{
    double value = 0.0;
    if (!PI_qSPV_Double (iD,
                         memoryType.c_str (),
                         parameter.containerUnitId.c_str(),
                         parameter.functionUnitId.c_str (),
                         parameter.ParameterId.c_str (),
                         &value))
    {
        throw std::runtime_error ("Could not read parameter value.");
    }
    return value;
}

