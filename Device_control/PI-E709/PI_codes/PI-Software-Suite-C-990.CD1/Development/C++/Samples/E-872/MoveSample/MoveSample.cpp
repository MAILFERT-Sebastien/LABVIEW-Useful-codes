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

    This sample connects to a E-872 controller using a connection dialog. It shows how to execute an open-loop motion on
    two channels (channels 1 and 2). The controller and the two positioners have to be initialized with PIMikroMove before
    starting this sample.
**/

#include "MoveSample.h"


int main ()
{
    int iD = OpenConnection (Dialog);

    if ( iD < 0 )
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);

        std::vector<int> channels = { 1, 2 };
        int nrOfSteps         = 2000;
        int nrOfRemaningSteps = 0;

        // query controller axes
        char availableAxes [200];
        if ( !PI_qSAI (iD, availableAxes, 199) )
        {
           throw std::runtime_error ("Could not command steps.");
        }
        std::cout << "Available channels:\n" << availableAxes << "\n";

        // execute open-loop motion
        // only one channel at a time can be moved.
        std::cout << "Execute open-loop motion one channel at a time.\n";

        for ( size_t item = 0; item < channels.size(); item++ )
        {
            std::string channelName = std::to_string (channels[item]);
            std::cout << "Executing a motion on channel " << channelName << " (" << nrOfSteps << " steps)" << std::endl;

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

                std::cout << "Remaining steps on " << channelName << " : " << nrOfRemaningSteps << "\n";

                Sleep (10);
            }
            std::cout << "Motion completed on channel " << channelName << ".\n\n";
        }

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

void ReportError (int iD)
{
    int err = PI_GetError (iD);
    char szErrMsg[300];

    if (PI_TranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}
