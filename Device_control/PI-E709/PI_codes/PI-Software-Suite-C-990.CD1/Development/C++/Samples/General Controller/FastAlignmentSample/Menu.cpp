#include "Menu.h"
#include "GCSMethods.h"
#include "Helper.h"

// ----------------------------------------------------------------------------------------------------------
unsigned int GetUserInput (int& MenuSelector)
{
    std::cout << "\n-->";
    unsigned int UserInput = 0;
    PrintAndReadline (UserInput);

    if (0 == UserInput)
    {
        MenuSelector = 0;
        return UserInput;
    }

    return UserInput;
}

// ----------------------------------------------------------------------------------------------------------
std::string GetUserInput (int& MenuSelector, std::vector<int>& l_iScanVector)
{
    std::cout << "\n----->";
    std::string UserInput;
    PrintAndReadline (UserInput);

    if ("0" == UserInput)
    {
        l_iScanVector.push_back (0);
        MenuSelector = 0;
        return "0";
    }

    std::vector <std::string> l_ScanVector = ConvertStringToVector (UserInput, ' ');
    std::vector <std::string>::iterator l_It;

    for (l_It = l_ScanVector.begin (); l_It != l_ScanVector.end (); ++l_It)
    {
        try
        {
            l_iScanVector.push_back (std::stoi (l_It->c_str()));
        }
        catch (const std::invalid_argument&)
        {
            l_iScanVector.clear();
            l_iScanVector.push_back (0);
            MenuSelector = 0;
            return "";
        }
    }

    return UserInput;
}

// ----------------------------------------------------------------------------------------------------------
void Header (int ID, const ConnectMap& Connections)
{
    std::cout << "\n";
    std::cout << "\n";
    std::cout << doc_application_name;
    std::cout << doc_company_name;
    std::cout << "\n";
    //std::cout << doc_current_controller << ReadControllerName (ID) <<" "<< ReadControllerAlias (ID) <<"\n";
    std::cout << doc_current_controller << Connections.at(ID) << "\n";

    std::cout << doc_error_status << GetError (ID) << "\n";
    std::cout << "\n";

    std::cin.clear ();
}
// ----------------------------------------------------------------------------------------------------------
void MainMenue (int& MenuSelector)
{
    std::cout << doc_select_controller;
    std::cout << doc_edit_routines;
    std::cout << doc_start_routine_single;
    std::cout << doc_start_routine_multiple;
    std::cout << doc_show_analogvalues;
    std::cout << doc_move_axes;
    std::cout << doc_exit_application;

    MenuSelector = GetUserInput (MenuSelector);
    if (0 == MenuSelector)
    {
        MenuSelector = -1;
    }
}

// ----------------------------------------------------------------------------------------------------------
void SelectController (int* ID, int& MenuSelector, const ConnectMap& Connections)
{
    std::cout << doc_select_controller;

    ConnectMap::const_iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        std::cout << "\t" << "(" << l_It->first+1 << ") " << l_It->second << "\n";
    }
    std::cout << "\t" << "(0) Back" << "\n";

    int UserInput = GetUserInput (MenuSelector);
    if (static_cast<int>(Connections.size()) >= UserInput && 0 < UserInput)
    {
        *ID = UserInput -1;
        MenuSelector = 0;
    }
}

