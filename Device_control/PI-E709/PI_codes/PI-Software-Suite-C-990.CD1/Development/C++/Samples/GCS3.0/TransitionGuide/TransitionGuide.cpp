/**
    (c)2021 Physik Instrumente (PI) SE & Co. KG
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

    This sample connects to two controllers using connection dialogs.
    It demonstrates the differences between GCS 2.0 and GCS 3.0 devices:
      - referencing
      - changing velocities
      - motion
      - recording data
      - recover after error

**/

#include "TransitionGuide.h"

#include <algorithm>


int main ()
{
    const int ID_GCS20 = PI_InterfaceSetupDlg ("TransitionGuide\\GCS20_Device");
    if (ID_GCS20 < 0)
    {
        std::cout << "Unable to connect GCS2.0 device.";
        return 1;
    }

    const int ID_GCS30 = PI_InterfaceSetupDlg ("TransitionGuide\\GCS30_Device");
    if (ID_GCS30 < 0)
    {
        std::cout << "Unable to connect GCS3.0 device.";
        return 1;
    }

    int result = 0;

    try
    {
        PrintControllerIdentification (ID_GCS20);
        PrintControllerIdentification (ID_GCS30);

        const std::vector<std::string> axesGCS20 { "1", "2" };
        const std::vector<std::string> axesGCS30 { "AXIS_1", "AXIS_2" };

        //////////////////////////////////////////////////////////
        //
        //  referencing
        //

        StartReferenceGCS20 (ID_GCS20, axesGCS20);
        StartReferenceGCS30 (ID_GCS30, axesGCS30);

        std::cout << "... referencing ..." << std::endl;

        WaitWhileReferencing (ID_GCS20);
        WaitWhileReferencing (ID_GCS30);

        //////////////////////////////////////////////////////////
        //
        //  change velocities
        //

        ChangeVelocitiesGCS20 (ID_GCS20, axesGCS20);
        ChangeVelocitiesGCS30 (ID_GCS30, axesGCS30);

        //////////////////////////////////////////////////////////
        //
        //  closed loop positioning
        //

        auto minPosGCS20 = GetMinPositionLimitsGCS20 (ID_GCS20, axesGCS20);
        auto minPosGCS30 = GetMinPositionLimitsGCS30 (ID_GCS30, axesGCS30);
        auto maxPosGCS20 = GetMaxPositionLimitsGCS20 (ID_GCS20, axesGCS20);
        auto maxPosGCS30 = GetMaxPositionLimitsGCS30 (ID_GCS30, axesGCS30);
        auto posGCS20 = GetCurrentPositionsGCS20 (ID_GCS20, axesGCS20);
        auto posGCS30 = GetCurrentPositionsGCS30 (ID_GCS30, axesGCS30);

        for (size_t i = 0; i < axesGCS20.size (); ++i)
        {
            std::cout << axesGCS20[i] << ": " << minPosGCS20[i] << " - " << posGCS20[i] << " - " << maxPosGCS20[i] << std::endl;
        }
        for (size_t i = 0; i < axesGCS30.size (); ++i)
        {
            std::cout << axesGCS30[i] << ": " << minPosGCS30[i] << " - " << posGCS30[i] << " - " << maxPosGCS30[i] << std::endl;
        }

        PrepareForClosedLoopPositioningGCS20 (ID_GCS20, axesGCS20);
        PrepareForClosedLoopPositioningGCS30 (ID_GCS30, axesGCS30);

        std::vector<double> targetsGCS20;
        for (size_t i = 0; i < axesGCS20.size (); ++i)
        {
            targetsGCS20.push_back ((minPosGCS20[i] + maxPosGCS20[i]) / 2);
        }
        std::vector<double> targetsGCS30;
        for (size_t i = 0; i < axesGCS30.size (); ++i)
        {
            targetsGCS30.push_back ((minPosGCS30[i] + maxPosGCS30[i]) / 2);
        }

        MoveToTargetPosition (ID_GCS20, axesGCS20, targetsGCS20);
        MoveToTargetPosition (ID_GCS30, axesGCS30, targetsGCS30);

        std::cout << "... moving ..." << std::endl;

        WaitForMotion (ID_GCS20);
        WaitForMotion (ID_GCS30);

        posGCS20 = GetCurrentPositionsGCS20 (ID_GCS20, axesGCS20);
        posGCS30 = GetCurrentPositionsGCS30 (ID_GCS30, axesGCS30);
        for (size_t i = 0; i < axesGCS20.size (); ++i)
        {
            std::cout << axesGCS20[i] << ": " << posGCS20[i] << std::endl;
        }
        for (size_t i = 0; i < axesGCS30.size (); ++i)
        {
            std::cout << axesGCS30[i] << ": " << posGCS30[i] << std::endl;
        }

        //////////////////////////////////////////////////////////
        //
        //  positioning relative to current target position
        //

        std::vector<double> offsetsGCS20;
        for (size_t i = 0; i < axesGCS20.size (); ++i)
        {
            offsetsGCS20.push_back ((maxPosGCS20[i] - minPosGCS20[i]) / 10);
        }
        std::vector<double> offsetsGCS30;
        for (size_t i = 0; i < axesGCS30.size (); ++i)
        {
            offsetsGCS30.push_back ((maxPosGCS30[i] - minPosGCS30[i]) / 10);
        }

        MoveRelativeToTargetPositionGCS20 (ID_GCS20, axesGCS20, offsetsGCS20);
        MoveRelativeToTargetPositionGCS30 (ID_GCS30, axesGCS30, offsetsGCS30);

        std::cout << "... moving ..." << std::endl;

        WaitForMotion (ID_GCS20);
        WaitForMotion (ID_GCS30);

        posGCS20 = GetCurrentPositionsGCS20 (ID_GCS20, axesGCS20);
        posGCS30 = GetCurrentPositionsGCS30 (ID_GCS30, axesGCS30);
        for (size_t i = 0; i < axesGCS20.size (); ++i)
        {
            std::cout << axesGCS20[i] << ": " << posGCS20[i] << std::endl;
        }
        for (size_t i = 0; i < axesGCS30.size (); ++i)
        {
            std::cout << axesGCS30[i] << ": " << posGCS30[i] << std::endl;
        }

        //////////////////////////////////////////////////////////
        //
        //  data recorder
        //

        PrepareDataRecorderForPositionAndTargetGCS20 (ID_GCS20, axesGCS20[0]);
        MoveRelativeToTargetPositionGCS20 (ID_GCS20, {axesGCS20[0]}, {offsetsGCS20[0]});
        WaitForDataRecorderDataGCS20 (ID_GCS20, 1024);
        auto dataGCS20 = ReadDataRecorderDataGCS20 (ID_GCS20, 1024);
        for (size_t i=0; i<20; ++i)
        {
            std::cout << axesGCS20[0] << " " << i + 1 << ": " << dataGCS20[i * 2] << ", " << dataGCS20[(i * 2) + 1] << std::endl;
        }
        PrepareDataRecorderForPositionAndTargetGCS30 (ID_GCS30, "REC_1", axesGCS30[0]);
        MoveRelativeToTargetPositionGCS30 (ID_GCS30, {axesGCS30[0]}, {offsetsGCS30[0]});
        WaitForDataRecorderGCS30 (ID_GCS30, "REC_1", 1024);
        auto dataGCS30 = ReadDataRecorderDataGCS30 (ID_GCS30, "REC_1", 1024);
        for (size_t i=0; i<20; ++i)
        {
            std::cout << axesGCS30[0] << " " << i + 1 << ": " << dataGCS30[i * 2] << ", " << dataGCS30[(i * 2) + 1] << std::endl;
        }

        WaitForMotion (ID_GCS20);
        WaitForMotion (ID_GCS30);

        //////////////////////////////////////////////////////////
        //
        //  recover from error (only GCS 3.0) if any error appeared
        //

        // RecoverFromErrorGCS30 (ID_GCS30, axesGCS30);
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what () << std::endl;
        ReportError (ID_GCS20);
        ReportError (ID_GCS30);
        result = 1;
    }

    PI_CloseConnection (ID_GCS20);
    PI_CloseConnection (ID_GCS30);

    return result;
}

