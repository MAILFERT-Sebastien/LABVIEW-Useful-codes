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

    This sample connects to a E-135 controller using a connection dialog. It shows how to execute an open-loop motion on
    two channels (channels 1 and 2) using three different methods (POL, SVA and OSM/OSN?). The controller and the positioners
    have to be initialized with PIMikroMove before starting this sample.
**/

#include "MoveSample.h"


int main ()
{
    int iD = OpenConnection (Dialog); //USB, "0123456789"

    if ( iD < 0 )
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);

        // query controller axes
        char availableAxes [200];
        if ( !PI_qSAI (iD, availableAxes, 199) )
        {
            throw std::runtime_error ("Could not command steps.");
        }
        std::cout << "Available channels:\n" << availableAxes << "\n";

        // There are 3 ways to execute an open-loop motion for E-135: 1) POL command, 2) SVA command 3) OSM/OSN? commands
        // Only one channel at a time can be moved.

        std::vector <int> channels = { 1, 2 };

        ExecuteMotionUsingPOL (iD, channels);

        ExecuteMotionUsingSVA (iD, channels);

        ExecuteMotionUsingOSN (iD, channels);

        PI_CloseConnection (iD);
        std::cout << "Closing connection." << std::endl;
    }
    catch ( std::runtime_error e )
    {
        CloseConnectionWithComment (iD, e.what ());
        return 1;
    }

    return 0;
}


int OpenConnection (ConnectionType type, std::string hostName)
{
    int         iD = 0;
    std::string connType;

    switch ( type )
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
            std::cout << "Found " << noDevices << " controllers connected via USB, connecting to the first one:" << std::endl <<
                connectedUsbController << std::endl;
            char* pos = strchr (connectedUsbController, '\n');

            if ( pos )
            {
                *pos = '\0';
            }

            iD       = PI_ConnectUSB (connectedUsbController);
            connType = "USB";
            break;
    }

    if ( iD < 0 )
    {
        ReportError (iD);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return iD;
}


void PrintControllerIdentification (int iD)
{
    char szIDN[200];

    if ( !PI_qIDN (iD, szIDN, 199) )
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


void ReportError (int iD)
{
    int  err = PI_GetError (iD);
    char szErrMsg[300];

    if ( PI_TranslateError (err, szErrMsg, 299) )
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}


void ExecuteMotionUsingPOL (int iD, std::vector <int> channels)
{
    // Only one channel at a time can be moved.
    for ( size_t item = 0; item < channels.size (); item++ )
    {
        std::string channelName = std::to_string (channels[item]);

        //Move actuator to the maximum of its travel range
        std::cout << "Move channel " << channelName << " to the maximum of its travel range using POL command" << std::endl;
        int maximumTravelRange = 1;
        if ( !PI_POL (iD, channelName.c_str (), &maximumTravelRange) )
        {
            throw std::runtime_error ("Could not move to maximum of travel range.");
        }

        WaitForMotionToFinish (iD, channelName.c_str ());

        //Move actuator to the minimum of its travel range
        std::cout << "Move channel " << channelName << " to the minimum of its travel range using POL command" << std::endl;
        int minimumTravelRange = 0;
        if ( !PI_POL (iD, channelName.c_str (), &minimumTravelRange) )
        {
            throw std::runtime_error ("Could not move to minimum of travel range.");
        }

        WaitForMotionToFinish (iD, channelName.c_str ());

        std::cout << "Motion completed on channel " << channelName << ".\n\n";
    }
}


void ExecuteMotionUsingSVA (int iD, std::vector <int> channels)
{
    // Only one channel at a time can be moved.
    for ( size_t item = 0; item < channels.size (); item++ )
    {
        std::string channelName = std::to_string (channels[item]);

        //Move actuator to a 30% of its travel range
        double maximumTravelRange = 30.0;
        std::cout << "Move channel " << channelName << " to " << maximumTravelRange << "% ot its travel range using SVA command" << std::
            endl;
        if ( !PI_SVA (iD, channelName.c_str (), &maximumTravelRange) )
        {
            throw std::runtime_error ("Could not move to specified target within travel range");
        }

        WaitForMotionToFinish (iD, channelName.c_str ());

        std::cout << "Motion completed on channel " << channelName << ".\n\n";
    }
}


void ExecuteMotionUsingOSN (int iD, std::vector <int> channels)
{
    int nrOfSteps         = 2;
    int nrOfRemaningSteps = 0;

    // Only one channel at a time can be moved.
    for ( size_t item = 0; item < channels.size (); item++ )
    {
        std::string channelName = std::to_string (channels[item]);
        std::cout << "Executing a motion on channel " << channelName << " using OSM/OSN? (" << nrOfSteps << " steps)" << std::endl;

        if ( !PI_OSM (iD, &(channels[item]), &nrOfSteps, 1) )
        {
            throw std::runtime_error ("Could not command steps.");
        }

        // wait for motion to stop
        BOOL isMoving = TRUE;
        while ( isMoving )
        {
            if ( !PI_IsMoving (iD, channelName.c_str (), &isMoving) )
            {
                throw std::runtime_error ("Unable to retrieve motion state.");
            }

            if ( !PI_qOSN (iD, &(channels[item]), &nrOfRemaningSteps, 1) )
            {
                throw std::runtime_error ("Unable to query remaining steps.");
            }

            std::cout << "Remaining steps on " << channelName << " : " << nrOfRemaningSteps << ".\n";

            Sleep (200);
        }
        std::cout << "Motion completed on channel " << channelName << ".\n\n";
    }
}


void WaitForMotionToFinish (int iD, std::string channel)
{
    BOOL isMoving = true;
    while ( isMoving )
    {
        if ( !PI_IsMoving (iD, channel.c_str (), &isMoving) )
        {
            throw std::runtime_error ("Unable to retrieve motion state.");
        }

        Sleep (200);
    }
}

void SetStepWidthUsingVoltageIncrementParameter(int iD, std::string channel)
{
    //Set step width by setting the voltage increment parameter
    double       voltageIncrement[1];
    unsigned int parameterName[1];
    parameterName[0] = PARAM_VoltageIncrement;

    //Read the parameter value
    if ( !PI_qSPA (iD, channel.c_str (), parameterName, voltageIncrement, NULL, 0) )
    {
        throw std::runtime_error ("Unable to read voltage increment parameter.");
    }
    std::cout << "Step width on " << channel << " is "<< voltageIncrement<<" V.\n\n";


    //Set the parameter value
    if ( !PI_SPA (iD, channel.c_str (), parameterName, voltageIncrement, NULL) )
    {
        throw std::runtime_error ("Unable to write voltage increment parameter.");
    }
}
