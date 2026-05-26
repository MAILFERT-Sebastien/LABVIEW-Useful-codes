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
    Then the data recorder is configured and samples of a step response are captured.
    The recorded data is written to std::cout and also to a file, named "test.dat".
    The positioner has to be referenced before starting this sample.

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

        std::string sAxis ("AXIS_1");
        DataRecorderSettings config = {
            "REC_1",
            {1, 2},         // record table IDs
            { {"AXIS_1", "-", "0x103"}, // parameters to be read
              {"AXIS_1", "-", "0x102"} },
            1,              // record table rate
            {"MOV", sAxis, ""},
            "ASCII",
            2,              // read channels
            200             // number of samples to read
        };

        SetupDataRecorder (iD, config);
        SetModeOfOperation (iD, sAxis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);
        DoStep (iD, sAxis, 1);
        SetModeOfOperation (iD, sAxis, MODE_OF_OPERATION_OPEN_LOOP_POSITION);
        WaitForRecordingToFinished (iD, config);
        ReadDataRecorderData (iD, config);
    }
    catch (const std::runtime_error& e)
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

void DoStep (int ID, std::string axis, const double amplitude)
{
    double currentTarget = 0.0;
    if (!PI_qMOV (ID, axis.c_str (), &currentTarget))
    {
        throw std::runtime_error ("Unable to get current target.");
    }
    double targetPosition = currentTarget + amplitude;
    if (!PI_MOV (ID, axis.c_str (), &targetPosition))
    {
        throw std::runtime_error ("Unable to perform step.");
    }

    std::cout << "Step with amplitude of " << amplitude << " completed." << std::endl;

    WaitForMotionDone (ID, axis);
}

void SetupDataRecorder (int ID, DataRecorderSettings config)
{
    std::cout << "Configuring data recorder..." << std::endl;

    // configure data recorder
    if (!PI_REC_RATE (ID, config.recorderId.c_str(), config.recordRate))
    {
        throw std::runtime_error ("Unable to set record table rate.");
    }

    if (!PI_REC_TRACE (ID,
                       config.recorderId.c_str (),
                       config.traceIds[1],
                       config.parameters[1].containerUnitId.c_str (),
                       config.parameters[1].functionUnitId.c_str (),
                       config.parameters[1].ParameterId.c_str ()))
    {
        throw std::runtime_error ("Unable to set data recorder configuration.");
    }

    if (!PI_REC_TRACE (ID,
                       config.recorderId.c_str (),
                       config.traceIds[0],
                       config.parameters[0].containerUnitId.c_str (),
                       config.parameters[0].functionUnitId.c_str (),
                       config.parameters[0].ParameterId.c_str ()))
    {
        throw std::runtime_error ("Unable to set data recorder configuration.");
    }

    if (!PI_REC_TRG(ID,
                    config.recorderId.c_str (),
                    config.trigger.source.c_str (),
                    config.trigger.option1.c_str (),
                    config.trigger.option2.c_str ()))
    {
        throw std::runtime_error ("Unable to configure data recorder trigger.");
    }

    if (!PI_REC_START (ID, config.recorderId.c_str ()))
    {
        throw std::runtime_error ("Unable to activate data recorder.");
    }
}

void ReadDataRecorderData (int ID, DataRecorderSettings config)
{
    double* dataArray;      // pointer to internal data storage array
    char gcsHeader[1025];   // array storage for header

    // start reading asynchronously
    if (!PI_qREC_DAT (ID,
                      config.recorderId.c_str (),
                      config.format.c_str(),
                      1,
                      config.noOfSamplesToRead,
                      config.traceIds,
                      config.noOfTracesToRead,
                      &dataArray,
                      gcsHeader,
                      1024))
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
                config.noOfSamplesToRead = (index - 1) / config.noOfTracesToRead;

                break;
            }
            else
            {
                timeout = 20;
            }
        }

        std::cout << index << std::endl;
    } while (index < (config.noOfSamplesToRead * config.noOfTracesToRead));

    // after the data array is filled with data, you should process it or store it in a local variable,
    // as it will be cleared and/or overwritten with the next recording started by a motion command

    std::cout << "Finished" << std::endl;

    WriteDataRecorderData (gcsHeader, index, config, dataArray);
}

void WriteDataRecorderData (char gcsHeader[301], int& index, DataRecorderSettings& config, double* dataArray)
{
    // The controller is now sending its recorded data. The function returns the Header and
    // a pointer to memory allocated in the dll. You can now analyze the Header information in GcsHeader.
    std::ofstream outputFile;
    outputFile.open ("test.dat");

    std::cout << std::endl << "GCS Header:" << std::endl << gcsHeader << std::endl;
    outputFile << "# GCS Header:\n" << gcsHeader;

    for (index = 0; index < config.noOfSamplesToRead; index++)
    {
        std::ios init (NULL);
        init.copyfmt (std::cout);
        std::cout.fill ('0');
        std::cout.width (3);
        std::cout << std::fixed;
        std::cout.precision (5);
        std::cout << index;
        for (int k = 0; k < config.noOfTracesToRead; k++)
        {
            std::cout << "\t " << dataArray[index * config.noOfTracesToRead + k];
            outputFile << "\t" << std::fixed << dataArray[index * config.noOfTracesToRead + k];
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

void WaitForRecordingToFinished (int ID, DataRecorderSettings config)
{
    int noOfRecordedSamples = 0;
    while (noOfRecordedSamples < config.noOfSamplesToRead)
    {
        if (!PI_qREC_NUM (ID, config.recorderId.c_str (), &noOfRecordedSamples))
        {
            throw std::runtime_error ("Unable to read number of recorded samples.");
        }
        Sleep (100);
    }
}