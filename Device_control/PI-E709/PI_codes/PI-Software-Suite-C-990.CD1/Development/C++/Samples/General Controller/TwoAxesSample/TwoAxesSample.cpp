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

    This sample connects to a two-axis controller.
    Afterwards, it moves the two axes forward and backward repeatedly.
    The two axes have to be referenced before starting this sample.
	Note: The example does not check whether the target position is within the permissible working space of the positioner.
    
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
        std::string sAxis = commonAxesIdentifier;
        
        SetServoState (iD, sAxis, SERVO_ON);
        std::vector<double> minPosLimits = GetMinPositionLimit(iD, sAxis);
        std::vector<double> maxPosLimits = GetMaxPositionLimit(iD, sAxis);
        std::vector<double> travelRange = CalculateTravelRange(minPosLimits, maxPosLimits);

        MoveAbsoluteToNearMinimum (iD, sAxis, minPosLimits, travelRange);
		WaitForMotionDone(iD, "");
        RelativeMoves (iD, sAxis, maxPosLimits, travelRange);
        SetServoState (iD, sAxis, SERVO_OFF);

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

std::vector<double> GetCurrentPosition(int ID, std::string axes)
{
    double currentPositions[NUMBER_OF_AXES];

    if (!PI_qPOS (ID, axes.c_str (), currentPositions))
    {
        throw std::runtime_error ("Unable to query current position.");
    }

    return std::vector<double> (currentPositions, currentPositions + NUMBER_OF_AXES);
}

std::vector<double> GetMinPositionLimit(int ID, std::string axis)
{
    double minPosLimit[NUMBER_OF_AXES];

    if (!PI_qTMN(ID, axis.c_str(), minPosLimit))
    {
        throw std::runtime_error("Unable to get minimum position limit.");
    }

    return std::vector<double> (minPosLimit, minPosLimit + NUMBER_OF_AXES);
}

std::vector<double> GetMaxPositionLimit(int ID, std::string axis)
{
    double maxPosLimit[NUMBER_OF_AXES];

    if (!PI_qTMX(ID, axis.c_str(), maxPosLimit))
    {
        throw std::runtime_error("Unable to get maximum position limit.");
    }

    return std::vector<double> (maxPosLimit, maxPosLimit + NUMBER_OF_AXES);

}

std::vector<double> CalculateTravelRange(std::vector<double> minPosLimits, std::vector<double> maxPosLimits)
{
    double travelRange[NUMBER_OF_AXES];

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        travelRange[idx] = std::abs (maxPosLimits.at(idx)) + std::abs (minPosLimits.at(idx));
    }

    return std::vector<double> (travelRange, travelRange + NUMBER_OF_AXES);
}

bool IsNotNearLimit(std::string axes, std::vector<double> maxPositionLimits, std::vector<double> currentPositions)
{
    bool isNotNearLimit = true;

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        isNotNearLimit = isNotNearLimit && maxPositionLimits.at(idx) * 0.75 > currentPositions.at(idx);
    }

    return isNotNearLimit;
}

void MoveAbsoluteToNearMinimum(int ID, std::string axis, std::vector<double> minPosLimits, std::vector<double> travelRange)
{
    double targetPositions[NUMBER_OF_AXES];

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        targetPositions[idx] = minPosLimits.at(idx) + (travelRange.at(idx) * 0.1);
    }

    if (!PI_MOV (ID, axis.c_str (), targetPositions))
    {
        throw std::runtime_error ("Unable to move axes near minimum position.");
    }
}

void RelativeMoves (int ID, std::string axis, std::vector<double> maxPosLimits, std::vector<double> travelRange)
{
	std::vector<std::string> vAxis;
	vAxis.push_back(axis.substr(0, 1));
	vAxis.push_back(axis.substr(axis.size() - 1, 1));

	std::vector<double> distances = travelRange;
	std::transform(distances.begin(), distances.end(), distances.begin(), [](double x) {return x * 0.1; });
	while (true)
	{
		for (size_t idx = 0; idx < vAxis.size(); idx++)
		{
			std::cout << "Moving axis " << vAxis[idx] << " for " << distances[idx] << " units" << std::endl;

			try
			{
				RelativeMove(ID, vAxis[idx], distances[idx]);
			}
			catch (...)
			{
				// we expect this to fail, since the distances are increased every time
			    std::string msg ("MVR failed. Current axis: ");
			    msg += vAxis[idx].c_str();
			    msg += "; Value: ";
			    msg += std::to_string ( (long long)distances[idx]);
				std::cout << msg << std::endl;
				return;
			}

			WaitForMotionDone(ID, vAxis[idx]);
			Sleep(30);
		}
		std::transform(distances.begin(), distances.end(), distances.begin(),
			[&](double x) {return x * -1.0 * 1.2; });
	}
}

void RelativeMove(int ID, std::string axis, double offset)
{
	if (!PI_MVR (ID, axis.c_str(), &offset))
	{
		throw std::runtime_error("Cannot move to target.");
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
