#pragma once

#include <windows.h>
#include "Declarations.h"

// ----------------------------------------------------------------------------------------------------------
struct Menu
{
    enum EMenu
    {
        eMainMenu = 0,
        eSelectController = 1,
        eAddEditRoutine = 2,
        eStartSingleRoutineDR = 3,
        eStartSingleRoutineDRSub = 31,
        eStartMultipleRoutine = 4,
        eStartMultipleRoutineDRSub = 41,
        eShowAnalogValues = 5,
        eMoveAxes = 6,
        eMoveAxesSub = 61,
    };
};

// ----------------------------------------------------------------------------------------------------------
const std::string doc_select_controller = "(1) Select controller\n";
const std::string doc_edit_routines = "(2) List/add/edit routine\n";
const std::string doc_start_routine_single = "(3) Start single routine with data recorder\n";
const std::string doc_start_routine_multiple = "(4) Start multiple routines without data recorder\n";
const std::string doc_show_analogvalues = "(5) Show signal Intensity\n";
const std::string doc_move_axes = "(6) Move axes\n";
const std::string doc_menu_back = "(0) Back\n";
const std::string doc_exit_application = "(0) Exit\n";

//FDR + FDG + FDL
const std::string doc_manage_routine_name = "Routine name (new name will add routine)";
const std::string doc_manage_scan_axis = "Scan axis";
const std::string doc_manage_scan_axis_range = "Scan axis range";
const std::string doc_manage_step_axis = "Step axis";
const std::string doc_manage_step_axis_range = "Step axis range";
const std::string doc_manage_mp1 = "Scan axis mid. pos.";
const std::string doc_manage_mp2 = "Step axis mid. pos.";
const std::string doc_manage_cp1 = "Scan axis calibration pos.";
const std::string doc_manage_cp2 = "Step axis calibration pos.";
const std::string doc_manage_tt = "Target type";
const std::string doc_manage_l = "Threshold level";
const std::string doc_manage_a = "Input channel ID";
const std::string doc_manage_c = "ThresholdCalculation ON/OFF (1/0)";
const std::string doc_manage_s = "Threshold Sigma";
const std::string doc_manage_f = "Frequency";
const std::string doc_manage_v = "Velocity";
const std::string doc_manage_st = "Stop option";
const std::string doc_manage_cm = "Estimation method";
const std::string doc_manage_miil = "Min. rel. input level";
const std::string doc_manage_mail = "Max. rel. input level";
const std::string doc_manage_ml = "Stop level (0 to 1)";
const std::string doc_manage_mia = "Min. radius";
const std::string doc_manage_maa = "Max. radius";
const std::string doc_manage_sp = "Speed factor";
const std::string doc_manage_mdc = "Max. dir. changes";
const std::string doc_manage_spo = "Speed offset";

//Header
const std::string doc_application_name = "Fast Alignment Sample Application\n";
const std::string doc_company_name = "Physik Instrumente (PI) SE & Co. KG\n";

const std::string doc_current_controller = "Current connection: ";
const std::string doc_error_status = "Status: ";

//routines
const std::string doc_routines_running = "Current routines running: ";
const std::string doc_routines_running_refresh = "(1) Refresh\n";
const std::string doc_routines_running_stop = "(2) Stop\n";
const std::string doc_routines_running_results = "(3) Show results\n";
const std::string doc_routines_running_savedata = "(4) Save data\n";
const std::string doc_routines_running_showdata = "(5) Visualize data\n";

//Move
const std::string doc_move_axes_absolute = "(1) Move absolute\n";
const std::string doc_move_axes_relative = "(2) Move relative\n";
const std::string doc_axis_to_move = "Axis to move ";
const std::string doc_step_size = "Step size ";
const std::string doc_step_neg = "(1) Negative step\n";
const std::string doc_step_pos = "(2) Positive step\n";

// ----------------------------------------------------------------------------------------------------------
unsigned int GetUserInput (int& MenuSelector);
std::string GetUserInput (int& MenuSelector, std::vector<int>& l_iScanVector);
void Header (int ID, const ConnectMap& Connections);
void MainMenue (int& MenuSelector);
void SelectController (int* ID, int& MenuSelector, const ConnectMap& Connections);
void AddEditRoutine (int ID, int& MenuSelector, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs, const ConnectMap& Connections);
void StartSingleRoutineSubMenu (int ID, int& MenuSelector, FDRScan& FDR, FDGScan& FDG, FDLScan& FDL, const ConnectMap& Connections);
void StartMultipleRoutineSubMenu (int ID, int& MenuSelector, const std::string& ScanRoutineNames, const ConnectMap& Connections);
void StartMultipleRoutine (int ID, int& MenuSelector, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs, const ConnectMap& Connections);
void StartSingleRoutineDR (int ID, int& MenuSelector, FDRScans& FDRs, FDGScans& FDGs, FDLScans& FDLs, const ConnectMap& Connections);
void ShowAnalogValues (int ID, int& MenuSelector);
void MoveAxes (int ID, int& MenuSelector, FDRScans& UNUSED(FDRs), FDGScans& UNUSED(FDGs), FDLScans& UNUSED(FDLs));
void ShowAxesPosition (const std::map <std::string, double>& l_AxesPositions);
void ShowAvailableAxes (const std::map <std::string, double>& l_AxesPositions);
