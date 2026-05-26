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
    It shows how to determine min/max position limits, how to do absolute moves and how to recover from an axis error.
    The positioner has to be referenced before starting this sample.

**/

#include "ResetErrorSample.h"


int main ()
{
    int id = OpenConnection (Dialog);

    if (id < 0)
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        PrintControllerIdentification (id);

        std::string axis = "AXIS_1";
        std::string pidAxis = "CNTR_1";

        SetModeOfOperation (id, axis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);

        double originalPosition = GetPosition (id, axis);
        double minPosLimit = GetMinPositionLimit (id, axis);
        double maxPosLimit = GetMaxPositionLimit (id, axis);

        std::cout << "Successfully determined min (" << minPosLimit << ") and max (" << maxPosLimit << ") position limits." << std::endl;

        const auto pid = PrepareMotionError (id, axis, pidAxis); // Prepare for motion error with setting PID = 0
        double range = maxPosLimit - minPosLimit;
        double targetPos = minPosLimit + (range * 0.25); // Motion error will not be triggered in this sample, if movement is too small.

        SetPosition (id, axis, targetPos); // do absolute motion
        // let axis "accelerate". Because PID = 0 the axis will stay at current position until motion error is generated.
        Sleep (2000);

        // Note that by default, all axes are grouped via the “Group” configuration parameter (address: Axis_x - 0x50).
        // If an axis from a group goes into the “fault” state, all other axes of the group also go into the “fault” state
        // and require also the error handling.
        RecoverFromError (id, axis, pidAxis, pid);
        SetPosition (id, axis, targetPos); // Continue with absolute motion

        WaitForMotionDone (id, axis.c_str ());
        std::cout << "Successfully approached first quarter of travel range: " << targetPos << std::endl;

        SetPosition (id, axis, originalPosition); // Go back to original position
        WaitForMotionDone (id, axis.c_str ());
        std::cout << "Successfully moved back to original position: " << originalPosition << std::endl;

        SetModeOfOperation (id, axis.c_str (), MODE_OF_OPERATION_OPEN_LOOP_POSITION);
        PI_CloseConnection (id);
        std::cout << "Closing connection." << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        CloseConnectionWithComment (id, e.what ());
        return 1;
    }

    return 0;
}

