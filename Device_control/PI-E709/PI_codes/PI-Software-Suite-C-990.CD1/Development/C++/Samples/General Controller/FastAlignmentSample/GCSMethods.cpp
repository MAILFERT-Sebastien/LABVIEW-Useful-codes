#include "GCSMethods.h"
#include "Menu.h"

// ----------------------------------------------------------------------------------------------------------
std::string GetScanRoutineName (const FDRScan& FDR, const FDGScan& FDG, const FDLScan& FDL)
{
    std::string ScanRoutineName;
    if (FDR.ScanRoutineName.size ())
    {
        ScanRoutineName = FDR.ScanRoutineName;
    }
    else if (FDG.ScanRoutineName.size ())
    {
        ScanRoutineName = FDG.ScanRoutineName;
    }
    else if (FDL.ScanRoutineName.size())
    {
        ScanRoutineName = FDL.ScanRoutineName;
    }

    return ScanRoutineName;
}

// ----------------------------------------------------------------------------------------------------------
std::string GetRunningScanRoutineName (int ID, const FDRScan& FDR, const FDGScan& FDG, const FDLScan& FDL)
{
    std::string ScanRoutineName = GetScanRoutineName (FDR, FDG, FDL);
    int OptionsArray[1] = { 0 };
    const int RoutineIsRunning = 2;
    (void)FAGetRoutineState (ID, ScanRoutineName.c_str (), OptionsArray);

    if (RoutineIsRunning == OptionsArray[0])
    {
        return ScanRoutineName;
    }
    else
    {
        return "";
    }
}

// ----------------------------------------------------------------------------------------------------------
std::string GetRunningScanRoutineName (int ID, const std::vector <std::string>& ScanNames)
{
    std::string ReturnString;
    std::vector<std::string>::const_iterator l_It;
    for (l_It = ScanNames.begin (); l_It != ScanNames.end (); ++l_It)
    {
        std::string ScanRoutineName = *l_It;
        int OptionsArray[1] = { 0 };
        const int RoutineIsRunning = 2;
        (void)FAGetRoutineState (ID, ScanRoutineName.c_str (), OptionsArray);

        if (RoutineIsRunning == OptionsArray[0])
        {
            ReturnString = ScanRoutineName + " ";
        }
    }

    return ReturnString;
}

// ----------------------------------------------------------------------------------------------------------
void RoutinesRunningResults (int ID, const FDRScan& FDR, const FDGScan& FDG, const FDLScan& FDL)
{
    std::string ScanRoutineName = GetScanRoutineName (FDR, FDG, FDL);
    GetResults (ID, ConvertStringToVector (ScanRoutineName, ' '));
}

