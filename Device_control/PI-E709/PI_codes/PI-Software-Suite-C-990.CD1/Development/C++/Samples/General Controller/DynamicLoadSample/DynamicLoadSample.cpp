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

    This sample shows how to load the PI_GCS2_DLL and its methods dynamically, 
    connect to a controller using a connection dialog and reads the device identification.
    
**/

#include "DynamicLoadSample.h"


int main ()
{
    int iID = -1;

    try
    {
        HINSTANCE hPI_Dll = OpenDLL ();
        std::string sFuncName;

        // get function pointers
        sFuncName = GCS_FUNC_PREFIX;
        sFuncName.append ("InterfaceSetupDlg");
        pInterfaceSetupDlg = (PFPI_InterfaceSetupDlg)GetProcAddress (hPI_Dll, (LPCSTR)sFuncName.c_str ());

        if (!pInterfaceSetupDlg)
        {
            std::stringstream stream;
            stream << "Loading function " << sFuncName << " failed.";
            throw std::runtime_error (stream.str ());
        }

        sFuncName = GCS_FUNC_PREFIX;
        sFuncName.append ("qIDN");
        pqIDN = (PFPI_qIDN)GetProcAddress (hPI_Dll, (LPCSTR)sFuncName.c_str ());

        if (!pqIDN)
        {
            std::stringstream stream;
            stream << "Loading function " << sFuncName << " failed.";
            throw std::runtime_error (stream.str ());
        }

        sFuncName = GCS_FUNC_PREFIX;
        sFuncName.append ("CloseConnection");
        pCloseConnection = (PFPI_CloseConnection)GetProcAddress (hPI_Dll, (LPCSTR)sFuncName.c_str ());

        if (!pCloseConnection)
        {
            std::stringstream stream;
            stream << "Loading function " << sFuncName << " failed.";
            throw std::runtime_error (stream.str ());
        }

        sFuncName = GCS_FUNC_PREFIX;
        sFuncName.append ("GetError");
        pGetError = (PFPI_GetError)GetProcAddress (hPI_Dll, (LPCSTR)sFuncName.c_str ());

        if (!pGetError)
        {
            std::stringstream stream;
            stream << "Loading function " << sFuncName << " failed.";
            throw std::runtime_error (stream.str ());
        }

        sFuncName = GCS_FUNC_PREFIX;
        sFuncName.append ("TranslateError");
        pTranslateError = (PFPI_TranslateError)GetProcAddress (hPI_Dll, (LPCSTR)sFuncName.c_str ());

        if (!pTranslateError)
        {
            std::stringstream stream;
            stream << "Loading function " << sFuncName << " failed.";
            throw std::runtime_error (stream.str ());
        }

        std::cout << "All function pointers are loaded." << std::endl;

        // connect to controller
        iID = OpenConnection (Dialog);

        char szIDN[100];

        if (!pqIDN (iID, szIDN, 99))
        {
            throw std::runtime_error ("IDN failed.");
        }

        std::cout << "Successfully connected to " << szIDN << std::endl;
        pCloseConnection (iID);
        std::cout << "Closed connection." << std::endl;
        FreeLibrary (hPI_Dll);
    }
    catch (std::runtime_error e)
    {
        CloseConnectionWithComment (iID, e.what ());
        return 1;
    }

    return 0;
}

HINSTANCE OpenDLL ()
{
    std::string dllPath = GetPathToDLL ();

    std::cout << "Open " << dllPath << " dynamically" << std::endl;

    // load DLL
    HINSTANCE hPI_Dll = LoadLibrary (dllPath.c_str ());

    if (!hPI_Dll)
    {
        throw std::runtime_error ("LoadLibrary(\"" + dllPath + "\") failed.");
    }

    std::cout << "LoadLibrary(\"" << dllPath << "\") successful" << std::endl;

    return hPI_Dll;
}


int OpenConnection (ConnectionType type, std::string hostName)
{
    int iD = 0;
    std::string connType;

    switch (type)
    {
        case Dialog:
            iD = pInterfaceSetupDlg ("");
            connType = "Dialog";
            break;
    }

    if (iD < 0)
    {
        ReportError (iD);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return iD;
}

void CloseConnectionWithComment (int iD, std::string comment)
{
    std::cout << comment << "\n";

    ReportError (iD);

    if (iD >= 0)
    {
        pCloseConnection (iD);
    }
}

std::string GetPathToDLL ()
{
    PWSTR szProgramDataPath;

    // get program data path
    HRESULT result = SHGetKnownFolderPath (FOLDERID_ProgramData, KF_FLAG_DEFAULT, NULL, &szProgramDataPath);

    if (result != S_OK)
    {
        std::cout << "Error getting ProgramData path" << std::endl;
        return "";
    }

    // convert strings
    std::wstring wsProgramDataPath (szProgramDataPath);
    std::string sProgramDataPath (wsProgramDataPath.begin (), wsProgramDataPath.end ());
    CoTaskMemFree (static_cast<void*>(szProgramDataPath));

    // get DLL path
    std::string szGCSDLLName = sProgramDataPath
        .append (GCS_TRANSLATOR_SUB_DIRECTORY)
        .append (GCS_DLLNAME);

    return szGCSDLLName;
}

void ReportError (int iD)
{
    int err = pGetError (iD);
    char szErrMsg[300];

    if (pTranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}
