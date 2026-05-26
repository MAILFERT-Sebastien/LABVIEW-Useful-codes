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
    Then the wave generator is configured and started.
    The positioner has to be referenced before starting this sample.
    
**/

#include "WaveGeneratorSample.h"


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

        // check if controller has a wave generator
        int noWaveGenerators = 0;

        if (!PI_qTWG (iD, &noWaveGenerators))
        {
            throw std::runtime_error ("Unable to retrieve number of wave generators.");
        }

        if (noWaveGenerators > 0)
        {
            std::string sAxis ("1");
            // choose from the predefined parameter sets defined in the header file
            waveGeneratorSettings wgConfig = commonSettings;

            SetServoState (iD, sAxis, SERVO_ON);
            StartWaveGenerator (iD, sAxis, wgConfig);
            SetServoState (iD, sAxis, SERVO_OFF);
        }
        else
        {
            std::cout << "Your currently connected controller has no wave generators." << std::endl;
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

double GetMinPositionLimit (int ID, std::string axis)
{
    double minPosLimit = 0;

    if (!PI_qTMN (ID, axis.c_str (), &minPosLimit))
    {
        throw std::runtime_error ("Unable to get minimum position limit.");
    }

    return minPosLimit;
}

double GetMaxPositionLimit (int ID, std::string axis)
{
    double maxPosLimit;

    if (!PI_qTMX (ID, axis.c_str (), &maxPosLimit))
    {
        throw std::runtime_error ("Unable to get maximum position limit.");
    }

    return maxPosLimit;
}

double CalculateAmplitude (int ID, std::string axis)
{
    double amplitude = (std::abs (GetMinPositionLimit (ID, axis)) + std::abs (GetMaxPositionLimit (ID, axis))) / 4;
    return amplitude;
}

void StartWaveGenerator (int ID, std::string axis, waveGeneratorSettings config)
{
    double amplitudeOfWave = CalculateAmplitude (ID, axis);
    std::cout << "Setting up wave generator: Frequency: " << config.frequency << " Hz, Amplitude: " << amplitudeOfWave << " mm" << std::endl;

    // define waveform

    // query servo cycle time
    double servoCycleTime[1];           // servo cycle time of controller
    unsigned int parametersToQuery[1];
    parametersToQuery[0] = PARAM_ServoUpdateTime;

    if (!PI_qSPA (ID, axis.c_str (), parametersToQuery, servoCycleTime, NULL, 0))
    {
        throw std::runtime_error ("Unable to retrieve servo update time.");
    }

    // The available number of points depends on the controller and the settings of the wave tables.
    // calculate number of point in wavetable from given frequency
    int numberOfPoints = (int)(1 / (servoCycleTime[0] * config.frequency));

    // wavetable contains one segment
    int segmentLength = numberOfPoints;

    // curve center point is the middle of the segment
    int centerPointOfWave = numberOfPoints / 2;

    // send wave table to controller
    if (!PI_WAV_SIN_P (ID,
                       config.waveTableId,
                       config.offsetOfFirstPointInWaveTable,
                       numberOfPoints,
                       config.appendWave,
                       centerPointOfWave,
                       amplitudeOfWave,
                       config.offsetOfWave,
                       segmentLength))
    {
        throw std::runtime_error ("Unable to set up waveform.");
    }

    // link wave table to wave generator
    if (!PI_WSL (ID, &config.id, &config.waveTableId, 1))
    {
        throw std::runtime_error ("Unable to link wave table to wave generator.");
    }

    // set wave table rate
    if (!PI_WTR(ID, &SELECT_ALL_GENERATORS, &config.tableRate, &config.interpolationType, 1))
    {
        throw std::runtime_error ("Unable to set wave table rate.");
    }

    // set wave generator output cycles
    if (!PI_WGC (ID, &config.id, &config.numberOfCycles, 1))
    {
        throw std::runtime_error ("Unable to set wave generator output cycles.");
    }

    std::cout << "Starting wave generator..." << std::endl;

    // move stage to offset of wave prior to starting wave generator
    if (!PI_MOV (ID, axis.c_str (), &config.offsetOfWave))
    {
        throw std::runtime_error ("Unable to move axis to start position.");
    }

    WaitForMotionDone (ID, axis);

    // restart recording as soon as wave generator output starts running
    if (!PI_WGR (ID))
    {
        throw std::runtime_error ("Failed to restart recording.");
    }

    // start wave generator output, data recorder starts simultaneously
    if (!PI_WGO (ID, &config.id, &config.startMode, 1))
    {
        throw std::runtime_error ("Unable to start wave generator output.");
    }

    // initialize provisional array for WG Id, 
    // since PI_IsGeneratorRunning expects an array as parameter
    int isWGRunning[1] = { 1 };                   // status of wave generator (0 = not running, 1 = running)
    int waveGeneratorIdArray[1] = { config.id };

    // query wave generator status
    while (isWGRunning[0] == 1)
    {
        Sleep (100);

        if (!PI_IsGeneratorRunning (ID, waveGeneratorIdArray, isWGRunning, 1))
        {
            throw std::runtime_error ("Unable to retrieve wave generator status.");
        }
    }
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