void PrintControllerIdentification (int iD)
{
    char szIDN[200];

    if (!PI_qIDN (iD, szIDN, 199))
    {
        throw std::runtime_error ("qIDN failed");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void ReportError (int iD)
{
    int err = PI_GetError (iD);
    if (err != 0)
    {
        char szErrMsg[300];
        if (PI_TranslateError (err, szErrMsg, 299))
        {
            std::cout << "Error " << err << ": " << szErrMsg << std::endl;
        }
    }
}

void StartReferenceGCS20 (const int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });

    // C-891 and C-410 need to activate axes with EAX
    // but most GCS 2.0 devices do not support EAX
    // const BOOL ACTIVATED = TRUE;
    // std::vector<BOOL> eaxState (axes.size (), ACTIVATED);
    // if (!PI_EAX(iD, axesString.c_str(), eaxState.data()))
    // {
    //     throw std::runtime_error ("EAX failed");
    // }

    // some GCS 2.0 devices need to be in the servo-off state,
    // some need to be in the servo-on state,
    // some don't care and handle the servo state internally

    const BOOL SERVO_OFF = FALSE;
    const BOOL SERVO_ON = TRUE;
    // std::vector<BOOL> svoState (axes.size (), SERVO_OFF);
    std::vector<BOOL> svoState (axes.size (), SERVO_ON);
    if (!PI_SVO (iD, axesString.c_str (), svoState.data ()))
    {
        throw std::runtime_error ("SVO failed");
    }

    if (!PI_FRF (iD, axesString.c_str ()))
    {
        throw std::runtime_error ("FRF failed");
    }
}