int OpenConnection (ConnectionType type, std::string hostName)
{
    int id = 0;
    std::string connType;

    switch (type)
    {
        case Dialog:
            id = PI_InterfaceSetupDlg ("");
            connType = "Dialog";
            break;
        case RS232:
            id = PI_ConnectRS232 (1, 115200);
            connType = "RS232";
            break;
        case TCPIP:
            id = PI_ConnectTCPIP (hostName.c_str (), 50000);
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

            id = PI_ConnectUSB (connectedUsbController);
            connType = "USB";
            break;
    }

    if (id < 0)
    {
        ReportError (id);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return id;
}

void PrintControllerIdentification (int id)
{
    char szIDN[200];

    if (!PI_qIDN (id, szIDN, 199))
    {
        throw std::runtime_error ("qIDN failed. Exiting.");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void CloseConnectionWithComment (int id, std::string comment)
{
    std::cout << comment << std::endl;

    ReportError (id);
    PI_CloseConnection (id);
}

double GetPosition (int id, std::string axis)
{
    double position = 0.0;
    if (!PI_qPOS (id, axis.c_str (), &position))
    {
        throw std::runtime_error ("Unable to retrieve current position.");
    }
    return position;
}

void SetPosition (int id, std::string axis, double position)
{
    if (!PI_MOV (id, axis.c_str (), &position))
    {
        throw std::runtime_error ("Unable to approach target position.");
    }
}

double GetMinPositionLimit (int id, std::string axis)
{
    Parameter paramMinPosLimit {axis, "-", "0x121"};
    return ReadParameterValue (id, "RAM", paramMinPosLimit);
}

double GetMaxPositionLimit (int id, std::string axis)
{
    Parameter paramMaxPosLimit {axis, "-", "0x122"};
    return ReadParameterValue (id, "RAM", paramMaxPosLimit);
}

bool WaitForMotionDone (int id, std::string axis)
{
    BOOL isMoving = TRUE;

    while (isMoving)
    {
        if (!PI_IsMoving (id, axis.c_str (), &isMoving))
        {
            throw std::runtime_error ("Unable to retrieve motion state.");
        }

        Sleep (100);
    }

    return true;
}

void SetModeOfOperation (int id, std::string axis, int mode)
{
    if (!PI_SAM (id, axis.c_str (), mode))
    {
        throw std::runtime_error ("SAM failed. Exiting");
    }

    std::cout << "Set mode of operation " << mode << std::endl;
}

void ReportError (int id)
{
    int err = PI_GetError (id);
    char szErrMsg[300];

    if (PI_TranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}

double ReadParameterValue (int id, const std::string& memoryType, const Parameter& parameter)
{
    double value = 0.0;
    if (!PI_qSPV_Double (id,
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

void WriteParameterValue (int id, const std::string& memoryType, const Parameter& parameter, double value)
{
    if (!PI_SPV_Double (id,
                        memoryType.c_str (),
                        parameter.containerUnitId.c_str(),
                        parameter.functionUnitId.c_str (),
                        parameter.ParameterId.c_str (),
                        value))
    {
        throw std::runtime_error ("Could not write parameter value.");
    }
}

PID PrepareMotionError (int id, std::string axis, std::string pid)
{
    // Higher user level is required for writing parameters. See password in your controller manual.
    if (!PI_UCL (id, "ADV", "advanced"))
    {
        throw std::runtime_error ("Unable to change user level.");
    }
    std::cout << "Changed user level to ADV." << std::endl;

    PID result;
    result.P = ReadParameterValue (id, "RAM", {axis, pid, "0x120,1"});
    result.I = ReadParameterValue (id, "RAM", {axis, pid, "0x121,1"});
    result.D = ReadParameterValue (id, "RAM", {axis, pid, "0x122,1"});
    WriteParameterValue (id, "RAM", {axis, pid, "0x120,1"}, 0.0);
    WriteParameterValue (id, "RAM", {axis, pid, "0x121,1"}, 0.0);
    WriteParameterValue (id, "RAM", {axis, pid, "0x122,1"}, 0.0);
    return result;
}

void RecoverFromError (int id, std::string axis, std::string pidAxis, PID oldPID)
{
    if (!AxisHasError (id, axis))
    {
        throw std::runtime_error ("Axis expected to be in error state. Did PrepareMotionError() fail or is absolute movement too small?");
    }
    std::cout << "Axis is now in error state." << std::endl;

    ResetAxesError (id);

    // Restore PID parameters to allow movement
    WriteParameterValue (id, "RAM", {axis, pidAxis, "0x120,1"}, oldPID.P);
    WriteParameterValue (id, "RAM", {axis, pidAxis, "0x121,1"}, oldPID.I);
    WriteParameterValue (id, "RAM", {axis, pidAxis, "0x122,1"}, oldPID.D);

    // Enable axis
    BOOL enable = TRUE;
    if (!PI_EAX (id, axis.c_str (), &enable))
    {
        throw std::runtime_error ("Unable to enable axis.");
    }
    std::cout << "Enabled axis" << std::endl;

    SetModeOfOperation (id, axis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);
}

void ResetAxesError (int id)
{
    auto axes = ReadAllAxesNames (id);
    for (const auto& axis : axes)
    {
        if (AxisHasError (id, axis))
        {
            while (CheckAxisStatusValue (id, axis, START_BIT_DSM, DSM_BIT_MASK) == DSM_FAULT_REACTION_BITS)
            {}
            std::cout << "reset axis error: " << axis << std::endl;
            if (!PI_RES (id, axis.c_str ()))
            {
                throw std::runtime_error ("Unable to reset error.");
            }
        }
    }
}

bool AxisHasError (int id, std::string axis)
{
    return CheckAxisStatusValue (id, axis, START_BIT_DSM, DSM_ERROR_BIT) == DSM_ERROR_BIT;
}

unsigned int CheckAxisStatusValue(int id, std::string axis, unsigned int startBit, unsigned int bitMask)
{
    unsigned int status = 0;
    if (!PI_qSTV (id, axis.c_str (), &status))
    {
        throw std::runtime_error ("qSTV failed");
    }

    return (status >> startBit) & bitMask;
}

std::vector<std::string> ReadAllAxesNames (int id)
{
    char allAxes[200];
    if (!PI_qSAI (id, allAxes, 199))
    {
        throw std::runtime_error ("Unable to get list of axes.");
    }
    std::vector<std::string> axes;
    const char* str = allAxes;
    do
    {
        const char* begin = str;

        while (*str != ' ' && *str != '\n' && *str)
        {
            str++;
        }
        std::string axis (begin, str);
        if (!axis.empty ())
        {
            axes.push_back (axis);
        }
    } while (0 != *str++);

    return axes;
}
