#include "GCSMethods.h"
#include "Helper.h"
#include "Menu.h"


int main ()
{
    int ID = -1;
    int MenuSelector = 0;

    //get Ini-file data
    IniConfs l_IniConfs;
    StringScans l_StringScans;
    if (!GetIniFile ("FA.ini", l_IniConfs, l_StringScans))
    {
        std::cout << "GetIniFile() == FALSE" << "\n";
        std::cout << "Press any key to exit" << "\n";
        std::cin.get ();
        return 0;
    }

    //Convert INI data to struct
    FDRScans l_FDRs;
    FDGScans l_FDGs;
    FDLScans l_FDLs;

    ConvertStringScansToFDRScans (l_StringScans, l_FDRs);
    ConvertStringScansToFDGScans (l_StringScans, l_FDGs);
    ConvertStringScansToFDLScans (l_StringScans, l_FDLs);

    //Connect
    ConnectMap Connections;
    if (!ControllerConnect (l_IniConfs, &ID, Connections))
    {
        std::cout << "ControllerConnect() == FALSE" << "\n";
    }

    //StringScans (INI) => Controller
    SetControllerFDRScans (Connections, l_FDRs);
    SetControllerFDGScans (Connections, l_FDGs);
    SetControllerFDLScans (Connections, l_FDLs);

    //Readback FDR-/FDGScans (merge INI-data scans + controller scans)
    l_FDRs.clear ();
    l_FDGs.clear();
    l_FDLs.clear();
    GetControllerFDRScans (Connections, l_FDRs);
    GetControllerFDGScans (Connections, l_FDGs);
    GetControllerFDLScans (Connections, l_FDLs);

    //Menue
    while (0 <= MenuSelector)
    {
        Header (ID, Connections);
        switch (MenuSelector)
        {
            case (Menu::eMainMenu):
                MainMenue (MenuSelector);
                break;
            case (Menu::eSelectController):
                SelectController (&ID, MenuSelector, Connections);
                break;
            case (Menu::eAddEditRoutine):
                AddEditRoutine (ID, MenuSelector, l_FDRs, l_FDGs, l_FDLs, Connections);
                break;
            case (Menu::eStartSingleRoutineDR):
                StartSingleRoutineDR (ID, MenuSelector, l_FDRs, l_FDGs, l_FDLs, Connections);
                break;
            case (Menu::eStartMultipleRoutine):
                StartMultipleRoutine (ID, MenuSelector, l_FDRs, l_FDGs, l_FDLs, Connections);
                break;
            case (Menu::eShowAnalogValues):
                ShowAnalogValues (ID, MenuSelector);
                break;
            case (Menu::eMoveAxes):
                MoveAxes (ID, MenuSelector, l_FDRs, l_FDGs, l_FDLs);
                break;

            default:
                MenuSelector = 0;
        }   
    }
    return 0;
}