void StartReferenceGCS30 (const int iD, const std::vector<std::string>& axes)
{
    for (const auto& axis : axes)
    {
        // axis needs to be activated and in mode-of-operation "open"

        const BOOL ACTIVATED = TRUE;
        if (!PI_EAX (iD, axis.c_str (), &ACTIVATED))
        {
            throw std::runtime_error ("EAX failed");
        }

        const int MODE_OF_OPERATION_OPEN = 0;
        if (!PI_SAM (iD, axis.c_str (), MODE_OF_OPERATION_OPEN))
        {
            throw std::runtime_error ("SAM failed");
        }

        if (!PI_FRF (iD, axis.c_str ()))
        {
            throw std::runtime_error ("FRF failed");
        }
    }
}

void WaitWhileReferencing (const int iD)
{
    BOOL flag = FALSE;
    while (flag != TRUE)
    {
        if (!PI_IsControllerReady (iD, &flag))
        {
            throw std::runtime_error ("IsControllerReady failed");
        }
    }
}

std::vector<double> GetMinPositionLimitsGCS20 (int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    std::vector<double> minPos (axes.size ());
    if (!PI_qTMN (iD, axesString.c_str (), minPos.data ()))
    {
        throw std::runtime_error ("qTMN failed");
    }
    return minPos;
}

std::vector<double> GetMaxPositionLimitsGCS20 (int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    std::vector<double> maxPos (axes.size ());
    if (!PI_qTMX (iD, axesString.c_str (), maxPos.data ()))
    {
        throw std::runtime_error ("qTMX failed");
    }
    return maxPos;
}

std::vector<double> GetCurrentPositionsGCS20 (int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    std::vector<double> pos (axes.size ());
    if (!PI_qPOS (iD, axesString.c_str (), pos.data ()))
    {
        throw std::runtime_error ("qPOS failed");
    }
    return pos;
}

std::vector<double> GetMinPositionLimitsGCS30 (int iD, const std::vector<std::string>& axes)
{
    std::vector<double> minPos;
    for (const auto& axis : axes)
    {
        double value;
        if (!PI_qSPV_Double (iD, "RAM", axis.c_str (), "-", "0x121", &value))
        {
            throw std::runtime_error ("qSPV_Double failed");
        }
        minPos.push_back (value);
    }
    return minPos;
}

std::vector<double> GetMaxPositionLimitsGCS30 (int iD, const std::vector<std::string>& axes)
{
    std::vector<double> maxPos;
    for (const auto& axis : axes)
    {
        double value;
        if (!PI_qSPV_Double (iD, "RAM", axis.c_str (), "-", "0x122", &value))
        {
            throw std::runtime_error ("qSPV_Double failed");
        }
        maxPos.push_back (value);
    }
    return maxPos;
}