// ----------------------------------------------------------------------------------------------------------
void GetResults (int ID, const std::vector<std::string>& ScanRoutineNames)
{
    std::vector<std::string>::const_iterator l_ItScan;
    for (l_ItScan = ScanRoutineNames.begin (); l_ItScan != ScanRoutineNames.end (); ++l_ItScan)
    {
        std::string ScanRoutineName = *l_ItScan;
        const int MAX_BUFFER_SIZE = 2047;
        char Result[MAX_BUFFER_SIZE] = { 0 };
        int BufferSize = MAX_BUFFER_SIZE - 1;

        std::map<int, std::string> Results;
        (void)Results.insert (std::make_pair <int, std::string> (2, "Signal level at maximum position, in normed value :\t"));
        (void)Results.insert (std::make_pair <int, std::string> (3, "Position of maximum signal, in [axis unit] :\t\t"));
        (void)Results.insert (std::make_pair <int, std::string> (5, "Process Time / Duration in second :\t\t\t"));

        std::cout << "\tRoutine" << ScanRoutineName << ":\n";
        std::map<int, std::string>::iterator l_It;
        for (l_It = Results.begin (); l_It != Results.end (); ++l_It)
        {
            (void)FAGetRoutineResults (ID
                , ScanRoutineName.c_str ()
                , static_cast<unsigned int>(l_It->first)
                , Result
                , BufferSize);

            std::string l_Tmp (Result);
            unsigned int l_Pos = static_cast<unsigned int>(l_Tmp.find ("="));
            l_Tmp = l_Tmp.substr (l_Pos + 1);

            memset (Result, 0, MAX_BUFFER_SIZE);

            std::cout << "\t\t" << l_It->second << l_Tmp << "\n";
        }
    }
}
// ----------------------------------------------------------------------------------------------------------
void RoutinesRunningStop (int ID, const std::vector <std::string>& ScanRoutineNames)
{
    std::vector <std::string>::const_iterator l_It;
    for (l_It = ScanRoutineNames.begin (); l_It != ScanRoutineNames.end (); ++l_It)
    {
        std::string ScanRoutineName = *l_It;
        int OptionsArray[1] = { 0 };
        const int RoutineIsRunning = 2;
        FAGetRoutineState (ID, ScanRoutineName.c_str (), OptionsArray);

        if (RoutineIsRunning == OptionsArray[0])
        {
            (void)PI_FRP (ID, ScanRoutineName.c_str (), OptionsArray);
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
std::string GetDRR (int ID, std::string ScanRoutineName)
{
    const int c_WAIT_TIME = 1000;
    int OptionsArray[1] = { 0 };
    const int RoutineIsNotRunning = 0;
    while (RoutineIsNotRunning != OptionsArray[0])
    {
        // Gets the current state of a fast alignment routine.
        FAGetRoutineState (ID, ScanRoutineName.c_str (), OptionsArray);
        Sleep (10);
    }

    const int RecordTableIdsArray[] = { 1, 2, 3 };
    int NumberOfRecTables = 3;
    int OffsetOfFirstPointInRecordTable = 1;
    int MinNumberOfValues = 1024;
    double *ValueArray = NULL;
    const int GcsArrayHeaderMaxSize = 10240;
    char GcsArrayHeader[GcsArrayHeaderMaxSize + 1] = { 0 };

    // Read data record tables.
    ReadDataRecordTables (ID
        , RecordTableIdsArray
        , NumberOfRecTables
        , OffsetOfFirstPointInRecordTable
        , MinNumberOfValues
        , &ValueArray
        , GcsArrayHeader
        , GcsArrayHeaderMaxSize
    );

    //wait for data
    int BufferIndex = 0;
    int BufferIndexOld = -1;
    while (BufferIndexOld < BufferIndex)
    {
        Sleep (c_WAIT_TIME);
        BufferIndex = PI_GetAsyncBufferIndex (ID);
        BufferIndexOld = BufferIndex;
    }

    //get data
    std::string OutPut;
    OutPut = GcsArrayHeader;
    for (int PrintIndex = 0; PrintIndex < BufferIndex / NumberOfRecTables; PrintIndex++)
    {
        for (int ValueCounter = 0; ValueCounter < NumberOfRecTables; ValueCounter++)
        {
            OutPut = OutPut + ConvertToStringFixed (ValueArray[(PrintIndex * NumberOfRecTables) + ValueCounter]) + "\t";
        }
        OutPut = OutPut + "\n";
    }

    return OutPut;
}

// ----------------------------------------------------------------------------------------------------------
void RoutinesRunningSavedata (int ID, std::string ScanRoutineName)
{
    const std::string l_FileName = "FASampleAppExport.dat";

    std::string DRRResult = GetDRR (ID, ScanRoutineName);

    std::ofstream OutFile;
    OutFile.open (l_FileName);
    OutFile << DRRResult;
    OutFile.close ();

    std::cout << "\n\t\tSave " << l_FileName << "\n";
}

// ----------------------------------------------------------------------------------------------------------
void RoutinesRunningShowdata (int ID, std::string ScanRoutineName)
{
    std::cout << GetDRR (ID, ScanRoutineName);
}

// ----------------------------------------------------------------------------------------------------------
std::vector<std::string> GetScans (int ID, const FDRScans& FDRs, const FDGScans& FDGs, const FDLScans& FDLs)
{
    std::vector<std::string> l_ScanVector;
    std::string StringID = ConvertToString (ID);

    if (ID < static_cast<int>(FDRs.size ()))
    {
        FDRMap::const_iterator l_It;
        for (l_It = FDRs.at(StringID).begin (); l_It != FDRs.at(StringID).end (); ++l_It)
        {
            l_ScanVector.push_back (ConvertFDRScanToString (l_It->second));
        }
    }
    if (ID < static_cast<int>(FDGs.size ()))
    {
        FDGMap::const_iterator l_It;
        for (l_It = FDGs.at(StringID).begin (); l_It != FDGs.at(StringID).end (); ++l_It)
        {
            l_ScanVector.push_back (ConvertFDGScanToString (l_It->second));
        }
    }
    if (ID < static_cast<int>(FDLs.size()))
    {
        FDLMap::const_iterator l_It;
        for (l_It = FDLs.at(StringID).begin(); l_It != FDLs.at(StringID).end(); ++l_It)
        {
            l_ScanVector.push_back(ConvertFDLScanToString(l_It->second));
        }
    }
    return l_ScanVector;
}

// ----------------------------------------------------------------------------------------------------------
void ShowScans (int ID, const FDRScans& FDRs, const FDGScans& FDGs, const FDLScans& FDLs)
{
    unsigned int l_Count = 1;
    std::vector<std::string> l_Scans = GetScans (ID, FDRs, FDGs, FDLs);

    std::vector<std::string>::iterator l_It;
    for (l_It = l_Scans.begin (); l_It != l_Scans.end (); ++l_It)
    {
        std::cout << "\t" << "(" << l_Count++ << ") " << *l_It << "\n";
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL IsRoutineRunning (int ID, std::string ScanRoutineName)
{
    int OptionsArray[1] = { 0 };
    const int RoutineIsRunning = 2;
    FAGetRoutineState (ID, ScanRoutineName.c_str (), OptionsArray);

    if (RoutineIsRunning == OptionsArray[0])
    {
        return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL IsRoutineRunning (int ID, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs)
{
    std::vector<std::string> l_Scans = GetScans (ID, FDRs, FDGs, FDLs);
    std::string l_ScanName;

    std::vector<std::string>::iterator l_It;
    for (l_It = l_Scans.begin (); l_It != l_Scans.end (); ++l_It)
    {
        std::string l_ScanRoutineName = *l_It;
        if (l_ScanRoutineName.size ())
        {
            if (IsRoutineRunning (ID, l_ScanRoutineName))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// ----------------------------------------------------------------------------------------------------------
void MoveAxesMOV (int ID, const std::map <std::string, double>& AxesPositions)
{
    const int NUM_OF_AXIS_MAX = 256;
    char l_Axis[NUM_OF_AXIS_MAX*2] = {0};
    double l_Pos[NUM_OF_AXIS_MAX] = {0};

    int l_IndexPos = 0;
    int l_IndexAxis = 0;
    std::map <std::string, double>::const_iterator l_It;
    for (l_It = AxesPositions.begin (); l_It != AxesPositions.end (); ++l_It)
    {
        l_Axis[l_IndexAxis] = *l_It->first.c_str();
        l_Pos[l_IndexPos] = l_It->second;

        std::cout << "\t\t" << l_Axis[l_IndexAxis] << "(" << l_Pos[l_IndexPos] << "): ";
        PrintAndReadline (l_Pos[l_IndexPos]);
        l_IndexAxis++;
        l_Axis[l_IndexAxis++] = 10;
        l_IndexPos++;
    }
    if (!PI_MOV (ID, l_Axis, l_Pos))
    {
        return;
    }
    BOOL l_Res[NUM_OF_AXIS_MAX] = { FALSE };
    WaitForAxisToStop (ID, l_Axis, l_Res, NUM_OF_AXIS_MAX);
}

// ----------------------------------------------------------------------------------------------------------
BOOL FAGetRoutineResults (int ID
                          , const char* ScanRoutineName
                            , const unsigned int ResultId
                          , char* Result
                          , int BufferSize)
{
    if (!ScanRoutineName || !Result)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_qFRR (ID
        , ScanRoutineName
        , ResultId
        , Result
        , BufferSize)
        )
    {
        printf_s ("qFRR failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConnectTCPIP (int* pID, char* IP, int Port)
{
    *pID = PI_ConnectTCPIP (IP, Port);

    if (0 > *pID)
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConnectRS232 (int* pID, int comPort, int baudRate)
{
    *pID = PI_ConnectRS232 (comPort, baudRate);

    if (0 > *pID)
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConnectUSB (int* pID)
{
    char ControllerSerialNumber[] = "0";  // Use function EnumerateUSB to get all PI controller 
                                           // connected to you PC or look at the label of the case of your controller.
    *pID = PI_ConnectUSB (ControllerSerialNumber);

    if (0 > *pID)
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
void WaitForAxisToStop (int ID, char * Axis, BOOL* Res, const int MaxLen)
{
    BOOL l_Moving = TRUE;
    while (l_Moving)
    {
        (void)PI_IsMoving (ID, Axis, Res);
        l_Moving = FALSE;
        for (int l_Count = 0; l_Count < MaxLen; l_Count++)
        {
            l_Moving |= Res[l_Count] == TRUE;
        }
        Sleep (10);
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL FADefineAreaScanRoutine (int ID
                              , const char* ScanRoutineName
                              , char* axis
                              , const double ScanAxisRange
                              , const char* StepAxis
                              , const double StepAxisRange
                              , const char* Parameters)
{
    if (!axis
        || !ScanRoutineName
        || !ScanAxisRange
        || !StepAxis
        || !StepAxisRange
        || !Parameters
        )
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }


    if (!PI_FDR (ID
        , ScanRoutineName
        , axis
        , ScanAxisRange
        , StepAxis
        , StepAxisRange
        , Parameters)
        )
    {
        printf_s ("FDR failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL FADefineGradientScanRoutine (int ID
                                  , const char* ScanRoutineName
                                  , char* axis
                                  , const char* StepAxis
                                  , const char* Parameters)
{
    if (!axis
        || !ScanRoutineName
        || !StepAxis
        || !Parameters
        )
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_FDG (ID
        , ScanRoutineName
        , axis
        , StepAxis
        , Parameters)
       )
    {
        printf_s ("FDG failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL FADefineLightningScanRoutine(int ID
    , const char* ScanRoutineName
    , char* axis
    , const double ScanAxisRange
    , const char* StepAxis
    , const double StepAxisRange
    , const char* Parameters)
{
    if (!axis
        || !ScanRoutineName
        || !ScanAxisRange
        || !StepAxis
        || !StepAxisRange
        || !Parameters
        )
    {
        printf_s("wrong parameter.\n");
        return FALSE;
    }


    if (!PI_FDL(ID
        , ScanRoutineName
        , axis
        , ScanAxisRange
        , StepAxis
        , StepAxisRange
        , Parameters)
        )
    {
        printf_s("FDL failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ControllerConnect (IniConfs Confs, int *pID, ConnectMap& Connections)
{
    IniConfs::iterator l_ItConfs;
    for (l_ItConfs = Confs.begin (); l_ItConfs != Confs.end (); ++l_ItConfs)
    {
        std::string l_Comtype = l_ItConfs->second[ConfKeys::eCommunicationType];
        if ("TCPIP" == l_Comtype)
        {
            std::string IP = l_ItConfs->second[ConfKeys::eIPAdress];
            std::string Port = l_ItConfs->second[ConfKeys::eIPPort];
            if (ConnectTCPIP (pID, (char*)IP.c_str (), std::stoi (Port)))
            {
                Connections.insert (std::pair<int, string> (*pID, (ReadControllerName (*pID) + " " + ReadControllerAlias (*pID))));
            }
        }
        else if ("USB" == l_Comtype)
        {
            if (ConnectUSB (pID))
            {
                Connections.insert (std::pair<int, string> (*pID, ReadControllerName (*pID) + " " + ReadControllerAlias (*pID)));
            }
        }
        else if ("RS232" == l_Comtype)
        {
            std::string ComPort = l_ItConfs->second[ConfKeys::eComPortNr];
            std::string BaudRate = l_ItConfs->second[ConfKeys::eComPortBaudrate];
            if (ConnectRS232 (pID, std::stoi (ComPort), std::stoi (BaudRate)))
            {
                Connections.insert (std::pair<int, string> (*pID, ReadControllerName (*pID) + " " + ReadControllerAlias (*pID)));
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL FAStartsRoutine (int ID, const char* ScanRoutineName)
{
    if (!ScanRoutineName)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }


    if (!PI_FRS (ID
        , ScanRoutineName)
    )
    {
        printf_s ("FRS failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL SetDataRecorderTableTriggerSource (int ID
    , const int* RecordTableIdsArray
    , int* TriggerSourceArray
    , char* Values
    , int ArraySize)
{
    if (!RecordTableIdsArray || !TriggerSourceArray || !Values)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_DRT (ID
        , RecordTableIdsArray
        , TriggerSourceArray
        , Values
        , ArraySize)
       )
    {
        printf_s ("DRT failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL SetDataRecorderConfiguration ( int ID
                                    , const int* RecordTableIdsArray
                                    , char* RecordSourceIds
                                    , int* RecordOptionsArray)
{
    if (!RecordTableIdsArray || !RecordSourceIds)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }


    if (!PI_DRC (ID
        , RecordTableIdsArray
        , RecordSourceIds
        , RecordOptionsArray
    )
        )
    {
        printf_s ("DRC failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ReadDataRecordTables (int ID
    , const int* RecordTableIdsArray
    , int NumberOfRecTables
    , int OffsetOfFirstPointInRecordTable
    , int MinNumberOfValues
    , double** ValueArray
    , char* GcsArrayHeader
    , int GcsArrayHeaderMaxSize)
{
    if (!RecordTableIdsArray || !ValueArray || !GcsArrayHeader)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_qDRR (ID
        , RecordTableIdsArray
        , NumberOfRecTables
        , OffsetOfFirstPointInRecordTable
        , MinNumberOfValues
        , ValueArray
        , GcsArrayHeader
        , GcsArrayHeaderMaxSize)
       )
    {
        printf_s ("qDRR failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL FAGetInputChannelValue (int ID
    , const int* FastAlignmentInputIdsArray
    , double* CalculatedInputValueArray
    , int ArraySize)
{
    if (!FastAlignmentInputIdsArray || !CalculatedInputValueArray)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_qTCI (ID
        , FastAlignmentInputIdsArray
        , CalculatedInputValueArray
        , ArraySize)
       )
    {
        printf_s ("qTCI failed.\n");
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
std::string ReadControllerName (int ID)
{
    if (0 > ID)
    {
        return "";
    }
    char Buffer[2047] = { 0 };
    (void)PI_qIDN (ID, &Buffer[0], 2046);

    std::string l_Tmp (Buffer);
    unsigned int l_Pos1 = static_cast <unsigned int> (l_Tmp.find_first_of (","));
    l_Tmp = l_Tmp.substr (l_Pos1 + 1);
    l_Tmp.erase (0, l_Tmp.find_first_not_of (" "));
    unsigned int l_Pos2 = static_cast <unsigned int> (l_Tmp.find_first_of (","));
    return l_Tmp.substr (0, l_Pos2);
}

// ----------------------------------------------------------------------------------------------------------
void GetGCSCommandSet (int ID, std::string Command, std::string& Answer)
{
    const int c_READ_TIME_MAX = 25;
    const int c_BUFFER_SIZE_MAX = 4096;
    char l_Buffer[c_BUFFER_SIZE_MAX] = { 0 };

    (void)PI_GcsCommandset (ID, Command.c_str ());

    int i_AnswerSize = 1;
    while (0 < i_AnswerSize)
    {
        (void)PI_GcsGetAnswer (ID, l_Buffer, c_BUFFER_SIZE_MAX);
        Answer.append (l_Buffer);
        memset (l_Buffer, 0, c_BUFFER_SIZE_MAX - 1);
        Sleep (c_READ_TIME_MAX);
        (void)PI_GcsGetAnswerSize (ID, &i_AnswerSize);
    }
}

// ----------------------------------------------------------------------------------------------------------
std::string ReadControllerAlias (int ID)
{
    if (0 > ID)
    {
        return "";
    }

    std::string l_Buffer;
    GetGCSCommandSet (ID, "SPA? 1 0X0D000700", l_Buffer);

    std::string l_Answer;
    unsigned int l_Pos = static_cast <unsigned int> (l_Buffer.find ("="));
    l_Answer = l_Buffer.substr (l_Pos + 1);
    l_Pos = l_Answer.find (10);
    l_Answer = l_Answer.substr (0, l_Pos);

    if (1 >= l_Buffer.size ())
    {
        return "";
    }

    l_Buffer.clear ();
    GetGCSCommandSet (ID, "SPA? 1 0X0D001000", l_Buffer);

    l_Pos = l_Buffer.find ("=");
    l_Answer = l_Answer + "-" + l_Buffer.substr (l_Pos + 1);
    l_Pos = l_Answer.find (10);
    l_Answer = l_Answer.substr (0, l_Pos);

    return l_Answer;
}

// ----------------------------------------------------------------------------------------------------------
std::string GetError (int ID)
{
    if (0 > ID)
    {
        return "";
    }

    int ErrorNr = PI_GetError (ID);
    char Buffer[2047] = { 0 };
    std::string ErrorMessage = "No Error";
    if (0 != ErrorNr)
    {
        (void)PI_TranslateError (ErrorNr, Buffer, 2046);
        ErrorMessage = Buffer;
    }

    return ErrorMessage;
}

// ----------------------------------------------------------------------------------------------------------
BOOL FAGetRoutineState (int ID, const char* ScanRoutineName, int* OptionsArray)
{
    if (!ScanRoutineName)
    {
        printf_s ("wrong parameter.\n");
        return FALSE;
    }

    if (!PI_qFRP (ID
        , ScanRoutineName
        , OptionsArray)
        )
    {
        printf_s ("qFRP failed.\n");
        return FALSE;
    }

    return TRUE;
}
