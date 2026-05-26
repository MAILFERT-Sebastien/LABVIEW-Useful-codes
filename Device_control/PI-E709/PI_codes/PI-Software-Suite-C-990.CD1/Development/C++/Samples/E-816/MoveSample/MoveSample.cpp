// E816_DLL_CONSOLE_SAMPLE.cpp : Defines the entry point for the console application.
//

#ifdef WIN32
#include "stdafx.h"
#include "conio.h" // for getch()
#include <Windows.h>
#else
#include <stdlib.h>
#include <stdio.h>
#endif
#include "E816_DLL.h"

#define SINGLE_AXIS 1
#define INTERFACE_RS232

int main(int argc, char* argv[])
{
    // try to open communication
    int ID = -1;
#ifdef INTERFACE_RS232
    int port = 1;
    int baud = 115200;
    if(argc == 2)
    {
        port = atoi(argv[1]);
        printf("\n You selected port %d via command line\n",port);
    }
    ID = E816_ConnectRS232(port, baud);
    printf("\n\nE-816 on COM%d, %d Baud\n", port, baud);
#endif
#ifdef INTERFACE_SETUP_DLG
    ID = E816_InterfaceSetupDlg(NULL);
#endif
#ifdef INTERFACE_USB
    char szDevices[1000];
    int nrDevices = E816_EnumerateUSB(szDevices, 999, NULL);
    if (nrDevices<=0)
    {
        printf("No devices connected to USB");
        goto exit;
    }
    char* p = strtok(szDevices, "\n");
    printf("Found %d devices, connecting to first: \"%s\"\n", nrDevices, szDevices);
    ID = E816_ConnectUSB(szDevices);

#endif
    if (ID >= 0)
    {
        char sIDN[1024];
        char* sspa = "AAAAAAAAAA";
        double spa[10];
        int spacmd[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int svo[2] = {1, 1};
        char axes[10];

        if (!E816_qIDN(ID, sIDN, 1023)) goto error;

        printf("ID of E816 = \"%s\"\n\n",sIDN);

        char sSAI[1024];
        if (!E816_qSAI(ID, sSAI, 1023)) goto error;
        printf("SAI?: \"%s\"\n\n",sSAI);

        printf("SPA TEST ");
        if (!E816_qSPA(ID, sspa, spacmd, spa) ) goto error;
        printf("SPA for A:");
        for (int i=0; i<10; i++)
            printf("   %d:%f\n", i+1, spa[i]);

        printf("\nMOV-qPOS Test ");

#ifdef SINGLE_AXIS
        sprintf (axes, "B");
#else
        sprintf (axes, "BC");
#endif
        if (! E816_SVO(ID, axes, svo) ) goto error;


#ifdef SINGLE_AXIS
        printf("\"SVO B%d\"\n", svo[0]);
#else
        printf("\"SVO B%d C%d\"\n", svo[0], svo[1]);
#endif

        // Test MOV - qPOS several times
        for (int i=0; i<10; i++)
        {
            //if (!(i%10)) printf(".");

            double val[2];
            val[0] = (i%2) ? 1.0 : 10.0;
            val[1] = (i%2) ? 10.0 : 1.0;

            if(! E816_MOV(ID, axes, val) ) goto error1;
            //Sleep(5);
#ifdef SINGLE_AXIS
            printf("\"MOV B%f\"\n", val[0]);
#else
            printf("\"MOV B%f C%f\"\n", val[0], val[1]);
#endif

            double pos[2];
            for (int j=0; j<10; j++)
            {
                if (! E816_qPOS(ID, axes, pos) ) goto error;
#ifdef SINGLE_AXIS
                printf("Current position of E-816: ");
                printf("B = %8.4f\n", pos[0]);
#else
                printf("Current position of E-816: ");
                printf("B = %8.4f   C = %8.4f\n", pos[0], pos[1]);
#endif
            }
        }

error1:
        printf("MOV ");
error:
        int err = E816_GetError(ID);
        char errMsg[1024];
        E816_TranslateError(err, errMsg, 1023);
        printf("Error %d: \"%s\"", err, errMsg);
        printf("Closing connection to E-816");
        E816_CloseConnection(ID);
    }
    else
    {
        printf ("Cannot connect to E-816\n");
    }
    printf("\n\n< ... any key to exit ... >\n");
#ifdef WIN32
    _getch();
#endif
    return 0;
}

