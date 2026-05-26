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

    This sample connects to a controller using a connection dialog.
    Then a trajectory is set up, transferred to the controller and started.
    The positioner has to be intialized and referenced before starting this sample.
	Note: The example does not check whether the target position is within the permissible working space of the positioner.
    
**/

#include "TrajectorySample.h"


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

        std::string sAxis = "1 2";

        SetServoState (iD, sAxis, SERVO_ON);

        const int trajectoryPoints = 360;
        double trajectory_x[trajectoryPoints];
        double trajectory_y[trajectoryPoints];

        PrepareTrajectory (iD, sAxis, trajectory_x, trajectory_y, trajectoryPoints);
        RunTrajectory (iD, sAxis, trajectory_x, trajectory_y, trajectoryPoints);
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

bool PrepareTrajectory (int iD, std::string axis, double* trajectory_x, double* trajectory_y, const int trajectoryPointCount)
{
    double amplitude = 1.0;
    double centerPoint_x = 0.0;
    double centerPoint_y = 0.0;

    // calculate trajectory values for circular motion

    for (int i = 0; i < trajectoryPointCount; i++)
    {
        trajectory_x[i] = centerPoint_x + (amplitude * sin (2 * M_PI * i / trajectoryPointCount));
        trajectory_y[i] = centerPoint_y + (amplitude * cos (2 * M_PI * i / trajectoryPointCount));
    }

    // move to start position for circular motion
    double startPoint[2];
    startPoint[0] = trajectory_x[0];
    startPoint[1] = trajectory_y[0];

    if (!PI_MOV (iD, axis.c_str (), startPoint))
    {
        throw std::runtime_error ("Unable to move to start positon of trajectory.");
    }

    // wait for the axis to reach target position
    WaitForMotionDone (iD);
    std::cout << "Moved to start position of trajectory." << std::endl;

    // set timing for given period in servo cycles per point
    double servotime = 0.0;
    unsigned int parametersToQuery[1];
    parametersToQuery[0] = PARAM_ServoUpdateTime;

    if (!PI_qSPA (iD, "1", parametersToQuery, &servotime, NULL, 100))
    {
        throw std::runtime_error ("Unable to retrieve servo update time.\n");
    }

    double period = 5.0;
    int trajectoryRate = int (period / trajectoryPointCount / servotime) * 2;

    if (!PI_TGT (iD, trajectoryRate))
    {
        throw std::runtime_error ("TGT failed.");
    }

    std::cout << "Set trajectory rate to " << trajectoryRate << " ("
        << period << " / " << trajectoryPointCount << " / " << servotime << "." << std::endl;

    // clear all trajectories on controller
    int trajectoryIDs[2] = { 1 , 2 };

    if (!PI_TGC (iD, trajectoryIDs, 2))
    {
        throw std::runtime_error ("TGC failed.");
    }

    std::cout << "Successfully prepared trajectory." << std::endl;

    return true;
}

bool RunTrajectory (int iD, std::string axis, double* trajectory_x, double* trajectory_y, const int trajectoryPointCount)
{
    double idBufferSize = 0.0;
    unsigned int parametersToQuery[1];
    parametersToQuery[0] = PARAM_IDBufferSize;

    // get maximum buffer size from controller
    if (!PI_qSPA (iD, "1", parametersToQuery, &idBufferSize, NULL, 100))
    {
        throw std::runtime_error ("Unable to retrieve maximum buffer size from controller.");
    }

    std::cout << "Buffer size: " << idBufferSize << std::endl;

    int trajectoryIDs[2] = { 1, 2 };
    unsigned int idx = 0;
    double currentTrajectoryPoint[2];
    int currentPointsInTrajectory[2];
    bool isTrajectoryRunning = false;

    while (idx < (unsigned int)trajectoryPointCount)
    {
        currentTrajectoryPoint[0] = trajectory_x[idx];
        currentTrajectoryPoint[1] = trajectory_y[idx];

        // get number of trajectory points stored on controller
        if (!PI_qTGL (iD, trajectoryIDs, currentPointsInTrajectory, 2))
        {
            throw std::runtime_error ("qTGL failed.");
        }

        std::cout << "Number of values in buffer: " << currentPointsInTrajectory[0] << ", "
            << currentPointsInTrajectory[1] << std::endl;

        if (currentPointsInTrajectory[0] < (int)idBufferSize && currentPointsInTrajectory[1] < idBufferSize)
        {
            // append points to trajectories on controller
            if (!PI_TGA (iD, trajectoryIDs, currentTrajectoryPoint, 2))
            {
                throw std::runtime_error ("TGA failed.");
            }

            idx++;
            std::cout << idx << " / " << trajectoryPointCount << " points added" << std::endl;
        }
        else
        {
            std::cout << "Start trajectory..." << std::endl;

            if (!isTrajectoryRunning)
            {
                // start trajectories after buffer is filled
                isTrajectoryRunning = StartTrajectory (iD, trajectoryIDs);
            }
            else
            {
                Sleep (1);
            }
        }
    }

    if (!isTrajectoryRunning)
    {
        // start trajectories after trajectory is written to controller
        StartTrajectory (iD, trajectoryIDs);
    }

    // tell the controller that no more points are added to the trajectories
    if (!PI_TGF (iD, trajectoryIDs, 2))
    {
        throw std::runtime_error ("TGF failed.");
    }

    // and wait for the controller to finish trajectories
    while (currentPointsInTrajectory[0] > 0 && currentPointsInTrajectory[1] > 0)
    {
        Sleep (10);

        if (!PI_qTGL (iD, trajectoryIDs, currentPointsInTrajectory, 2))
        {
            throw std::runtime_error ("TGL failed.");
        }
        else
        {
            std::cout << "Number of values in buffer: " << currentPointsInTrajectory[0] << ", "
                << currentPointsInTrajectory[0] << std::endl;
        }
    }

    std::cout << std::endl << "End of trajectory." << std::endl;

    return true;
}

bool StartTrajectory (int iD, int* trajectoryIDs)
{
    if (!PI_TGS (iD, trajectoryIDs, 2))
    {
        throw std::runtime_error ("TGS failed.");
    }

    return true;
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

bool WaitForMotionDone (int ID)
{
    BOOL isMoving = TRUE;

    while (isMoving)
    {
        if (!PI_IsMoving (ID, NULL, &isMoving))
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
