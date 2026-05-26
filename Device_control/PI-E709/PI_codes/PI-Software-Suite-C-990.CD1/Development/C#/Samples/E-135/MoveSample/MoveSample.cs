/** <copyright file="MoveSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
 *  (c)2015-2020 Physik Instrumente (PI) SE & Co. KG
 *  Software products that are provided by PI are subject to the General Software License Agreement of
 *  Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components.
 *  For more information, please read the General Software License Agreement and the Third Party Software Note linked below.
 *
 *  General Software License Agreement :
 *  http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
 *  Third Party Software Note :
 *  http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf
 *  </copyright>*/

/**<remarks>
 *  !! MOTION WARNING !!
 *
 *  This sample connects to a E-135 controller using a connection dialog. It shows how to execute an open-loop motion on
 *  two channels (channels 1 and 2) using three different methods (POL, SVA and OSM/OSN?). The controller and the positioners
 *  have to be initialized with PIMikroMove before starting this sample.
 *
 *</remarks>*/

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using PI;

namespace MoveSample
{
    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    /**<summary>
     *  This class executes an open-loop motion for a E-872 for all connected and configured channels.
     *  </summary>*/
    public class MoveSample
    {
        #region fields

        internal int ControllerId = -1;
        int[] channels = {1,2};
        string[] channelStrings = {"1","2"};
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_TRUE = 1;
        private const int PI_FALSE = 0;
        private const int NUMBEROFSTEPS = 2;

        #endregion

        #region public methods

        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();
                PrintNamesOfConnectedChannels();



