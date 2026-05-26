/** <copyright file="ReferenceSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
 *  (c)2015-2021 Physik Instrumente (PI) SE & Co. KG
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
 *  This sample connects to a controller using the connection dialog.
 *  Then it starts up the connected stage and references it.
 *</remarks>*/

using System;
using System.Text;
using System.Threading;
using PI;

namespace ReferenceSample
{
    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    internal class GcsCommandError : Exception
    {
        public GcsCommandError(string message)
            : base(message)
        {
        }
    }

    public class ReferenceSample
    {
        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_AXIS_ENABLE = 1;
        private const int PI_AXIS_REFERENCED = 1;
        #endregion

        #region public methods

        public void Run ()
        {
            try
            {
                OpenConnection (ConnectionType.Dialog);
                PrintControllerIdentification ();

                var axes = new[] {"AXIS_1", "AXIS_2"};

                foreach (var axis in axes)
                {
                    EnableAxis (axis);

                    if (!Reference (ControllerId, axis))
                    {
                        Console.WriteLine ($"Unable to reference axis {axis}.");
                    }
                }

            }
            catch (GcsCommandError e)
            {
                PrintErrorMessage (e);
            }
            catch (Exception e)
            {
                Console.WriteLine (e.Message);
            }
            finally
            {
                DisconnectController ();
                Console.WriteLine ("Press any key to stop...");
                Console.ReadKey ();
            }
        }

        #endregion

        #region private methods

        private void EnableAxis (string axis)
        {
            if (GCS2.EAX (ControllerId, axis, new[] {PI_AXIS_ENABLE}) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Could not enabled axis.");
            }
        }

        private bool Reference (int controllerId, string axis)
        {
            var referenceState = new[] {-1};
            if (GCS2.qFRF (controllerId, axis, referenceState) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to get referenced state.");
            }

            if (referenceState[0] == PI_AXIS_REFERENCED)
            {
                Console.WriteLine ($"Axis {axis} is already referenced.");
                return true;
            }

            if (GCS2.SAM(controllerId, axis, 0) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to set mode of operation.");
            }

            Console.WriteLine ($"Referencing axis {axis} ...");

            if (GCS2.FRF (controllerId, axis) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to reference.");
            }

            // Wait until the reference move is done.
            var isReady  = 0;
            var elapsed  = 0;
            var maxTries = 20;
            while (isReady != 1)
            {
                if (GCS2.IsControllerReady (controllerId, ref isReady) == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError ("Unable to get controller ready state.");
                }

                Thread.Sleep (1000);
                if (elapsed++ >= maxTries)
                {
                    throw new Exception ($"Unable to get controller ready state after {elapsed} tries.");
                }
            }

            // check if referencing was successful
            if (GCS2.qFRF (controllerId, axis, referenceState) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to get referenced state.");
            }

            if (referenceState[0] != PI_AXIS_REFERENCED)
            {
                return false;
            }

            Console.WriteLine ($"Successfully referenced axis {axis}.");

            return true;
        }

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
            Console.WriteLine("-- Try to Connect --");
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
                                .Split(new[] { Environment.NewLine }, StringSplitOptions.None)
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
}
