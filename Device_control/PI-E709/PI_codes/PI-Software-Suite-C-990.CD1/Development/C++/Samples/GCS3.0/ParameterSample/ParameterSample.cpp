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

    This sample connects a controller using the connection dialog. The value of
    the parameter for the maximum velocity is read and the value is modified and
    set as velocity of the axis. The stage has to be referenced before starting
    this sample.

**/

#include "ParameterSample.h"


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
        Parameter paramVelocityAxis1 {"AXIS_1", "TRAJ_1", "0x104"};
        Parameter paramMaxVelocityAxis1 {"AXIS_1", "TRAJ_1", "0x105"};
        double maxVelocity = ReadParameterValue (iD, "RAM", paramMaxVelocityAxis1);
        std::cout << "Maximum velocity of AXIS_1 is " << maxVelocity << "." << std::endl;
        double velocity = maxVelocity / 2;
        WriteParameterValue (iD, "RAM", paramVelocityAxis1, velocity);
        std::cout << "Set velocity of AXIS_1 to " << velocity << "." << std::endl;
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

void WriteParameterValue (int iD, const std::string& memoryType, const Parameter& parameter, const double value)
{
    if (!PI_SPV_Double (iD,
                        memoryType.c_str (),
                        parameter.containerUnitId.c_str (),
                        parameter.functionUnitId.c_str (),
                        parameter.ParameterId.c_str (),
                        value))
    {
        throw std::runtime_error ("Could not write parameter value.");
    }
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
