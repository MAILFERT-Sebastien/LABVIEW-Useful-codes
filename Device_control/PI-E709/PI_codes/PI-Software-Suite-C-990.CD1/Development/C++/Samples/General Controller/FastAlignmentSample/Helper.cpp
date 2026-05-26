#include "Helper.h"
#include "GCSMethods.h"

// ----------------------------------------------------------------------------------------------------------
std::vector <std::string>  ConvertStringToVector (const std::string& InputString, char Delimiter)
{
    std::vector <std::string> l_Vector;
    std::istringstream iss (InputString);
    std::string token;
    while (std::getline (iss, token, Delimiter))
    {
        l_Vector.push_back (token);
    }
    return l_Vector;
}

// ----------------------------------------------------------------------------------------------------------
void ConvertStringScansToFDRScans (const std::map <std::string, StringScanMap>& IniScanMaps, FDRScans& FDRScans)
{
    int l_Count = 0;
    std::map <std::string, StringScanMap>::const_iterator l_ItSystemScan;
    for (l_ItSystemScan = IniScanMaps.begin (); l_ItSystemScan != IniScanMaps.end (); ++l_ItSystemScan)
    {
        StringScanMap::const_iterator l_ItScans;
        for (l_ItScans = l_ItSystemScan->second.begin (); l_ItScans != l_ItSystemScan->second.end (); ++l_ItScans)
        {
            std::string l_SystemIdentifier = l_ItSystemScan->first;
            std::string l_Scan = l_ItScans->second;

            const std::vector <std::string> l_ScanVector = ConvertStringToVector (l_Scan, ' ');

            FDRScan l_FDRScan;
            if (ConvertStringScanToFDRScan (l_ScanVector, l_FDRScan))
            {
                FDRScans[l_SystemIdentifier].insert (std::make_pair (l_Count, l_FDRScan));
                l_Count++;
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void ConvertStringScansToFDGScans (const std::map <std::string, StringScanMap>& IniScanMaps, FDGScans& FDGScans)
{
    int l_Count = 0;
    std::map <std::string, StringScanMap>::const_iterator l_ItSystemScan;
    for (l_ItSystemScan = IniScanMaps.begin (); l_ItSystemScan != IniScanMaps.end (); ++l_ItSystemScan)
    {
        StringScanMap::const_iterator l_ItScans;
        for (l_ItScans = l_ItSystemScan->second.begin (); l_ItScans != l_ItSystemScan->second.end (); ++l_ItScans)
        {
            std::string l_SystemIdentifier = l_ItSystemScan->first;
            std::string l_Scan = l_ItScans->second;

            const std::vector <std::string> l_ScanVector = ConvertStringToVector (l_Scan, ' ');

            FDGScan l_FDGScan;
            if (ConvertStringScanToFDGScan (l_ScanVector, l_FDGScan))
            {
                FDGScans[l_SystemIdentifier].insert (std::make_pair (l_Count, l_FDGScan));
                l_Count++;
            }
        }
    }
}

void ConvertStringScansToFDLScans(const std::map <std::string, StringScanMap>& IniScanMaps, FDLScans& FDLScans)
{
    int l_Count = 0;
    std::map <std::string, StringScanMap>::const_iterator l_ItSystemScan;
    for (l_ItSystemScan = IniScanMaps.begin(); l_ItSystemScan != IniScanMaps.end(); ++l_ItSystemScan)
    {
        StringScanMap::const_iterator l_ItScans;
        for (l_ItScans = l_ItSystemScan->second.begin(); l_ItScans != l_ItSystemScan->second.end(); ++l_ItScans)
        {
            std::string l_SystemIdentifier = l_ItSystemScan->first;
            std::string l_Scan = l_ItScans->second;

            std::vector <std::string> l_ScanVector = ConvertStringToVector(l_Scan, ' ');

            FDLScan l_FDLScan;
            if (ConvertStringScanToFDLScan(l_ScanVector, l_FDLScan))
            {
                FDLScans[l_SystemIdentifier].insert(std::make_pair(l_Count, l_FDLScan));
                l_Count++;
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConvertStringScanToFDRScan (const std::vector <std::string>& Scan, FDRScan& FDRScan)
{
    size_t ParameterPos = 0;
    try {
        std::string l_ScanParameter = Scan[ParameterPos++];
        if ("FDR" == Scan[0])
        {
            while (ParameterPos < Scan.size())
            {
                //FDR Parser
                if (l_ScanParameter == "FDR")
                {
                    FDRScan.ScanRoutineName = Scan[ParameterPos++];
                    FDRScan.ScanAxis = Scan[ParameterPos++];
                    FDRScan.ScanAxisRange = std::stod(Scan[ParameterPos++]);
                    FDRScan.StepAxis = Scan[ParameterPos++];
                    FDRScan.StepAxisRange = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "L")
                {
                    FDRScan.ThresholdLevel = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "A")
                {
                    FDRScan.InputChannel = std::stoi(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "F")
                {
                    FDRScan.Frequency = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "V")
                {
                    FDRScan.Velocity = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MP1")
                {
                    FDRScan.ScanAxisMidPos = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MP2")
                {
                    FDRScan.StepAxisMidPos = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "TT")
                {
                    FDRScan.TargetType = std::stoi(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "CM")
                {
                    FDRScan.CentroidMethod = std::stoi(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MIIL")
                {
                    FDRScan.MinRelInputLevel = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MAIL")
                {
                    FDRScan.MaxRelInputLevel = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "ST")
                {
                    FDRScan.StopPosition = std::stoi(Scan[ParameterPos++]);
                }

                if (ParameterPos < Scan.size())
                {
                    l_ScanParameter = Scan[ParameterPos++];
                }
            }
            return TRUE;
        }
        return FALSE;
    } 
    catch (std::exception ex)
    {
        return FALSE;
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConvertStringScanToFDGScan (const std::vector <std::string>& Scan, FDGScan& FDGScan)
{
    size_t ParameterPos = 0;
    try {
        std::string l_ScanParameter = Scan[ParameterPos++];
        if ("FDG" == Scan[0])
        {
            while (ParameterPos < Scan.size ())
            {
                //FDG Parser
                if (l_ScanParameter == "FDG")
                {
                    FDGScan.ScanRoutineName = Scan[ParameterPos++];
                    FDGScan.ScanAxis = Scan[ParameterPos++];
                    FDGScan.StepAxis = Scan[ParameterPos++];
                }
                else if (l_ScanParameter == "ML")
                {
                    FDGScan.MinimumLevel = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "A")
                {
                    FDGScan.InputChannel = std::stoi (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "F")
                {
                    FDGScan.Frequency = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "V")
                {
                    FDGScan.Velocity = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MIA")
                {
                    FDGScan.MinAmplitude = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MAA")
                {
                    FDGScan.MaxAmplitude = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "SP")
                {
                    FDGScan.SpeedFactor = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "SPO")
                {
                    FDGScan.SpeedOffset = std::stod (Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MDC")
                {
                    FDGScan.MaxDirectionChanges = std::stoi (Scan[ParameterPos++]);
                }


                if (ParameterPos < Scan.size ())
                {
                    l_ScanParameter = Scan[ParameterPos++];
                }
            }
            return TRUE;
        }
        return FALSE;
    }
    catch (std::exception ex)
    {
        return FALSE;
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConvertStringScanToFDLScan(const std::vector <std::string>& Scan, FDLScan& FDLScan)
{
    size_t ParameterPos = 0;
    try {
        std::string l_ScanParameter = Scan[ParameterPos++];
        if ("FDL" == Scan[0])
        {
            while (ParameterPos < Scan.size())
            {
                //FDL Parser
                if (l_ScanParameter == "FDL")
                {
                    FDLScan.ScanRoutineName = Scan[ParameterPos++];
                    FDLScan.ScanAxis = Scan[ParameterPos++];
                    FDLScan.ScanAxisRange = std::stod(Scan[ParameterPos++]);
                    FDLScan.StepAxis = Scan[ParameterPos++];
                    FDLScan.StepAxisRange = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "S")
                {
                    FDLScan.ThresholdSigma = std::stoi(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "C")
                {
                    FDLScan.ThresholdCalculation = StdStringToBool(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "A")
                {
                    FDLScan.InputChannel = std::stoi(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MT")
                {
                    FDLScan.MaximumSearchTime = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MP1")
                {
                    FDLScan.ScanAxisMidPos = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "MP2")
                {
                    FDLScan.StepAxisMidPos = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "CP1")
                {
                    FDLScan.ScanAxisCalcPos = std::stod(Scan[ParameterPos++]);
                }
                else if (l_ScanParameter == "CP2")
                {
                    FDLScan.StepAxisCalcPos = std::stod(Scan[ParameterPos++]);
                }

                if (ParameterPos < Scan.size())
                {
                    l_ScanParameter = Scan[ParameterPos++];
                }
            }
            return TRUE;
        }
        return FALSE;
    }
    catch (std::exception ex)
    {
        return FALSE;
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL GetIniFile (const char* FileName, IniConfs& Conf, StringScans& Scans)
{
    char ExePath[MAX_PATH];
    HMODULE hModule = GetModuleHandle (NULL);
    std::string sExePath;
    if (hModule != NULL)
    {
        GetModuleFileNameA (hModule, ExePath, (sizeof (ExePath)));
        sExePath = ExePath;
        std::size_t PathEnd = sExePath.find_last_of ("/\\");
        sExePath = sExePath.substr (0, PathEnd + 1);
    }

    std::string FilePath = sExePath + "..\\AppData\\" + FileName;

    std::map <int, std::string> l_Keys;

    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eSystemName, "SystemName"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eCommunicationType, "CommunicationType"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eComPortBaudrate, "ComPortBaudrate"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eComPortNr, "ComPortNr"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eIPAdress, "IPAdress"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eIPPort, "IPPort"));

    //Get Config-File
    INIReader reader (FilePath);

    if (reader.ParseError () < 0)
    {
        return FALSE;
    }

    IniConfMap l_IniConfMap;
    StringScanMap l_IniScanMap;

    for (unsigned int l_Count = 1; l_Count < static_cast <unsigned int>(l_Keys.size () + 1); l_Count++)
    {
        l_IniConfMap.insert (std::pair <int, std::string> (l_Count, ""));
    }

    //Get Systems
    BOOL ConfigHasMoreSytems = TRUE;
    int SystemCounter = 1;
    while (ConfigHasMoreSytems)
    {

        std::string l_CurrentSystem = "System" + ConvertToString (SystemCounter);
        std::string l_Val = reader.Get (l_CurrentSystem, l_Keys[ConfKeys::eSystemName], "");
        if (0 < l_Val.size ())
        {
            Conf.insert (std::pair <std::string, IniConfMap> (l_CurrentSystem, l_IniConfMap));
            Scans.insert (std::pair <std::string, StringScanMap> (l_CurrentSystem, l_IniScanMap));
            SystemCounter++;
        }
        else
        {
            ConfigHasMoreSytems = FALSE;
        }
    }

    //Get System-Values
    int KeyCounter = 1;
    std::map <std::string, IniConfMap>::iterator l_ItSystems;
    for (l_ItSystems = Conf.begin (); l_ItSystems != Conf.end (); ++l_ItSystems)
    {
        std::string l_CurrentSystem = l_ItSystems->first;
        KeyCounter = 1;
        std::map <int, std::string>::iterator l_ItSystem;
        for (l_ItSystem = l_ItSystems->second.begin (); l_ItSystem != l_ItSystems->second.end (); ++l_ItSystem)
        {
            l_ItSystem->second = reader.Get (l_CurrentSystem, l_Keys[KeyCounter], "");
            KeyCounter++;
        }
    }

    //Get System-Scans
    BOOL ConfigHasMoreScans = TRUE;
    int ScanCounter = 1;
    std::map <std::string, StringScanMap>::iterator l_ItIniScanMap;
    for (l_ItIniScanMap = Scans.begin (); l_ItIniScanMap != Scans.end (); ++l_ItIniScanMap)
    {
        ConfigHasMoreScans = TRUE;
        ScanCounter = 1;
        while (ConfigHasMoreScans)
        {
            std::string l_CurrentSystem = l_ItIniScanMap->first;
            std::string l_CurrentScan = "ScanRoutine" + ConvertToString (ScanCounter);
            std::string l_Val = reader.Get (l_CurrentSystem, l_CurrentScan, "");
            if (0 < l_Val.size ())
            {
                l_ItIniScanMap->second.insert (std::pair <std::string, std::string> (l_CurrentScan, l_Val));
                ScanCounter++;
            }
            else
            {
                ConfigHasMoreScans = FALSE;
            }
        }
    }
    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
std::vector <std::string> StdStringToVector (const std::string& InputString, char Delimiter)
{
    std::vector <std::string> l_Vector;
    std::istringstream iss (InputString);
    std::string token;
    while (std::getline (iss, token, Delimiter))
    {
        l_Vector.push_back (token);
    }
    return l_Vector;
}

// ----------------------------------------------------------------------------------------------------------
void MakeFDRParameters (const FDRScan& Scan, std::string& Parameters)
{
    Parameters = "L " + ConvertToString (Scan.ThresholdLevel)
        + " A " + ConvertToString (Scan.InputChannel)
        + " F " + ConvertToString (Scan.Frequency)
        + " V " + ConvertToString (Scan.Velocity)
        + " MP1 " + ConvertToString (Scan.ScanAxisMidPos)
        + " MP2 " + ConvertToString (Scan.StepAxisMidPos)
        + " TT " + ConvertToString (Scan.TargetType)
        + " CM " + ConvertToString (Scan.CentroidMethod)
        + " MIIL " + ConvertToString (Scan.MinRelInputLevel)
        + " MAIL " + ConvertToString (Scan.MaxRelInputLevel)
        + " ST " + ConvertToString (Scan.StopPosition);
}

// ----------------------------------------------------------------------------------------------------------
void SetControllerFDRScans (const ConnectMap& Connections, FDRScans& FDRScans)
{
    std::vector <int> l_IDs;
    ConnectMap::const_iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        l_IDs.push_back (l_It->first);
    }

    unsigned int l_Count = 0;
    FDRScans::const_iterator l_ItFDR;
    for (l_ItFDR = FDRScans.begin (); l_ItFDR != FDRScans.end (); ++l_ItFDR)
    {
        //nur ge�ffnete verbindungen bedienen
        if (l_Count < static_cast<unsigned int> (l_IDs.size ()))
        {
            FDRMap::const_iterator l_ItSingleScan;
            for (l_ItSingleScan = l_ItFDR->second.begin (); l_ItSingleScan != l_ItFDR->second.end (); ++l_ItSingleScan)
            {
                std::string l_Parameters;
                MakeFDRParameters (l_ItSingleScan->second, l_Parameters);
                FADefineAreaScanRoutine (l_IDs[l_Count]
                    , l_ItSingleScan->second.ScanRoutineName.c_str ()
                    , (char*)l_ItSingleScan->second.ScanAxis.c_str ()
                    , l_ItSingleScan->second.ScanAxisRange
                    , l_ItSingleScan->second.StepAxis.c_str ()
                    , l_ItSingleScan->second.StepAxisRange
                    , l_Parameters.c_str ()
                );
            }
            l_Count++;
        }
    }
}


// ----------------------------------------------------------------------------------------------------------
void MakeFDGParameters (const FDGScan& Scan, std::string& Parameters)
{
    Parameters = "ML " + ConvertToString (Scan.MinimumLevel)
        + " A " + ConvertToString (Scan.InputChannel)
        + " MIA " + ConvertToString (Scan.MinAmplitude)
        + " MAA " + ConvertToString (Scan.MaxAmplitude)
        + " F " + ConvertToString (Scan.Frequency)
        + " SP " + ConvertToString (Scan.SpeedFactor)
        + " V " + ConvertToString (Scan.Velocity)
        + " MDC " + ConvertToString (Scan.MaxDirectionChanges)
        + " SPO " + ConvertToString (Scan.SpeedOffset);
}

// ----------------------------------------------------------------------------------------------------------
void SetControllerFDGScans (const ConnectMap& Connections, FDGScans& FDGScans)
{
    std::vector <int> l_IDs;
    ConnectMap::const_iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        l_IDs.push_back (l_It->first);
    }

    unsigned int l_Count = 0;
    FDGScans::const_iterator l_ItFDG;
    for (l_ItFDG = FDGScans.begin (); l_ItFDG != FDGScans.end (); ++l_ItFDG)
    {
        //nur ge�ffnete verbindungen bedienen
        if (l_Count < static_cast<unsigned int>(l_IDs.size ()))
        {
            FDGMap::const_iterator l_ItSingleScan;
            for (l_ItSingleScan = l_ItFDG->second.begin (); l_ItSingleScan != l_ItFDG->second.end (); ++l_ItSingleScan)
            {
                std::string l_Parameters;
                MakeFDGParameters (l_ItSingleScan->second, l_Parameters);

                FADefineGradientScanRoutine (l_IDs[l_Count]
                    , l_ItSingleScan->second.ScanRoutineName.c_str ()
                    , (char*)l_ItSingleScan->second.ScanAxis.c_str ()
                    , l_ItSingleScan->second.StepAxis.c_str ()
                    , l_Parameters.c_str ()
                );
            }
            l_Count++;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void MakeFDLParameters(const FDLScan& Scan, std::string& Parameters)
{
    Parameters = " S " + ConvertToString(Scan.ThresholdSigma)
        + " C " + ConvertToString(Scan.ThresholdCalculation)
        + " A " + ConvertToString(Scan.InputChannel)
        + " MT " + ConvertToString(Scan.MaximumSearchTime)
        + " MP1 " + ConvertToString(Scan.ScanAxisMidPos)
        + " MP2 " + ConvertToString(Scan.StepAxisMidPos)
        + " CP1 " + ConvertToString(Scan.ScanAxisCalcPos)
        + " CP1 " + ConvertToString(Scan.StepAxisCalcPos);
}

// ----------------------------------------------------------------------------------------------------------
void SetControllerFDLScans(const ConnectMap& Connections, FDLScans& FDLScans)
{
    std::vector <int> l_IDs;
    ConnectMap::const_iterator l_It;
    for (l_It = Connections.begin(); l_It != Connections.end(); ++l_It)
    {
        l_IDs.push_back(l_It->first);
    }

    unsigned int l_Count = 0;
    FDLScans::iterator l_ItFDL;
    for (l_ItFDL = FDLScans.begin(); l_ItFDL != FDLScans.end(); ++l_ItFDL)
    {
        //nur ge�ffnete verbindungen bedienen
        if (l_Count < static_cast<unsigned int> (l_IDs.size()))
        {
            FDLMap::iterator l_ItSingleScan;
            for (l_ItSingleScan = l_ItFDL->second.begin(); l_ItSingleScan != l_ItFDL->second.end(); ++l_ItSingleScan)
            {
                std::string l_Parameters;
                MakeFDLParameters(l_ItSingleScan->second, l_Parameters);
                FADefineLightningScanRoutine(l_IDs[l_Count]
                    , l_ItSingleScan->second.ScanRoutineName.c_str()
                    , (char*)l_ItSingleScan->second.ScanAxis.c_str()
                    , l_ItSingleScan->second.ScanAxisRange
                    , l_ItSingleScan->second.StepAxis.c_str()
                    , l_ItSingleScan->second.StepAxisRange
                    , l_Parameters.c_str()
                );
            }
            l_Count++;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerScanNames (
    std::map<int,ScanNameMap>& ControllerScanNames,
    const ConnectMap& Connections,
    ScanType::EScanType ScanType)
{
    const int c_READ_TIME_MAX = 25;
    const int c_BUFFER_SIZE_MAX = 4096;

    ConnectMap::const_iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        int l_ID = l_It->first;
        if (!PI_GcsCommandset (l_ID, "FRR?"))
        {
            return;
        }

        char l_Buffer[c_BUFFER_SIZE_MAX] = { 0 };
        std::string l_Answer;
        int i_AnswerSize = 1;
        while (0 < i_AnswerSize)
        {
            (void)PI_GcsGetAnswer (l_ID, l_Buffer, c_BUFFER_SIZE_MAX);
            l_Answer.append (l_Buffer);
            memset (l_Buffer, 0, c_BUFFER_SIZE_MAX - 1);
            Sleep (c_READ_TIME_MAX);
            (void)PI_GcsGetAnswerSize (l_ID, &i_AnswerSize);
        }

        //get scan names
        std::string l_OldToken;
        std::vector <std::string> l_ScanVector = StdStringToVector (l_Answer.c_str (), '\n');

        int l_Count = 0;
        std::vector <std::string>::const_iterator l_it;
        for (l_it = l_ScanVector.begin (); l_it != l_ScanVector.end (); ++l_it)
        {
            std::istringstream iss (l_it->c_str ());
            std::string token;
            std::getline (iss, token, ' ');

            if (0 < token.size () && (l_OldToken != token))
            {
                l_OldToken = token;
                int FindFDR = l_ScanVector[l_Count + 3].find ("FDR");
                int FindFDG = l_ScanVector[l_Count + 3].find ("FDG");
                int FindFDL = l_ScanVector[l_Count + 3].find ("FDL");

                if ((0 < FindFDR) && ScanType == 1)
                {
                    ControllerScanNames[l_ID].push_back (token);
                }
                if ((0 < FindFDG) && ScanType == 2)
                {
                    ControllerScanNames[l_ID].push_back (token);
                }
                if ((0 < FindFDL) && ScanType == 3)
                {
                    ControllerScanNames[l_ID].push_back(token);
                }
            }
            l_Count++;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerFDRScans (const ConnectMap& Connections, FDRScans& FDRScans)
{
    std::map <int, ScanNameMap> l_ControllerScanNames;
    GetControllerScanNames (l_ControllerScanNames, Connections, ScanType::eFDR);

    std::map <int, ScanNameMap>::const_iterator l_It;
    for (l_It = l_ControllerScanNames.begin (); l_It != l_ControllerScanNames.end (); ++l_It)
    {
        int ID = l_It->first;
        StringScanMap l_ScanMap;
        FDRMap l_FDRMap;
        ScanNameMap::const_iterator l_ItScanName;
        int l_Count = 0;
        for (l_ItScanName = l_It->second.begin (); l_ItScanName != l_It->second.end (); ++l_ItScanName)
        {
            std::string ScanRoutineName = *l_ItScanName;
            const unsigned int ResultId = 4;
            char Result[2047] = { 0 };
            int BufferSize = 2047 - 1;

            FAGetRoutineResults (ID
                , ScanRoutineName.c_str ()
                , ResultId
                , Result
                , BufferSize
            );

            std::string l_SingleScan (Result);
            int l_Pos = l_SingleScan.find ("=");
            l_SingleScan = l_SingleScan.substr (l_Pos + 1);
            l_SingleScan.erase (0, l_SingleScan.find_first_not_of (" "));

            FDRScan l_FDRScan;
            ConvertStringScanToFDRScan (ConvertStringToVector (l_SingleScan, ' '), l_FDRScan);
            l_FDRMap.insert (std::pair <int, FDRScan> (l_Count, l_FDRScan));
            l_Count++;
        }
        std::cout << ConvertToString(l_Count) + " FDR Scan(s) on Controller." << "\n";
        FDRScans.insert (std::pair <std::string, FDRMap> (ConvertToString (ID), l_FDRMap));
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerFDGScans (const ConnectMap& Connections, FDGScans& FDGScans)
{
    std::map <int, ScanNameMap> l_ControllerScanNames;
    GetControllerScanNames (l_ControllerScanNames, Connections, ScanType::eFDG);

    std::map <int, ScanNameMap>::const_iterator l_It;
    for (l_It = l_ControllerScanNames.begin (); l_It != l_ControllerScanNames.end (); ++l_It)
    {
        int ID = l_It->first;
        StringScanMap l_ScanMap;
        FDGMap l_FDGMap;
        ScanNameMap::const_iterator l_ItScanName;
        int l_Count = 0;
        for (l_ItScanName = l_It->second.begin (); l_ItScanName != l_It->second.end (); ++l_ItScanName)
        {
            std::string ScanRoutineName = *l_ItScanName;
            const unsigned int ResultId = 4;
            char Result[2047] = { 0 };
            int BufferSize = 2047 - 1;

            FAGetRoutineResults (ID
                , ScanRoutineName.c_str ()
                , ResultId
                , Result
                , BufferSize
            );

            std::string l_SingleScan (Result);
            int l_Pos = l_SingleScan.find ("=");
            l_SingleScan = l_SingleScan.substr (l_Pos + 1);
            l_SingleScan.erase (0, l_SingleScan.find_first_not_of (" "));

            FDGScan l_FDGScan;
            ConvertStringScanToFDGScan (ConvertStringToVector (l_SingleScan, ' '), l_FDGScan);
            l_FDGMap.insert (std::pair <int, FDGScan> (l_Count, l_FDGScan));
            l_Count++;
        }
        std::cout << ConvertToString(l_Count) + " FDG Scan(s) on Controller." << "\n";
        FDGScans.insert (std::pair <std::string, FDGMap> (ConvertToString (ID), l_FDGMap));
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerFDLScans(const ConnectMap& Connections, FDLScans& FDLScans)
{
    std::map <int, ScanNameMap> l_ControllerScanNames;
    GetControllerScanNames(l_ControllerScanNames, Connections, ScanType::eFDL);

    std::map <int, ScanNameMap>::const_iterator l_It;
    for (l_It = l_ControllerScanNames.begin(); l_It != l_ControllerScanNames.end(); ++l_It)
    {
        int ID = l_It->first;
        StringScanMap l_ScanMap;
        FDLMap l_FDLMap;
        ScanNameMap::const_iterator l_ItScanName;
        int l_Count = 0;
        for (l_ItScanName = l_It->second.begin(); l_ItScanName != l_It->second.end(); ++l_ItScanName)
        {
            std::string ScanRoutineName = *l_ItScanName;
            const unsigned int ResultId = 4;
            char Result[2047] = { 0 };
            int BufferSize = 2047 - 1;

            FAGetRoutineResults(ID
                , ScanRoutineName.c_str()
                , ResultId
                , Result
                , BufferSize
            );

            std::string l_SingleScan(Result);
            int l_Pos = l_SingleScan.find("=");
            l_SingleScan = l_SingleScan.substr(l_Pos + 1);
            l_SingleScan.erase(0, l_SingleScan.find_first_not_of(" "));

            FDLScan l_FDLScan;
            ConvertStringScanToFDLScan(ConvertStringToVector(l_SingleScan, ' '), l_FDLScan);
            l_FDLMap.insert(std::pair <int, FDLScan>(l_Count, l_FDLScan));
            l_Count++;
        }
        std::cout << ConvertToString(l_Count) + " FDL Scan(s) on Controller." << "\n";
        FDLScans.insert(std::pair <std::string, FDLMap>(ConvertToString(ID), l_FDLMap));
    }
}

// ----------------------------------------------------------------------------------------------------------
std::string ConvertFDRScanToString (const FDRScan& FDRScan)
{
    std::string Parameters;
    MakeFDRParameters (FDRScan, Parameters);
    std::string ScanString;

    ScanString = "FDR " + ConvertToString (FDRScan.ScanRoutineName)
        + " " + ConvertToString (FDRScan.ScanAxis)
        + " " + ConvertToString (FDRScan.ScanAxisRange)
        + " " + ConvertToString (FDRScan.StepAxis)
        + " " + ConvertToString (FDRScan.StepAxisRange)
        + " " + Parameters;

    return ScanString;
}

// ----------------------------------------------------------------------------------------------------------
std::string ConvertFDGScanToString (const FDGScan& FDGScan)
{
    std::string Parameters;
    MakeFDGParameters (FDGScan, Parameters);
    std::string ScanString;

    ScanString = "FDG " + ConvertToString (FDGScan.ScanRoutineName)
        + " " + ConvertToString (FDGScan.ScanAxis)
        + " " + ConvertToString (FDGScan.StepAxis)
        + " " + Parameters;
    return ScanString;
}

// ----------------------------------------------------------------------------------------------------------
std::string ConvertFDLScanToString(const FDLScan& FDLScan)
{
    std::string Parameters;
    MakeFDLParameters(FDLScan, Parameters);
    std::string ScanString;

    ScanString = "FDL " + ConvertToString(FDLScan.ScanRoutineName)
        + " " + ConvertToString(FDLScan.ScanAxis)
        + " " + ConvertToString(FDLScan.ScanAxisRange)
        + " " + ConvertToString(FDLScan.StepAxis)
        + " " + ConvertToString(FDLScan.StepAxisRange)
        + " " + Parameters;

    return ScanString;
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (double& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        try
        {
            Value = std::stod (InputLine);
        }
        catch (const std::invalid_argument&)
        {
           ;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (std::string& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        Value = InputLine;
    }
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (unsigned int& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        try
        {
            Value = std::stoi (InputLine);
        }
        catch (const std::invalid_argument&)
        {
            ;
        }
    }
}

void PrintAndReadline (bool& Value)
{
    std::string InputLine;
    std::getline(std::cin, InputLine);

    if (!InputLine.empty())
    {
        try
        {
            Value = StdStringToBool(InputLine);
        }
        catch (const std::invalid_argument&)
        {
            ;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
bool StdStringToBool(const std::string &s)
{
    const auto sInt = stoi(s);
    if (s.size() != 1
        || sInt < 0 || sInt > '1') throw std::invalid_argument(s + " has to be '1' or '0'");
    bool b = (sInt == 1);
    return b;
}