                MoveChannelsUsingPOL();
                MoveChannelsUsingSVA();
                MoveChannelsUsingOSM();
            }
            catch (GcsCommandError e)
            {
                PrintErrorMessage(e);
            }
            finally
            {
                DisconnectController();
            }
        }

        #endregion

        #region private methods

        private void PrintControllerIdentification()
        {
            var controllerIdentification = new StringBuilder(1024);

            if (GCS2.qIDN(ControllerId, controllerIdentification, controllerIdentification.Capacity) ==
                PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("qIDN failed. Exiting."));
            }

            Console.WriteLine("qIDN returned: " + controllerIdentification);
        }

        private void OpenConnection(ConnectionType type, string hostName = "localhost")
        {
            switch (type)
            {
                case ConnectionType.Dialog:
                    ControllerId = GCS2.InterfaceSetupDlg("");
                    break;
                case ConnectionType.Rs232:
                    ControllerId = GCS2.ConnectRS232(1, 115200);
                    break;
                case ConnectionType.Tcpip:
                    ControllerId = GCS2.ConnectTCPIP(hostName, 50000);
                    break;
                case ConnectionType.Usb:
                {
                    var connectedUsbController = new StringBuilder(1024);
                    var noDevices = GCS2.EnumerateUSB(connectedUsbController, 1024, "");

                    Console.WriteLine("Found: " + noDevices + " USB controllers, connecting to the first one:");
                    Console.Write(connectedUsbController + "\n");

                    ControllerId = GCS2.ConnectUSB(
                        connectedUsbController
                            .ToString()
                            .Split(new[] {Environment.NewLine}, StringSplitOptions.None)
                            [0]);
                    break;
                }
                default:
                    throw new ArgumentOutOfRangeException(nameof(type), type, null);
            }

            if (0 <= ControllerId) return;

            var connType = type.Equals(ConnectionType.Dialog) ? type.ToString() : type.ToString().ToUpper();
            throw new GcsCommandError("Connection to " + connType + " failed. Exiting.");
        }

        private void DisconnectController()
        {
            if (ControllerId >= 0)
            {
                GCS2.CloseConnection(ControllerId);
                ControllerId = -1;
            }

            Console.WriteLine("Close connection.");
        }

        private void PrintNamesOfConnectedChannels()
        {
            StringBuilder axesBuffer = new StringBuilder(1024);

            if (PI.GCS2.qSAI(ControllerId, axesBuffer, axesBuffer.Capacity) == 0)
            {
                throw (new GcsCommandError("Could not query names from connected channels."));
            }

            string[] tmpString;
            tmpString = axesBuffer.ToString().Replace(" \n", ";").Replace("\n", "").Split(';');
            var listChannels = new List<int>();
            for (int index = 0; index < tmpString.Count(); ++index)
            {
                listChannels.Add(index + 1);
                Console.WriteLine("Name of channel number " + index + ": " + tmpString[index].Trim());
            }

            Console.WriteLine();
        }

        private void MoveChannelsUsingPOL()
        {
            // only one channel at a time can be moved.
            for (int index = 0; index < channels.Count(); ++index)
            {
                Console.WriteLine("Move channel " + channels[index] + " to the maximum of its travel range using POL command");
                int [] maximumTravelRange = {1};
                if ( PI.GCS2.POL (ControllerId, channels[index].ToString(), maximumTravelRange)== PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("Could not move to maximum of travel range."));
                }

                WaitForMotionDone(index, false);

                Console.WriteLine("Move channel " + channels[index] + " to the minimum of its travel range using POL command");
                int [] minimumTravelRange = {0};
                if ( PI.GCS2.POL (ControllerId, channels[index].ToString(), maximumTravelRange)== PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("Could not move to minimum of travel range."));
                }

                WaitForMotionDone(index, false);


                Console.WriteLine("Motion completed for channel " + channelStrings[index] + ".\n");
            }
        }

        private void MoveChannelsUsingSVA()
        {
            // only one channel at a time can be moved.
            for (int index = 0; index < channels.Count(); ++index)
            {
                double [] maximumTravelRange = {30.0};
                Console.WriteLine("Move channel " + channels[index] + " to "+ maximumTravelRange[0] + "% of its travel range using SVA command");
                if ( PI.GCS2.SVA (ControllerId, channels[index].ToString(), maximumTravelRange)== PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("Could not move to specified target within travel range."));
                }

                WaitForMotionDone(index, false);

                Console.WriteLine("Motion completed for channel " + channelStrings[index] + ".\n");
            }
        }

        private void MoveChannelsUsingOSM()
        {
            int[] numberOfSteps = {NUMBEROFSTEPS};

            // execute open-loop motion
            // only one channel at a time can be moved.
            Console.WriteLine("Execute open-loop motion one channel at a time.");

            for (int index = 0; index < channels.Count(); ++index)
            {
                Console.WriteLine("Executing motion on channel " + channels[index] + " (" + NUMBEROFSTEPS + " steps)");

                int[] currentChannel = {channels[index]};

                if (PI.GCS2.OSM(ControllerId, currentChannel, numberOfSteps, 1) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("Could not command steps."));
                }

                WaitForMotionDone(index, true);

                Console.WriteLine("Motion completed for channel " + channelStrings[index] + "\n");
            }
        }

        private void WaitForMotionDone(int currentIndex, bool displayRemainingSteps)
        {
            int[] isMoving = {PI_TRUE};

            while (isMoving[0] == PI_TRUE)
            {
                if (GCS2.IsMoving(ControllerId, channelStrings[currentIndex], isMoving) == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError("Unable to retrieve motion state.");
                }

                if (displayRemainingSteps)
                {
                    DisplayRemainingSteps(currentIndex);
                }

                System.Threading.Thread.Sleep(1000);
            }
        }

        private void DisplayRemainingSteps(int currentIndex)
        {
            int[] remainingSteps = {0};
            int[] currentChannel = {channels[currentIndex]};

            if (PI.GCS2.qOSN(ControllerId, currentChannel, remainingSteps, 1) == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("Unable to query remaining steps.\n"));
            }

            Console.WriteLine("Remaining steps on " + channels[currentIndex] + ": " + remainingSteps[0]);
        }

        private void PrintErrorMessage(GcsCommandError e)
        {
            var errorCode = GCS2.GetError(ControllerId);
            var errorMessage = new StringBuilder(1024);
            GCS2.TranslateError(errorCode, errorMessage, errorMessage.Capacity);

            Console.WriteLine("ERROR: " + e.Message + "\n - Reported error code: " + errorCode + ": \n\t" +
                              errorMessage);
        }

        #endregion
    }

    internal class GcsCommandError : Exception
    {
        public GcsCommandError(string message)
            : base(message)
        {
        }
    }
}