// ----------------------------------------------------------------------------------------------------------
void AddEditRoutine (int ID, int& MenuSelector, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs, const ConnectMap& Connections)
{
    std::cout << doc_edit_routines;

    ShowScans (ID, FDRs, FDGs, FDLs);
    std::cout << "\t" << "(0) Back" << "\n";

    std::string StringID = ConvertToString (ID);
    unsigned int l_NumOfFDRScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDGScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDLScan = static_cast<unsigned int> (FDLs[StringID].size());

    unsigned int UserInput = GetUserInput (MenuSelector);
    if (0 == UserInput)
    {
        return;
    }

    if (UserInput <= l_NumOfFDRScan)
    {
        FDRScan l_SelectedScan = FDRs[StringID][UserInput-1];
        std::string l_OldScanRoutineName = l_SelectedScan.ScanRoutineName;
        std::cout << "\t\t" << doc_manage_routine_name  << "("<< l_SelectedScan.ScanRoutineName << "): ";
        PrintAndReadline (l_SelectedScan.ScanRoutineName);
        std::cout << "\t\t" << doc_manage_scan_axis  << "("<< l_SelectedScan.ScanAxis << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxis);
        std::cout << "\t\t" << doc_manage_scan_axis_range  << "("<< l_SelectedScan.ScanAxisRange << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxisRange);
        std::cout << "\t\t" << doc_manage_scan_axis  << "("<< l_SelectedScan.ScanAxis << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxis);
        std::cout << "\t\t" << doc_manage_scan_axis_range  << "("<< l_SelectedScan.ScanAxisRange << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxisRange);
        std::cout << "\t\t" << doc_manage_step_axis  << "("<< l_SelectedScan.StepAxis << "): ";
        PrintAndReadline (l_SelectedScan.StepAxis);
        std::cout << "\t\t" << doc_manage_step_axis_range  << "("<< l_SelectedScan.StepAxisRange << "): ";
        PrintAndReadline (l_SelectedScan.StepAxisRange);
        std::cout << "\t\t" << doc_manage_mp1 << "("<< l_SelectedScan.ThresholdLevel << "): ";
        PrintAndReadline (l_SelectedScan.ThresholdLevel);
        std::cout << "\t\t" << doc_manage_a << "("<< l_SelectedScan.InputChannel << "): ";
        PrintAndReadline (l_SelectedScan.InputChannel);
        std::cout << "\t\t" << doc_manage_f << "("<< l_SelectedScan.Frequency << "): ";
        PrintAndReadline (l_SelectedScan.Frequency);
        std::cout << "\t\t" << doc_manage_v << "("<< l_SelectedScan.Velocity << "): ";
        PrintAndReadline (l_SelectedScan.Velocity);
        std::cout << "\t\t" << doc_manage_mp1 << "("<< l_SelectedScan.ScanAxisMidPos << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxisMidPos);
        std::cout << "\t\t" << doc_manage_mp2 << "("<< l_SelectedScan.StepAxisMidPos << "): ";
        PrintAndReadline (l_SelectedScan.StepAxisMidPos);
        std::cout << "\t\t" << doc_manage_tt << "("<< l_SelectedScan.TargetType << "): ";
        PrintAndReadline (l_SelectedScan.TargetType);
        std::cout << "\t\t" << doc_manage_cm << "("<< l_SelectedScan.CentroidMethod << "): ";
        PrintAndReadline (l_SelectedScan.CentroidMethod);
        std::cout << "\t\t" << doc_manage_miil << "("<< l_SelectedScan.MinRelInputLevel << "): ";
        PrintAndReadline (l_SelectedScan.MinRelInputLevel);
        std::cout << "\t\t" << doc_manage_mail << "("<< l_SelectedScan.MaxRelInputLevel << "): ";
        PrintAndReadline (l_SelectedScan.MaxRelInputLevel);
        std::cout << "\t\t" << doc_manage_st << "("<< l_SelectedScan.StopPosition << "): ";
        PrintAndReadline (l_SelectedScan.StopPosition);

        //Send FDRToController
        std::string l_Parameters;
        MakeFDRParameters (l_SelectedScan, l_Parameters);
        (void)FADefineAreaScanRoutine ( ID
                                ,l_SelectedScan.ScanRoutineName.c_str()
                                ,(char*)l_SelectedScan.ScanAxis.c_str()
                                ,l_SelectedScan.ScanAxisRange
                                ,l_SelectedScan.StepAxis.c_str()
                                ,l_SelectedScan.StepAxisRange
                                ,l_Parameters.c_str() );
    }
    else if (UserInput <= l_NumOfFDRScan + l_NumOfFDGScan)
    {
        FDGScan l_SelectedScan = FDGs[StringID][UserInput-1-l_NumOfFDRScan];
        std::string l_OldScanRoutineName = l_SelectedScan.ScanRoutineName;
        std::cout << "\t\t" << doc_manage_routine_name  << "("<< l_SelectedScan.ScanRoutineName << "): ";
        PrintAndReadline (l_SelectedScan.ScanRoutineName);
        std::cout << "\t\t" << doc_manage_scan_axis  << "("<< l_SelectedScan.ScanAxis << "): ";
        PrintAndReadline (l_SelectedScan.ScanAxis);
        std::cout << "\t\t" << doc_manage_step_axis  << "("<< l_SelectedScan.StepAxis << "): ";
        PrintAndReadline (l_SelectedScan.StepAxis);
        std::cout << "\t\t" << doc_manage_ml  << "("<< l_SelectedScan.MinimumLevel << "): ";
        PrintAndReadline (l_SelectedScan.MinimumLevel);
        std::cout << "\t\t" << doc_manage_a  << "("<< l_SelectedScan.InputChannel << "): ";
        PrintAndReadline (l_SelectedScan.InputChannel);
        std::cout << "\t\t" << doc_manage_mia  << "("<< l_SelectedScan.MinAmplitude << "): ";
        PrintAndReadline (l_SelectedScan.MinAmplitude);
        std::cout << "\t\t" << doc_manage_maa  << "("<< l_SelectedScan.MaxAmplitude << "): ";
        PrintAndReadline (l_SelectedScan.MaxAmplitude);
        std::cout << "\t\t" << doc_manage_f  << "("<< l_SelectedScan.Frequency << "): ";
        PrintAndReadline (l_SelectedScan.Frequency);
        std::cout << "\t\t" << doc_manage_sp  << "("<< l_SelectedScan.SpeedFactor << "): ";
        PrintAndReadline (l_SelectedScan.SpeedFactor);
        std::cout << "\t\t" << doc_manage_v  << "("<< l_SelectedScan.Velocity << "): ";
        PrintAndReadline (l_SelectedScan.Velocity);
        std::cout << "\t\t" << doc_manage_mdc  << "("<< l_SelectedScan.MaxDirectionChanges << "): ";
        PrintAndReadline (l_SelectedScan.MaxDirectionChanges);
        std::cout << "\t\t" << doc_manage_spo  << "("<< l_SelectedScan.SpeedOffset << "): ";
        PrintAndReadline (l_SelectedScan.SpeedOffset);

        //Send FDGToController
        std::string l_Parameters;
        MakeFDGParameters (l_SelectedScan, l_Parameters);
        (void)FADefineGradientScanRoutine ( ID
                                    ,l_SelectedScan.ScanRoutineName.c_str()
                                    ,(char*)l_SelectedScan.ScanAxis.c_str()
                                    ,l_SelectedScan.StepAxis.c_str()
                                    ,l_Parameters.c_str() );
    }
    else if (UserInput <= l_NumOfFDRScan + l_NumOfFDGScan + l_NumOfFDLScan)
    {
        FDLScan l_SelectedScan = FDLs[StringID][UserInput - 1 - l_NumOfFDRScan - l_NumOfFDGScan];
        std::string l_OldScanRoutineName = l_SelectedScan.ScanRoutineName;
        std::cout << "\t\t" << doc_manage_routine_name << "(" << l_SelectedScan.ScanRoutineName << "): ";
        PrintAndReadline(l_SelectedScan.ScanRoutineName);
        std::cout << "\t\t" << doc_manage_scan_axis << "(" << l_SelectedScan.ScanAxis << "): ";
        PrintAndReadline(l_SelectedScan.ScanAxis);
        std::cout << "\t\t" << doc_manage_scan_axis_range << "(" << l_SelectedScan.ScanAxisRange << "): ";
        PrintAndReadline(l_SelectedScan.ScanAxisRange);
        std::cout << "\t\t" << doc_manage_step_axis << "(" << l_SelectedScan.StepAxis << "): ";
        PrintAndReadline(l_SelectedScan.StepAxis);
        std::cout << "\t\t" << doc_manage_step_axis_range << "(" << l_SelectedScan.StepAxisRange << "): ";
        PrintAndReadline(l_SelectedScan.StepAxisRange);
        std::cout << "\t\t" << doc_manage_s << "(" << l_SelectedScan.ThresholdSigma << "): ";
        PrintAndReadline(l_SelectedScan.ThresholdSigma);
        std::cout << "\t\t" << doc_manage_c << "(" << l_SelectedScan.ThresholdCalculation << "): ";
        PrintAndReadline(l_SelectedScan.ThresholdCalculation);
        std::cout << "\t\t" << doc_manage_a << "(" << l_SelectedScan.InputChannel << "): ";
        PrintAndReadline(l_SelectedScan.InputChannel);
        std::cout << "\t\t" << doc_manage_mp1 << "(" << l_SelectedScan.ScanAxisMidPos << "): ";
        PrintAndReadline(l_SelectedScan.ScanAxisMidPos);
        std::cout << "\t\t" << doc_manage_mp2 << "(" << l_SelectedScan.StepAxisMidPos << "): ";
        PrintAndReadline(l_SelectedScan.StepAxisMidPos);
        std::cout << "\t\t" << doc_manage_cp1 << "(" << l_SelectedScan.ScanAxisCalcPos << "): ";
        PrintAndReadline(l_SelectedScan.ScanAxisCalcPos);
        std::cout << "\t\t" << doc_manage_cp2 << "(" << l_SelectedScan.StepAxisCalcPos << "): ";
        PrintAndReadline(l_SelectedScan.StepAxisCalcPos);

        //Send FDLToController
        std::string l_Parameters;
        MakeFDLParameters(l_SelectedScan, l_Parameters);
        (void)FADefineLightningScanRoutine(ID
            , l_SelectedScan.ScanRoutineName.c_str()
            , (char*)l_SelectedScan.ScanAxis.c_str()
            , l_SelectedScan.ScanAxisRange
            , l_SelectedScan.StepAxis.c_str()
            , l_SelectedScan.StepAxisRange
            , l_Parameters.c_str());
    }

    //Readback FDR-/FDGScans (merge INI-data scans + controller scans)
    FDRs.clear ();
    FDGs.clear ();
    FDLs.clear();
    GetControllerFDRScans (Connections, FDRs);
    GetControllerFDGScans (Connections, FDGs);
    GetControllerFDLScans (Connections, FDLs);
}

