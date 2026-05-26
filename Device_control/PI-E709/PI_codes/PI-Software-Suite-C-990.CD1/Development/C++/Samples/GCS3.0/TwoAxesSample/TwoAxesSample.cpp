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

    This sample connects to a two-axis controller and references both axes.
    Afterwards, it moves the two axes forward and backward repeatedly.

**/

#include "TwoAxesSample.h"

#include <algorithm>


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

        // based on your controller type, use the appropriate constant
        const std::vector<std::string> axes {"AXIS_1", "AXIS_2"};

        EnableAxes (iD, axes);
        Reference (iD, axes);

        SetModeOfOperation (iD, axes, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);
        std::vector<double> minPosLimits = GetMinPositionLimits (iD, axes);
        std::vector<double> maxPosLimits = GetMaxPositionLimits (iD, axes);
        std::vector<double> travelRange = CalculateTravelRange (minPosLimits, maxPosLimits);

        MoveAbsoluteToNearMinimum (iD, axes, minPosLimits, travelRange);
        WaitForMotionDone (iD, "");
        RelativeMoves (iD, axes, travelRange);
        SetModeOfOperation (iD, axes, MODE_OF_OPERATION_OPEN_LOOP_POSITION);

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

std::vector<double> GetCurrentPositions (int ID, std::vector<std::string> axes)
{
    std::vector<double> currentPositions;

    for (const auto& axis : axes)
    {
        double position;
        if (!PI_qPOS (ID, axis.c_str (), &position))
        {
            throw std::runtime_error ("Unable to query current position.");
        }
        currentPositions.push_back (position);
    }
    return currentPositions;
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


std::vector<double> GetMinPositionLimits(int ID, std::vector<std::string> axes)
{
    std::vector<double> minPosLimits;
    for (const auto& axis : axes)
    {
        minPosLimits.push_back (GetMinPositionLimit (ID, axis));
    }
    return minPosLimits;
}

std::vector<double> GetMaxPositionLimits(int ID, std::vector<std::string> axes)
{
    std::vector<double> maxPosLimits;
    for (const auto& axis : axes)
    {
        maxPosLimits.push_back (GetMaxPositionLimit (ID, axis));
    }
    return maxPosLimits;
}

std::vector<double> CalculateTravelRange (std::vector<double> minPosLimits, std::vector<double> maxPosLimits)
{
    std::vector<double> travelRanges = maxPosLimits;
    std::transform (travelRanges.begin (), travelRanges.end (), minPosLimits.begin (), travelRanges.begin (), std::minus<double> ());
    return travelRanges;
}

void Reference (int ID, std::vector<std::string> axes)
{
    for (const auto& axis : axes)
    {
        ReferenceIfNeeded (ID, axis);
    }
}

void ReferenceIfNeeded (int ID, std::string axis)
{
    BOOL bReferenced;
    BOOL bFlag;

    if (!PI_qFRF (ID, axis.c_str (), &bReferenced))
    {
        throw std::runtime_error ("could not reference axis " + axis);
    }

    if (!bReferenced)
    {
        SetModeOfOperation (ID, axis, MODE_OF_OPERATION_OPEN);
        // if needed, reference the axis using the refence switch
        std::cout << "Referencing axis " << axis << "..." << std::endl;

        if (!PI_FRF (ID, axis.c_str ()))
        {
            throw std::runtime_error ("could not reference axis " + axis);
        }

        // Wait until the reference move is done.
        bFlag = FALSE;

        while (bFlag != TRUE)
        {
            if (!PI_IsControllerReady (ID, &bFlag))
            {
                throw std::runtime_error ("could not reference axis " + axis);
            }
        }

        // check if referencing was successful
        if (!PI_qFRF (ID, axis.c_str (), &bReferenced))
        {
            throw std::runtime_error ("could not reference axis " + axis);
        }

        std::cout << "Successfully referenced axis " << axis << std::endl;

    }
    else
    {
        std::cout << "Axis is already referenced." << std::endl;
    }
}


void MoveAbsoluteToNearMinimum (int ID, std::vector<std::string> axes, std::vector<double> minPosLimits, std::vector<double> travelRange)
{
    std::vector<double> targetPositions;
    for (size_t idx = 0; idx < axes.size (); idx++)
    {
        targetPositions.push_back (minPosLimits[idx] + (travelRange[idx] * 0.1));
    }
    std::string axesString;
    for (const auto& axis : axes)
    {
        axesString += axis + " ";
    }

    if (!PI_MOV (ID, axesString.c_str (), targetPositions.data ()))
    {
        throw std::runtime_error ("Unable to move axes near minimum position.");
    }
}

void RelativeMoves (int ID, std::vector<std::string> axes, std::vector<double> travelRange)
{
    std::vector<double> distances = travelRange;
    std::transform (distances.begin (), distances.end (), distances.begin (), [](double x) {return x * 0.1;});
    while (true)
    {
        for (size_t idx = 0; idx < axes.size (); idx++)
        {
            std::cout << "Moving axis " << axes[idx] << " for " << distances[idx] << " units" << std::endl;

            try
            {
                RelativeMove (ID, axes[idx], distances[idx]);
            }
            catch (...)
            {
                // we expect this to fail, since the distances are increased every time
                std::string msg ("RelativeMove failed. Current axis: ");
                msg += axes[idx];
                msg += "; Value: ";
                msg += std::to_string (distances[idx]);
                std::cout << msg << std::endl;
                return;
            }

            WaitForMotionDone (ID, axes[idx]);
            Sleep (30);
        }
        std::transform (distances.begin (), distances.end (), distances.begin (),
                        [&](double x) {return x * -1.0 * 1.2;});
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

void SetModeOfOperation (int ID, std::vector<std::string> axes, int mode)
{
    for (const auto& axis : axes)
    {
        SetModeOfOperation (ID, axis, mode);
    }
}

void SetModeOfOperation (int ID, std::string axis, int mode)
{
    if (!PI_SAM (ID, axis.c_str (), mode))
    {
        throw std::runtime_error ("SAM failed. Exiting");
    }
    std::cout << "Set mode of operation " << mode << std::endl;
}

void EnableAxes (int ID, std::vector<std::string> axes)
{
    for (const auto& axis : axes)
    {
        const BOOL ENABLED = TRUE;
        if (!PI_EAX (ID, axis.c_str (), &ENABLED))
        {
            throw std::runtime_error ("EAX failed. Exiting");
        }
    }
    std::cout << "Enabled axes" << std::endl;
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

void RelativeMove (int ID, std::string axis, double offset)
{
    double currentTarget = 0.0;
    if (!PI_qMOV (ID, axis.c_str (), &currentTarget))
    {
        throw std::runtime_error ("Cannot read current target.");
    }
    double target = currentTarget + offset;
    if (!PI_MOV (ID, axis.c_str (), &target))
    {
        throw std::runtime_error ("Cannot move to target.");
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