std::vector<double> GetCurrentPositionsGCS30 (int iD, const std::vector<std::string>& axes)
{
    std::vector<double> pos;
    for (const auto& axis : axes)
    {
        double value;
        if (!PI_qPOS (iD, axis.c_str (), &value))
        {
            throw std::runtime_error ("qPOS failed");
        }
        pos.push_back (value);
    }
    return pos;
}

void PrepareForClosedLoopPositioningGCS20 (int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });

    // C-891 and C-410 need to activated axes with EAX
    // but most GCS 2.0 devices do not support EAX
    // const BOOL ACTIVATED = TRUE;
    // std::vector<BOOL> eaxState (axes.size (), ACTIVATED);
    // if (!PI_EAX(iD, axesString.c_str(), eaxState.data()))
    // {
    //     throw std::runtime_error ("EAX failed");
    // }

    const BOOL SERVO_ON = TRUE;
    std::vector<BOOL> svoState (axes.size (), SERVO_ON);
    if (!PI_SVO (iD, axesString.c_str (), svoState.data ()))
    {
        throw std::runtime_error ("SVO failed");
    }
}

void PrepareForClosedLoopPositioningGCS30 (int iD, const std::vector<std::string>& axes)
{
    for (const auto& axis : axes)
    {
        // axis needs to be activated and in mode-of-operation "closed loop position"

        const BOOL ACTIVATED = TRUE;
        if (!PI_EAX (iD, axis.c_str (), &ACTIVATED))
        {
            throw std::runtime_error ("EAX failed");
        }

        const int MODE_OF_OPERATION_CLOSED_LOOP_POSITION = 2;
        if (!PI_SAM (iD, axis.c_str (), MODE_OF_OPERATION_CLOSED_LOOP_POSITION))
        {
            throw std::runtime_error ("SAM failed");
        }
    }
}

void MoveToTargetPosition (int iD, const std::vector<std::string>& axes, const std::vector<double>& targets)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    if (!PI_MOV (iD, axesString.c_str (), targets.data ()))
    {
        throw std::runtime_error ("MOV failed");
    }
}

bool WaitForMotion (int iD)
{
    BOOL isMoving = TRUE;
    while (isMoving)
    {
        if (!PI_IsMoving (iD, "", &isMoving))
        {
            throw std::runtime_error ("IsMoving Failed");
        }
    }

    return true;
}

void MoveRelativeToTargetPositionGCS20 (int iD, const std::vector<std::string>& axes, const std::vector<double>& offsets)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    if (!PI_MVR (iD, axesString.c_str (), offsets.data ()))
    {
        throw std::runtime_error ("MVR failed");
    }
}

void MoveRelativeToTargetPositionGCS30 (int iD, const std::vector<std::string>& axes, const std::vector<double>& offsets)
{
    std::vector<double> targets;
    for (const auto& axis : axes)
    {
        double value;
        if (!PI_qMOV (iD, axis.c_str (), &value))
        {
            throw std::runtime_error ("qMOV failed");
        }
        targets.push_back (value);
    }
    for (size_t i=0; i<offsets.size (); ++i)
    {
        targets[i] += offsets[i];
    }
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    if (!PI_MOV (iD, axesString.c_str (), targets.data ()))
    {
        throw std::runtime_error ("MOV failed");
    }

}

void PrepareDataRecorderForPositionAndTargetGCS20 (int iD, const std::string& axis)
{
    // configure record tables for target and position
    const int AXIS_TARGET_POSITION = 1;
    const int AXIS_CURRENT_POSITION = 2;
    std::vector<int> tables { 1,2 };
    std::string sources = axis + " " + axis;
    std::vector<int> options { AXIS_TARGET_POSITION, AXIS_CURRENT_POSITION };
    if (!PI_DRC (iD, tables.data (), sources.c_str (), options.data ()))
    {
        throw std::runtime_error ("DRC failed");
    }

    // configure datarecorder trigger to start with motion
    const int ANY_COMMAND_CHANGING_TARGET_POSITION = 1;
    const int DUMMY_CHANNEL_FOR_ALL_CHANNELS = 0;
    if (!PI_DRT(iD, &DUMMY_CHANNEL_FOR_ALL_CHANNELS, &ANY_COMMAND_CHANGING_TARGET_POSITION, "0", 1))
    {
        throw std::runtime_error ("DRT failed");
    }

    // configure record rate
    if (!PI_RTR (iD, 1))
    {
        throw std::runtime_error ("RTR failed");
    }
}