// ----------------------------------------------------------------------------------------------------------
void StartSingleRoutineSubMenu (
    int ID,
    int& MenuSelector,
    FDRScan& FDR,
    FDGScan& FDG,
    FDLScan& FDL,
    const ConnectMap& Connections)
{
    Header (ID, Connections);

    std::string ScanRoutineName = GetRunningScanRoutineName (ID, FDR,  FDG, FDL);
    std::cout << "\t\t" << doc_routines_running << ScanRoutineName <<"\n";
    std::cout << "\t\t" << doc_routines_running_refresh;    //1
    std::cout << "\t\t" << doc_routines_running_stop;       //2
    std::cout << "\t\t" << doc_routines_running_results;    //3
    std::cout << "\t\t" << doc_routines_running_savedata;   //4
    std::cout << "\t\t" << doc_routines_running_showdata;   //5
    std::cout << "\t\t" << doc_menu_back;   //0

    unsigned int UserInput = GetUserInput (MenuSelector);

    if (1 == UserInput)
    {
        ;//Refresh per reload!
    }
    else if (2 == UserInput)
    {
        RoutinesRunningStop (ID, ConvertStringToVector(ScanRoutineName,' '));
    }
    else if (3 == UserInput)
    {
        RoutinesRunningResults (ID, FDR, FDG, FDL);
    }
    else if (4 == UserInput)
    {
        RoutinesRunningSavedata (ID, ScanRoutineName);
    }
    else if (5 == UserInput)
    {
        RoutinesRunningShowdata (ID, ScanRoutineName);
    }
    else
    {
        MenuSelector = Menu::eStartSingleRoutineDR;
    }
}

