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

#include <windows.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <string>

#include "PI_GCS2_DLL.h"


void PrintControllerIdentification (int id);
void ReportError (int iD);

void StartReferenceGCS20 (int iD, const std::vector<std::string>& axes);
void StartReferenceGCS30 (int iD, const std::vector<std::string>& axes);

void WaitWhileReferencing (int iD);

std::vector<double> GetMinPositionLimitsGCS20 (int ID, const std::vector<std::string>& axes);
std::vector<double> GetMinPositionLimitsGCS30 (int ID, const std::vector<std::string>& axes);
std::vector<double> GetMaxPositionLimitsGCS20 (int ID, const std::vector<std::string>& axes);
std::vector<double> GetMaxPositionLimitsGCS30 (int ID, const std::vector<std::string>& axes);

std::vector<double> GetCurrentPositionsGCS20 (int ID, const std::vector<std::string>& axes);
std::vector<double> GetCurrentPositionsGCS30 (int ID, const std::vector<std::string>& axes);

void PrepareForClosedLoopPositioningGCS20 (int iD, const std::vector<std::string>& axes);
void PrepareForClosedLoopPositioningGCS30 (int iD, const std::vector<std::string>& axes);

void MoveToTargetPosition (int iD, const std::vector<std::string>& axes, const std::vector<double>& targets);
bool WaitForMotion (int iD);

void MoveRelativeToTargetPositionGCS20 (int iD, const std::vector<std::string>& axes, const std::vector<double>& offsets);
void MoveRelativeToTargetPositionGCS30 (int iD, const std::vector<std::string>& axes, const std::vector<double>& offsets);

void PrepareDataRecorderForPositionAndTargetGCS20 (int iD, const std::string& axis);
void WaitForDataRecorderDataGCS20 (int iD, int nrData);
std::vector<double> ReadDataRecorderDataGCS20 (int iD, int nrData);

void PrepareDataRecorderForPositionAndTargetGCS30 (int iD, const std::string& dataRecorderId, const std::string& axis);
void WaitForDataRecorderGCS30 (int iD, const std::string& dataRecorderId, int nrData);
std::vector<double> ReadDataRecorderDataGCS30 (int iD, const std::string& dataRecorderId, int nrData);

unsigned int CheckAxisStatusValueGCS30(int id, std::string axis, unsigned int startBit, unsigned int bitMask);
void ResetAxesErrorGCS30 (int id, const std::vector<std::string>& axes);
void RecoverFromErrorGCS30 (int iD, const std::vector<std::string>& axes);

void ChangeVelocitiesGCS20 (int iD, const std::vector<std::string>& axes);
void ChangeVelocitiesGCS30 (int iD, const std::vector<std::string>& axes);

std::vector<double> GetMaxVelocitiesGCS20 (int ID, const std::vector<std::string>& axes);
std::vector<double> GetMaxVelocitiesGCS30 (int ID, const std::vector<std::string>& axes);
void SetVelocitiesGCS20 (int iD, const std::vector<std::string>& axes, const std::vector<double>& velocities);
void SetVelocitiesGCS30 (int iD, const std::vector<std::string>& axes, const std::vector<double>& velocities);
