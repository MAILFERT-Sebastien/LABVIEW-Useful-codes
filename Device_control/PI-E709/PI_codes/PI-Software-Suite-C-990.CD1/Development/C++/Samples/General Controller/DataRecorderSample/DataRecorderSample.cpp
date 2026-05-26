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
    Then the data recorder is configured and samples of a step response are captured.
    The recorded data is written to std::cout and also to a file, named "test.dat".
    The positioner has to be referenced before starting this sample.
	Note: The example does not check whether the target position is within the permissible working space of the positioner.
    
**/

#include "DataRecorderSample.h"


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

        std::string sAxis ("1");
        dataRecorderSettings config = {
            {1, 2},         // record table IDs
            {1, 2},         // record options
            1,              // record table rate
            1,              // trigger source
            "0",            // trigger option set to a dummy value
            2,              // read channels
            200             // number of samples to read
        };

        SetupDataRecorder (iD, sAxis, config);
        SetServoState (iD, sAxis, SERVO_ON);
        DoStep (iD, sAxis, 1);
        SetServoState (iD, sAxis, SERVO_OFF);
        ReadDataRecorderData (iD, sAxis, config);
    }
    catch (std::runtime_error e)
    {
        CloseConnectionWithComment (iD, e.what ());
        return 1;
    }

    PI_CloseConnection (iD);
    std::cout << "Closing connection." << std::endl;

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

void DoStep (int ID, std::string axis, const double amplitude)
{
    if (!PI_STE (ID, axis.c_str (), &amplitude))
    {
        throw std::runtime_error ("Unable to perform step.");
    }

    std::cout << "Step with amplitude of " << amplitude << " completed." << std::endl;

    WaitForMotionDone (ID, axis);
}

void SetupDataRecorder (int ID, std::string axis, dataRecorderSettings config)
{
    std::cout << "Configuring data recorder..." << std::endl;

    // configure data recorder
    if (!PI_RTR (ID, config.tableRate))
    {
        throw std::runtime_error ("Unable to set record table rate.");
    }

    std::stringstream stream;
    stream << axis << " " << axis;
    std::string recordSourceIDs = stream.str ();

    if (!PI_DRC (ID, config.tableIds, recordSourceIDs.c_str (), config.option))
    {
        throw std::runtime_error ("Unable to set data recorder configuration.");
    }

    // for all controllers, except a few, the RecordChannelIdArray for DRT starts with '0'
    // try which drtTable ID fits the controller
    // 0 - default, data recording is triggered with IMP
    // 1 - any command changing target position or open-loop control value
    int supportedDRTTableId = -1;

    for (int drtTableId = 0; drtTableId < 2; drtTableId++)
    {
        if (PI_DRT (ID, &drtTableId, &config.triggerSource, config.triggerOption, 1))
        {
            supportedDRTTableId = drtTableId;
            std::cout << "Supported record channel id for DRT starts with " << supportedDRTTableId << std::endl;
            break;
        }
    }

    if (supportedDRTTableId == -1)
    {
        throw std::runtime_error ("Unable to get supported record channel id for DRT.\n");
    }

    WaitForControllerReady (ID);
}

void ReadDataRecorderData (int ID, std::string axis, dataRecorderSettings config)
{
    double* dataArray;      // pointer to internal data storage array
    char gcsHeader[301];    // array storage for header

    // start reading asynchronously
    if (!PI_qDRR (ID, config.tableIds, config.readChannels, 1, config.noOfSamplesToRead, &dataArray, gcsHeader, 300))
    {
        throw std::runtime_error ("Unable to start reading asynchronously.\n");
    }

    std::cout << "Reading..." << std::endl;

    int index = -1;
    int oldIndex = 0;
    int timeout = 20;

    // wait until the read pointer has reached the number of expected samples
    do
    {
        oldIndex = index;
        Sleep (100);

        // While the controller sends data, the buffer index is being increased,
        // so the data array is filled with valid data
        index = PI_GetAsyncBufferIndex (ID);

        // If the index does not change for about 2 seconds, there is a problem
        if (index == oldIndex)
        {
            timeout--;

            if (timeout < 0)
            {
                std::cout << "Finished reading at " << index << " of " << config.noOfSamplesToRead << " samples."
                    << std::endl << "Stopping read procedure." << std::endl;
                config.noOfSamplesToRead = (index - 1) / config.readChannels;

                break;
            }
            else
            {
                timeout = 20;
            }
        }

        std::cout << index << std::endl;
    } while (index < (config.noOfSamplesToRead * config.readChannels));

    // after the data array is filled with data, you should process it or store it in a local variable,
    // as it will be cleared and/or overwritten with the next recording started by a motion command

    std::cout << "Finished" << std::endl;

    WriteDataRecorderData (gcsHeader, index, config, dataArray);
}

void WriteDataRecorderData (char gcsHeader[301], int& index, dataRecorderSettings& config, double* dataArray)
{
    // The controller is now sending its recorded data. The function returns the Header and
    // a pointer to memory allocated in the dll. You can now analyze the Header information in GcsHeader.
    std::ofstream outputFile;
    outputFile.open ("test.dat");

    std::cout << std::endl << "GCS Header:" << std::endl << gcsHeader << std::endl;
    outputFile << "# GCS Header:\n" << gcsHeader << "\n";

    for (index = 0; index < config.noOfSamplesToRead; index++)
    {
        // Print read data
        // The data columns
        // c1_1 c2_1 c3_1 c4_1
        // c1_2 c2_2 c3_2 c4_2
        // ...
        // c1_n c2_n c3_n c4_n
        // are aligned as follows:
        // OkFlag:
        // {c1_1,c2_1,c3_1,c4_1,c1_2,c2_2,...,c4_n}

        std::ios init (NULL);
        init.copyfmt (std::cout);
        std::cout.fill ('0');
        std::cout.width (3);
        std::cout << std::fixed;
        std::cout.precision (5);
        outputFile.copyfmt (std::cout);
        std::cout << index;
        outputFile << index;

        for (int k = 0; k < config.readChannels; k++)
        {
            std::cout << "\t " << dataArray[index * config.readChannels + k];
            outputFile << "\t" << std::fixed << dataArray[index * config.readChannels + k];
        }

        std::cout << std::endl;
        outputFile << std::endl;
    }

    std::ios init (NULL);

    std::cout.copyfmt (init);
    outputFile.copyfmt (std::cout);
    outputFile.close ();
}

bool WaitForControllerReady (int ID)
{
    BOOL ctlReady = FALSE;

    if (!PI_IsControllerReady (ID, &ctlReady))
    {
        throw std::runtime_error ("Unable to retrieve controller ready state.");
    }

    std::cout << "Controller is ready." << std::endl;

    return true;
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