// ----------------------------------------------------------------------------------------------------------
void StartMultipleRoutineSubMenu (int ID, int& MenuSelector, const std::string& ScanRoutineNames, const ConnectMap& Connections)
{
    Header (ID, Connections);

    std::string ScanRoutineName = GetRunningScanRoutineName (ID, ConvertStringToVector (ScanRoutineNames, ' '));
    std::cout << "\t\t" << doc_routines_running << ScanRoutineName <<"\n";
    std::cout << "\t\t" << doc_routines_running_refresh;
    std::cout << "\t\t" << doc_routines_running_stop;
    std::cout << "\t\t" << doc_routines_running_results;
    std::cout << "\t\t" << doc_menu_back;

    int UserInput = GetUserInput (MenuSelector);

    if (1 == UserInput)
    {
        ;//Refresh per reload!
    }
    else if (2 == UserInput)
    {
        RoutinesRunningStop (ID, ConvertStringToVector (ScanRoutineName, ' '));
    }
    else if (3 == UserInput)
    {
        GetResults (ID, ConvertStringToVector(ScanRoutineNames, ' '));
    }
    else
    {
        MenuSelector = Menu::eStartMultipleRoutine;
        return;
    }
}

// ----------------------------------------------------------------------------------------------------------
void StartMultipleRoutine (int ID, int& MenuSelector, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs, const ConnectMap& Connections)
{
    std::cout << doc_start_routine_multiple;

    ShowScans (ID, FDRs, FDGs, FDLs);
    std::cout << "\t" << "(0) Back" << "\n";
    std::cout << "\n\t" << "seperate scans by space" << "\n";

    std::string StringID = ConvertToString (ID);
    unsigned int l_NumOfFDRScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDGScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDLScan = static_cast<unsigned int> (FDLs[StringID].size());

    std::vector<int> l_ScanVector;
    std::string l_Scans = GetUserInput (MenuSelector, l_ScanVector);

    if ("0" == l_Scans)
    {
        return;
    }

    FDRScan l_SelectedFDRScan;
    FDGScan l_SelectedFDGScan;
    FDLScan l_SelectedFDLScan;

    std::string ScanRoutineName;
    std::string ScanRoutineNames;
    std::vector<int>::iterator l_It;
    for (l_It = l_ScanVector.begin (); l_It != l_ScanVector.end (); ++l_It)
    {
        unsigned int l_Scan = *l_It;

        if (l_Scan <= l_NumOfFDRScan)
        {
            l_SelectedFDRScan = FDRs[StringID][l_Scan-1];
            std::string l_Parameters;
            MakeFDRParameters (l_SelectedFDRScan, l_Parameters);
            (void)FADefineAreaScanRoutine ( ID
                                            ,l_SelectedFDRScan.ScanRoutineName.c_str()
                                            ,(char*)l_SelectedFDRScan.ScanAxis.c_str()
                                            ,l_SelectedFDRScan.ScanAxisRange
                                            ,l_SelectedFDRScan.StepAxis.c_str()
                                            ,l_SelectedFDRScan.StepAxisRange
                                            ,l_Parameters.c_str() );

            ScanRoutineName  = l_SelectedFDRScan.ScanRoutineName;
            ScanRoutineNames = ScanRoutineNames + ScanRoutineName + " ";
        }
        else if (l_Scan <= l_NumOfFDRScan + l_NumOfFDGScan)
        {
            l_SelectedFDGScan = FDGs[StringID][l_Scan-1-l_NumOfFDRScan];
            std::string l_Parameters;
            MakeFDGParameters (l_SelectedFDGScan, l_Parameters);
            (void)FADefineGradientScanRoutine ( ID
                                                ,l_SelectedFDGScan.ScanRoutineName.c_str()
                                                ,(char*)l_SelectedFDGScan.ScanAxis.c_str()
                                                ,l_SelectedFDGScan.StepAxis.c_str()
                                                ,l_Parameters.c_str() );
            ScanRoutineName  = l_SelectedFDGScan.ScanRoutineName;
            ScanRoutineNames = ScanRoutineNames + ScanRoutineName + " ";
        }
        else if (l_Scan <= l_NumOfFDRScan + l_NumOfFDGScan + l_NumOfFDLScan)
        {
            l_SelectedFDLScan = FDLs[StringID][l_Scan - 1 - l_NumOfFDRScan - l_NumOfFDGScan];
            std::string l_Parameters;
            MakeFDLParameters(l_SelectedFDLScan, l_Parameters);
            (void)FADefineLightningScanRoutine(ID
                , l_SelectedFDLScan.ScanRoutineName.c_str()
                , (char*)l_SelectedFDLScan.ScanAxis.c_str()
                , l_SelectedFDLScan.ScanAxisRange
                , l_SelectedFDLScan.StepAxis.c_str()
                , l_SelectedFDLScan.StepAxisRange
                , l_Parameters.c_str());

            ScanRoutineName = l_SelectedFDLScan.ScanRoutineName;
            ScanRoutineNames = ScanRoutineNames + ScanRoutineName + " ";
        }

        //Start
        (void)FAStartsRoutine (ID, ScanRoutineName.c_str());
    }

    MenuSelector = Menu::eStartMultipleRoutineDRSub;
    while (Menu::eStartMultipleRoutineDRSub == MenuSelector)
    {
        StartMultipleRoutineSubMenu (ID, MenuSelector, ScanRoutineNames, Connections);
    }
}