void WaitForDataRecorderDataGCS20 (int iD, int nrData)
{
    int nrDataRecorded = 0;
    while (nrDataRecorded < nrData)
    {
        int channel = 1;
        if (!PI_qDRL(iD, &channel, &nrDataRecorded, 1))
        {
            throw std::runtime_error ("qDRL failed");
        }
    }
}

std::vector<double> ReadDataRecorderDataGCS20 (int iD, int nrData)
{
    double* dataStorage;
    char header[2048];
    std::vector<int> tables { 1,2 };
    if (!PI_qDRR (iD, tables.data (), 2, 1, nrData, &dataStorage, header, 2047))
    {
        throw std::runtime_error ("qDRR failed");
    }
    int nrDataRead = 0;
    while (nrDataRead < nrData * static_cast<int>(tables.size ()))
    {
        nrDataRead = PI_GetAsyncBufferIndex (iD);
        if (nrDataRead < 0)
        {
            throw std::runtime_error ("PI_GetAsyncBufferIndex failed");
        }
    }
    return std::vector<double> (dataStorage, dataStorage + (nrData * tables.size ()));
}

void PrepareDataRecorderForPositionAndTargetGCS30 (int iD, const std::string& dataRecorderId, const std::string& axis)
{
    // configure data recorder traces for target and position
    const std::string AXIS_TARGET_POSITION = "0x102";
    const std::string AXIS_CURRENT_POSITION = "0x103";
    if (!PI_REC_TRACE (iD, dataRecorderId.c_str(), 1, axis.c_str (), "-", AXIS_TARGET_POSITION.c_str ()))
    {
        throw std::runtime_error ("REC_TRACE failed");
    }
    if (!PI_REC_TRACE (iD, dataRecorderId.c_str(), 2, axis.c_str (), "-", AXIS_CURRENT_POSITION.c_str ()))
    {
        throw std::runtime_error ("REC_TRACE failed");
    }

    // configure datarecorder trigger to start with motion
    if (!PI_REC_TRG (iD, dataRecorderId.c_str (), "MOV", axis.c_str (), "0"))
    {
        throw std::runtime_error ("REC_TRG failed");
    }

    // configure record rate
    if (!PI_REC_RATE (iD, dataRecorderId.c_str (), 1))
    {
        throw std::runtime_error ("REC_RATE failed");
    }

    // start data recorder
    if (!PI_REC_START (iD, dataRecorderId.c_str ()))
    {
        throw std::runtime_error ("REC_START failed");
    }
}

void WaitForDataRecorderGCS30 (int iD, const std::string& dataRecorderId, int nrData)
{
    int nrDataRecorded = 0;
    while (nrDataRecorded < nrData)
    {
        if (!PI_qREC_NUM (iD, dataRecorderId.c_str (), &nrDataRecorded))
        {
            throw std::runtime_error ("qREC_NUM failed");
        }
    }

}

std::vector<double> ReadDataRecorderDataGCS30 (int iD, const std::string& dataRecorderId, int nrData)
{
    double* dataStorage;
    char header[2048];
    std::vector<int> tables { 1,2 };
    if (!PI_qREC_DAT (iD, dataRecorderId.c_str (), "ASCII", 1, nrData, tables.data (), 2, &dataStorage, header, 2047))
    {
        throw std::runtime_error ("qREC_DAT failed");
    }
    int nrDataRead = 0;
    while (nrDataRead < nrData * static_cast<int>(tables.size ()))
    {
        nrDataRead = PI_GetAsyncBufferIndex (iD);
        if (nrDataRead < 0)
        {
            throw std::runtime_error ("PI_GetAsyncBufferIndex failed");
        }
    }
    return std::vector<double> (dataStorage, dataStorage + (nrData * tables.size ()));
}

