/** <copyright file="ReferenceSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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

/** <remarks></remarks>
 *  !! MOTION WARNING !!
 *
 *  This sample connects to a controller using the connection dialog and references the stage.
 *  The controller and the stage has to be initialized with PIMikroMove before starting this sample.
 *  </remarks>*/

namespace ReferenceSample
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using PI;

    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    /** <summary>
     *  This class runs the sample for a controller with the reference command FRF.
     *  Call the Run-method to execute this sample.
     *  Please choose the right configuration for your setup, there is one common configuration
     *  working with most of PI controllers and one configuration for the Hexapod controller C-887,
     *  and one for the E-712 Piezo controller.
     * </summary>*/
    public class ReferenceSample
    {
        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_TRUE  = 1;
        private const int PI_FALSE = 0;
        #endregion fields

        /// <remarks>
        /// select the reference configuration here: 'Common', 'E-712','C-887'
        /// </remarks>
        #region controller specific configuration
        public struct ReferenceSettings
        {
            internal string AxisIdentifier { get; }

            internal int[] ServoState { get; }

            public ReferenceSettings(string axisAxisIdentifier, int[] servoState)
            {
                AxisIdentifier = axisAxisIdentifier;
                ServoState = servoState;
            }
        }
        public static Dictionary<string, ReferenceSettings> ControllerReferenceSettings { get; }
            = new Dictionary<string, ReferenceSettings>
                {
                    {"Common", new ReferenceSettings("1", new []{PI_TRUE}) },
                    {"C-887", new ReferenceSettings("X", new []{PI_TRUE}) },
                    {"E-712", new ReferenceSettings("1", new []{PI_FALSE}) },
                };

        private static readonly ReferenceSettings Settings = ControllerReferenceSettings["Common"];
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                SetServoState(Settings.AxisIdentifier, Settings.ServoState);
                ReferenceStage(Settings.AxisIdentifier);
                SetServoState(Settings.AxisIdentifier, new [] {PI_FALSE});

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
        private void ReferenceStage(string axis)
        {
            // start referencing
            if ( GCS2.FRF(ControllerId, axis) == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("Unable to reference."));
            }

            // Wait until the reference move is done.
            var bRefDone = PI_FALSE;

            Console.WriteLine("Waiting for referencing to be finished.");
            while ( bRefDone == PI_FALSE )
            {
                if (GCS2.IsControllerReady(ControllerId,ref bRefDone ) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("Unable to retrieve controller ready state."));
                }
            }

            int[] bReferenced = { PI_FALSE };

            // check if axis was referenced
            if (GCS2.qFRF(ControllerId, axis, bReferenced) == PI_RESULT_FAILURE )
            {
                throw (new GcsCommandError("Unable to get referenced state."));
            }

            if ( bReferenced[0] == PI_TRUE )
            {
                Console.WriteLine("Settings " + axis + " is referenced.");
            }
            else
            {
                Console.WriteLine("Settings " + axis + " is not referenced.");
            }
        }

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state ) == PI_RESULT_FAILURE )
            {
                throw (new GcsCommandError("SVO failed. Exiting"));
            }
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
        public GcsCommandError( string message )
            : base(message)
        {
        }
    }
}