// ----------------------------------------------------------------------------------------------------------
void StartSingleRoutineDR (int ID,
    int& MenuSelector,
    FDRScans& FDRs,
    FDGScans& FDGs,
    FDLScans& FDLs,
    const ConnectMap& Connections)
{
    std::cout << doc_start_routine_single;
    ShowScans (ID, FDRs, FDGs, FDLs);
    std::cout << "\t" << "(0) Back" << "\n";

    std::string StringID = ConvertToString (ID);
    unsigned int l_NumOfFDRScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDGScan = static_cast<unsigned int> (FDRs[StringID].size());
    unsigned int l_NumOfFDLScan = static_cast<unsigned int> (FDLs[StringID].size());

    unsigned int  UserInput = GetUserInput (MenuSelector);
    if (0 == UserInput)
    {
        return;
    }

    std::string ScanRoutineName;
    std::string ScanAxis;
    std::string StepAxis;
    FDRScan l_SelectedFDRScan;
    FDGScan l_SelectedFDGScan;
    FDLScan l_SelectedFDLScan;

    if (UserInput <= l_NumOfFDRScan)
    {
        l_SelectedFDRScan = FDRs[StringID][UserInput-1];
        std::string l_Parameters;
        MakeFDRParameters (l_SelectedFDRScan, l_Parameters);
        (void)FADefineAreaScanRoutine ( ID
                                ,l_SelectedFDRScan.ScanRoutineName.c_str()
                                ,(char*)l_SelectedFDRScan.ScanAxis.c_str()
                                ,l_SelectedFDRScan.ScanAxisRange
                                ,l_SelectedFDRScan.StepAxis.c_str()
                                ,l_SelectedFDRScan.StepAxisRange
                                ,l_Parameters.c_str() );

        ScanRoutineName  = l_SelectedFDRScan.ScanRoutineName;
        ScanAxis         = l_SelectedFDRScan.ScanAxis;
        StepAxis         = l_SelectedFDRScan.StepAxis;
    }
    else if (UserInput <= l_NumOfFDRScan + l_NumOfFDGScan)
    {
        l_SelectedFDGScan = FDGs[StringID][UserInput-1-l_NumOfFDRScan];
        std::string l_Parameters;
        MakeFDGParameters (l_SelectedFDGScan, l_Parameters);
        (void)FADefineGradientScanRoutine ( ID
                                    ,l_SelectedFDGScan.ScanRoutineName.c_str()
                                    ,(char*)l_SelectedFDGScan.ScanAxis.c_str()
                                    ,l_SelectedFDGScan.StepAxis.c_str()
                                    ,l_Parameters.c_str() );
        ScanRoutineName  = l_SelectedFDGScan.ScanRoutineName;
        ScanAxis         = l_SelectedFDGScan.ScanAxis;
        StepAxis         = l_SelectedFDGScan.StepAxis;
    }
    else if (UserInput <= l_NumOfFDRScan + l_NumOfFDGScan + l_NumOfFDLScan)
    {
        l_SelectedFDLScan = FDLs[StringID][UserInput - 1 - l_NumOfFDRScan - l_NumOfFDGScan];
        std::string l_Parameters;
        MakeFDLParameters(l_SelectedFDLScan, l_Parameters);
        (void)FADefineLightningScanRoutine(ID
            , l_SelectedFDLScan.ScanRoutineName.c_str()
            , (char*)l_SelectedFDLScan.ScanAxis.c_str()
            , l_SelectedFDLScan.ScanAxisRange
            , l_SelectedFDLScan.StepAxis.c_str()
            , l_SelectedFDLScan.StepAxisRange
            , l_Parameters.c_str());

        ScanRoutineName = l_SelectedFDLScan.ScanRoutineName;
        ScanAxis = l_SelectedFDLScan.ScanAxis;
        StepAxis = l_SelectedFDLScan.StepAxis;
    }
    //DR
    //Defines a trigger source for the given data recorder table.
    const int DRTIdsArray[]             = {1};
    int TriggerSourceArray[]          = {1};
    char Values[]                     = "1";
    int ArraySize                     = 1;
    SetDataRecorderTableTriggerSource ( ID
                                        ,DRTIdsArray
                                        ,TriggerSourceArray
                                        ,Values
                                        ,ArraySize );

    const int RecordTableIdsArray[] = {1, 2, 3};
    const char FAAnalogInput[]      = "1";
    char RecordSourceIds[9]         = {0};

    //Set data recorder configuration.
    strcat_s (RecordSourceIds, ScanAxis.c_str());
    strcat_s (RecordSourceIds, " ");
    strcat_s (RecordSourceIds, StepAxis.c_str());
    strcat_s (RecordSourceIds, " ");
    strcat_s (RecordSourceIds, FAAnalogInput);
    int RecordOptionsArray[3] = {2, 2, 150};
    SetDataRecorderConfiguration (  ID
                                    ,RecordTableIdsArray
                                    ,RecordSourceIds
                                    ,RecordOptionsArray
                                  );


    //Start
    (void)FAStartsRoutine (ID, ScanRoutineName.c_str() );

    MenuSelector = Menu::eStartSingleRoutineDRSub;
    while (Menu::eStartSingleRoutineDRSub == MenuSelector)
    {
        StartSingleRoutineSubMenu (ID, MenuSelector, l_SelectedFDRScan, l_SelectedFDGScan, l_SelectedFDLScan, Connections);
    }
}

