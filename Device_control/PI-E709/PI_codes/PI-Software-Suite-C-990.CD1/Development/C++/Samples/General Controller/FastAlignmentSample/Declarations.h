#pragma once

#include "PI_GCS2_DLL.h" // Include header of PI GCS 2 DLL

#include <map>
#include <string>
#include <vector>
#include "conio.h"
#include "stdio.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>


//key-definitions
struct ConfKeys 
{
    enum EConfKeys
    {
        eSystemName         = 1,
        eCommunicationType  = 2,
        eComPortBaudrate    = 3,
        eComPortNr          = 4,
        eIPAdress           = 5,
        eIPPort             = 6,
    };
};

struct ComType 
{
    enum EComType
    {
        eTCPIP      = 1,
        eUSB        = 2,
        eRS232      = 3,
    };
};

//Scan-definitions
struct ScanType
{
    enum EScanType
    {
        eFDR = 1,
        eFDG = 2,
        eFDL = 3,
    };
};

struct FDRScan
{
    FDRScan ()
        :ScanAxisRange    (0.0),
        StepAxisRange     (0.0),
        ThresholdLevel    (0.0),
        InputChannel      (0),
        Frequency         (0.0),
        Velocity          (0.0),
        ScanAxisMidPos    (0.0),
        StepAxisMidPos    (0.0),
        TargetType        (0),
        CentroidMethod    (0),
        MinRelInputLevel  (0.0),
        MaxRelInputLevel  (0.0),
        StopPosition      (0)
    {
    }
        std::string     ScanRoutineName;
        std::string     ScanAxis;
        double          ScanAxisRange;
        std::string     StepAxis;
        double          StepAxisRange;
        double          ThresholdLevel;
        unsigned int    InputChannel;
        double          Frequency;
        double          Velocity;
        double          ScanAxisMidPos;
        double          StepAxisMidPos;
        unsigned int    TargetType;
        unsigned int    CentroidMethod;
        double          MinRelInputLevel;
        double          MaxRelInputLevel;
        unsigned int    StopPosition;
};


struct FDGScan
{
    FDGScan ()
        :MinimumLevel       (0.0),
        InputChannel        (0),
        Frequency           (0.0),
        Velocity            (0.0),
        MinAmplitude        (0.0),
        MaxAmplitude        (0.0),
        SpeedFactor         (0.0),
        SpeedOffset         (0.0),
        MaxDirectionChanges (0)
    {
    }
        std::string     ScanRoutineName;
        std::string     ScanAxis;
        std::string     StepAxis;
        double          MinimumLevel;
        unsigned int    InputChannel;
        double          Frequency;
        double          Velocity;
        double          MinAmplitude;
        double          MaxAmplitude;
        double          SpeedFactor;
        double          SpeedOffset;
        unsigned int     MaxDirectionChanges;
};

struct FDLScan
{
    FDLScan()
        :ScanAxisRange(0.0),
        StepAxisRange(0.0),
        ThresholdSigma(0),
        ThresholdCalculation(0),
        InputChannel(0),
        MaximumSearchTime(0.0),
        ScanAxisMidPos(0.0),
        StepAxisMidPos(0.0),
        ScanAxisCalcPos(0.0),
        StepAxisCalcPos(0.0)    
    {
    }
    std::string     ScanRoutineName;
    std::string     ScanAxis;
    double          ScanAxisRange;
    std::string     StepAxis;
    double          StepAxisRange;
    unsigned int    ThresholdSigma;
    bool            ThresholdCalculation;
    unsigned int    InputChannel;
    double          MaximumSearchTime;
    double          ScanAxisMidPos;
    double          StepAxisMidPos;
    double          ScanAxisCalcPos;
    double          StepAxisCalcPos;
};

#define UNUSED(x)

using IniConfMap = std::map <int, std::string>;
using IniConfs = std::map <std::string, IniConfMap>;

using StringScanMap = std::map <std::string, std::string>;
using StringScans = std::map <std::string, StringScanMap>;

using FDRMap = std::map <int, FDRScan>;
using FDRScans = std::map <std::string, FDRMap>;

using FDGMap = std::map <int, FDGScan>;
using FDGScans = std::map <std::string, FDGMap>;

using FDLMap = std::map <int, FDLScan>;
using FDLScans = std::map <std::string, FDLMap>;

using ConnectMap = std::map <int, std::string>;
using ScanNameMap = std::vector <std::string>;
