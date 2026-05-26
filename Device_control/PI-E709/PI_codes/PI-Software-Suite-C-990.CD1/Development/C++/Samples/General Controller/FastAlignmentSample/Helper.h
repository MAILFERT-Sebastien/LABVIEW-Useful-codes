#pragma once

#include <windows.h>
#include "Declarations.h"
#include "INIReader.h"


//Configuration
BOOL GetIniFile (const char* FileName, IniConfs&, StringScans&);

//Connection
BOOL ControllerConnect (IniConfs Conf, int* pID, ConnectMap& Connections);

//Prepair
void ConvertStringScansToFDRScans (const std::map <std::string, StringScanMap>& StringScanMap, FDRScans& FDRScans);
void ConvertStringScansToFDGScans (const std::map <std::string, StringScanMap>& StringScanMap, FDGScans& FDGScans);
void ConvertStringScansToFDLScans(const std::map <std::string, StringScanMap>& StringScanMap, FDLScans& FDLScans);

BOOL ConvertStringScanToFDRScan (const std::vector <std::string>& Scan, FDRScan& FDRScan);
BOOL ConvertStringScanToFDGScan (const std::vector <std::string>& Scan, FDGScan& FDGScan);
BOOL ConvertStringScanToFDLScan (const std::vector <std::string>& Scan, FDLScan& FDLScan);

std::string ConvertFDRScanToString (const FDRScan& FDRScan);
std::string ConvertFDGScanToString (const FDGScan& FDGScan);
std::string ConvertFDLScanToString (const FDLScan& FDLScan);

void MakeFDGParameters (const FDGScan& Scan, std:: string& Parameters);
void MakeFDRParameters (const FDRScan& Scan, std:: string& Parameters);
void MakeFDLParameters (const FDLScan& Scan, std::string& Parameters);

//Informations
void GetControllerScanNames (
    std::map<int,ScanNameMap>& ControllerScanNames,
    const ConnectMap& Connections,
    ScanType::EScanType ScanType);

void WaitForAxisToStop (int ID, char * Axis, BOOL* Res, const int MaxLen);


//SCANS
void GetControllerFDRScans (const ConnectMap& Connections, FDRScans& FDRScans);
void GetControllerFDGScans (const ConnectMap& Connections, FDGScans& FDGScans);
void GetControllerFDLScans (const ConnectMap& Connections, FDLScans& FDLScans);

void SetControllerFDRScans (const ConnectMap& Connections, FDRScans& FDRScans);
void SetControllerFDGScans (const ConnectMap& Connections, FDGScans& FDGScans);
void SetControllerFDLScans (const ConnectMap& Connections, FDLScans& FDLScans);


//helper
template <typename T>
std::string ConvertToString(T const & value)
{
    std::stringstream l_ss;
    l_ss  << value;
    return l_ss.str();
}

template <typename T>
std::string ConvertToStringFixed(T const & value)
{
    std::stringstream l_ss;
    l_ss << std::fixed << value;
    return l_ss.str();
}

bool StdStringToBool (const std::string &string);
std::vector <std::string> StdStringToVector (const std::string& InputString, char Delimiter);
std::vector <std::string> ConvertStringToVector (const std::string& InputString, char Delimiter);

void PrintAndReadline (double& Value);
void PrintAndReadline (std::string& Value);
void PrintAndReadline (unsigned int& Value);
void PrintAndReadline (bool& Value);