// ----------------------------------------------------------------------------------------------------------
void ShowAnalogValues (int ID, int& MenuSelector)
{
    std::cout << doc_show_analogvalues;
    std::cout << doc_menu_back;
    std::cout << "\n";
    const int NumOfIds = 4;
    int FastAlignmentInputIdsArray[NumOfIds] = {1,2,3,4};
    double CalculatedInputValueArray[]      = {0.0};
    const int ArraySize                     = 1;
    int l_Count = 0;
    while (l_Count < NumOfIds)
    {
        (void)FAGetInputChannelValue ( ID
                                ,&FastAlignmentInputIdsArray[l_Count]
                                ,CalculatedInputValueArray
                                ,ArraySize );
        std::cout << "\t" << FastAlignmentInputIdsArray[l_Count++] << "=" << CalculatedInputValueArray[0] << "\n";
    }

    int UserInput = GetUserInput (MenuSelector);
    if (0 == UserInput)
    {
        MenuSelector = Menu::eMainMenu;
    }
}

// ----------------------------------------------------------------------------------------------------------
std::map <std::string, double> GetAxesPositions (int ID)
{
    const int NUM_OF_AXIS_MAX = 256;
    char l_Axis[NUM_OF_AXIS_MAX] = {0};
    double l_Pos[NUM_OF_AXIS_MAX] = {0};
    (void)PI_qPOS (ID, &l_Axis[0], l_Pos);
    (void)PI_qSAI (ID, l_Axis, NUM_OF_AXIS_MAX-1);

    std::map <std::string, double> l_AxisPosMap;
    int l_Count = 0;
    std::vector <std::string> l_AxisVec = ConvertStringToVector (l_Axis, ' ');
    std::vector <std::string>::iterator l_It;
    for (l_It = l_AxisVec.begin (); l_It != l_AxisVec.end (); ++l_It)
    {
        std::string l_sAxis = *l_It;
        l_sAxis.erase (0, l_sAxis.find_first_not_of ("\n"));
        int l_Index = static_cast<int> (l_sAxis.find_last_of ("\n"));
        if (0 <= l_Index)
        {
            l_sAxis.erase (l_Index);
        }
        l_AxisPosMap.insert (std::pair <std::string, double> (l_sAxis, l_Pos[l_Count++]));
    }
    return l_AxisPosMap;
}