unsigned int CheckAxisStatusValueGCS30(int id, std::string axis, unsigned int startBit, unsigned int bitMask)
{
    unsigned int status = 0;
    if (!PI_qSTV(id, axis.c_str(), &status))
    {
        throw std::runtime_error("qSTV failed");
    }

    return (status >> startBit) & bitMask;
}

void ResetAxesErrorGCS30 (int id, const std::vector<std::string>& axes)
{
    for (const auto& axis : axes)
    {
        const unsigned int START_BIT_DSM = 0;    // Start bit drive state machine
        const unsigned int DSM_ERROR_BIT = 1;
        if (CheckAxisStatusValueGCS30 (id, axis, START_BIT_DSM, DSM_ERROR_BIT) == DSM_ERROR_BIT)
        {
            const unsigned int DSM_BIT_MASK = 0xFF;
            const unsigned int DSM_FAULT_REACTION_BITS = 3;
            while (CheckAxisStatusValueGCS30(id, axis, START_BIT_DSM, DSM_BIT_MASK) == DSM_FAULT_REACTION_BITS)
            {}
            std::cout << "reset axis error: " << axis << std::endl;
            if (!PI_RES (id, axis.c_str ()))
            {
                throw std::runtime_error ("RES failed");
            }
        }
    }
}

void RecoverFromErrorGCS30 (int iD, const std::vector<std::string>& axes)
{
    ResetAxesErrorGCS30 (iD, axes);
    PrepareForClosedLoopPositioningGCS30 (iD, axes);
}

void ChangeVelocitiesGCS20 (int iD, const std::vector<std::string>& axes)
{
    auto velocitiesGCS20 = GetMaxVelocitiesGCS20 (iD, axes);
    std::transform (velocitiesGCS20.begin (), velocitiesGCS20.end (), velocitiesGCS20.begin (),
                    [] (double v) { return v * 0.5; });
    SetVelocitiesGCS20 (iD, axes, velocitiesGCS20);
}

std::vector<double> GetMaxVelocitiesGCS20 (int iD, const std::vector<std::string>& axes)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    std::vector<double> maxVelocities (axes.size());
    const unsigned int PARAMETER_MAX_VELOCITY = 10;
    std::vector<unsigned int> parameterIDs (axes.size (), PARAMETER_MAX_VELOCITY);
    if (!PI_qSPA (iD, axesString.c_str (), parameterIDs.data(), maxVelocities.data (), nullptr, 0))
    {
        throw std::runtime_error ("qSPA failed");
    }
    return maxVelocities;
}

void SetVelocitiesGCS20 (int iD, const std::vector<std::string>& axes, const std::vector<double>& velocities)
{
    std::string axesString;
    std::for_each (axes.begin (), axes.end (),
                   [&axesString] (const std::string& axis)
                   { axesString += axis + " "; });
    if (!PI_VEL (iD, axesString.c_str (), velocities.data ()))
    {
        throw std::runtime_error ("VEL failed");
    }
}

void ChangeVelocitiesGCS30 (int iD, const std::vector<std::string>& axes)
{
    auto velocitiesGCS30 = GetMaxVelocitiesGCS30 (iD, axes);
    std::transform (velocitiesGCS30.begin (), velocitiesGCS30.end (), velocitiesGCS30.begin (),
                    [] (double v) { return v * 0.5; });
    SetVelocitiesGCS30 (iD, axes, velocitiesGCS30);
}

std::vector<double> GetMaxVelocitiesGCS30 (int iD, const std::vector<std::string>& axes)
{
    std::vector<double> maxVelocities;
    for (const auto& axis : axes)
    {
        double value;
        if (!PI_qSPV_Double (iD, "RAM", axis.c_str (), "TRAJ_1", "0x105", &value))
        {
            throw std::runtime_error ("qSPV_Double failed");
        }
        maxVelocities.push_back (value);
    }
    return maxVelocities;
}

void SetVelocitiesGCS30 (int iD, const std::vector<std::string>& axes, const std::vector<double>& velocities)
{
    for (size_t idx = 0; idx < axes.size(); ++idx)
    {
        if (!PI_SPV_Double (iD, "RAM", axes[idx].c_str (), "TRAJ_1", "0x104", velocities[idx]))
        {
            throw std::runtime_error ("SPV_Double failed");
        }
    }
}