// ----------------------------------------------------------------------------------------------------------
void ShowAxesPosition (const std::map <std::string, double>& l_AxesPositions)
{
    std::cout << "\n\t\t" << "Current axes positions:\n";

    std::map <std::string, double>::const_iterator l_It;
    for (l_It = l_AxesPositions.begin (); l_It != l_AxesPositions.end (); ++l_It)
    {
        std::cout << "\t\t" << l_It->first << "=" << l_It->second << "\n";
    }
}

// ----------------------------------------------------------------------------------------------------------
void ShowAvailableAxes (const std::map <std::string, double>& l_AxesPositions)
{
    std::cout << "\n\t\t" << "Available axes:";

    std::map <std::string, double>::const_iterator l_It;
    for (l_It = l_AxesPositions.begin (); l_It != l_AxesPositions.end (); ++l_It)
    {
        std::cout << " " << l_It->first;
    }
    std::cout << "\n";
}

// ----------------------------------------------------------------------------------------------------------
void MoveAxes (int ID, int& MenuSelector, FDRScans& UNUSED(FDRs), FDGScans& UNUSED(FDGs), FDLScans& UNUSED(FDLs))
{
    std::cout << doc_move_axes;
    std::cout << "\t" + doc_move_axes_absolute;
    std::cout << "\t" + doc_move_axes_relative;
    std::cout << "\t" + doc_menu_back;

    std::map <std::string, double> l_AxesPositions = GetAxesPositions (ID);

    const int NUM_OF_AXIS_MAX = 256;
    char l_Axis[NUM_OF_AXIS_MAX] = {0};
    double l_Pos[NUM_OF_AXIS_MAX] = {0};
    (void)PI_qPOS (ID, &l_Axis[0], l_Pos);
    (void)PI_qSAI (ID, l_Axis, NUM_OF_AXIS_MAX-1);

    int UserInput = GetUserInput (MenuSelector);
    if (1 == UserInput)
    {
        MoveAxesMOV (ID, l_AxesPositions);
        ShowAxesPosition (GetAxesPositions (ID));
    }
    else if (2 == UserInput)
    {
        //MVR
        ShowAvailableAxes (l_AxesPositions);
        std::map <std::string, double>::iterator l_It = l_AxesPositions.begin();
        std::string l_AxisToMOV = l_It->first;
        double StepSize = 0.1;
        std::cout << "\t\t" << doc_axis_to_move <<"(" << l_AxisToMOV << "): ";
        PrintAndReadline (l_AxisToMOV);
        std::cout << "\t\t" << doc_step_size <<"(" << StepSize << "): ";
        PrintAndReadline (StepSize);
        std::cout << "\t\t" << doc_step_neg;
        std::cout << "\t\t" << doc_step_pos;
        std::cout << "\t\t" << doc_menu_back;

        while (UserInput)
        {
            UserInput = GetUserInput (MenuSelector);
            if (0 == UserInput)
            {
                MenuSelector = Menu::eMoveAxes;
            }
            else if (1 == UserInput)
            {
                double l_StepSize = -StepSize;
                (void)PI_MVR (ID, l_AxisToMOV.c_str(), &l_StepSize);
            }
            else if (2 == UserInput)
            {
                double l_StepSize = StepSize;
                (void)PI_MVR (ID,l_AxisToMOV.c_str(), &l_StepSize);
            }
            (void)PI_qPOS (ID, l_AxisToMOV.c_str (), l_Pos);
            std::cout << "\t\tCurrent positions of axis (" << l_AxisToMOV << "): " << l_Pos[0];
        }
    }
    else
    {
        MenuSelector = Menu::eMainMenu;
        return;
    }

}
