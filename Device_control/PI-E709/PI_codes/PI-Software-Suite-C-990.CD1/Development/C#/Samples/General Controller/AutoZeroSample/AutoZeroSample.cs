/**  <copyright file="AutoZero.cs" company="Physik Instrumente (PI) SE & Co. KG">
  *  (c)2015-2020 Physik Instrumente (PI) SE & Co. KG
  *  Software products that are provided by PI are subject to the General Software License Agreement of
  *  Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components.
  *  For more information, please read the General Software License Agreement and the Third Party Software Note linked below.
  *
  *  General Software License Agreement :
  *  http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
  *  Third Party Software Note :
  *  http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf
  *  </copyright>* /

/** <remarks>
 *  !! MOTION WARNING !!
 *
 *  This sample connects to a controller (i.e. E-727) via connection dialog
 *  and which supports auto zero.
 *  The controller will then do a auto zero of the stage.
 *  The controller and the stage has to be initialized with PIMikroMove before starting this sample.
 *  </remarks>*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using PI;

namespace AutoZeroSample
{
    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    public struct AxisState
    {
        public string Axis;
        public int State;

        public AxisState(string axis, int state)
        {
            Axis = axis;
            State = state;
        }
    }

    /**  <summary>
      *  This executes a auto zero for controller which support this feature i.e E-727.
      *  Call the Run-method to execute this sample.
      *  </summary>*/
    public class AutoZeroSample
    {
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

        private static readonly ReferenceSettings Settings
            = new ReferenceSettings("1", new []{PI_FALSE});
        #endregion controller specific configuration

        #region fields
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_TRUE  = 1;
        private const int PI_FALSE = 0;
        internal int ControllerId = -1;
        #endregion fields

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                SetServoState(Settings.AxisIdentifier, Settings.ServoState);
                AutoZeroStage(Settings.AxisIdentifier);
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
        private void AutoZeroStage(string axisIdentifier)
        {
            var axisIdentifierSplit = axisIdentifier.Split(' ');
            var autoZeroState = Enumerable.Repeat(PI_TRUE, axisIdentifierSplit.Length).ToArray();

            // check auto zero state
            if ( GCS2.qATZ(ControllerId, axisIdentifier, autoZeroState) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to check auto zero state.");
            }

            var axisEnumerableStates = GetAxisEnumerableStates(axisIdentifierSplit, autoZeroState, PI_TRUE);

            // start auto zero procedure
            if ( axisEnumerableStates.Any(axisState
                => GCS2.ATZ(ControllerId, axisState.Axis, null, new []{axisState.State})
                   == PI_RESULT_FAILURE))
            {
                throw new GcsCommandError("Unable to start auto zero.");
            }

            // Wait until the auto zero is done.
            var bRefDone = PI_FALSE;

            Console.WriteLine("Waiting for auto zero to be finished.");
            while ( bRefDone == PI_FALSE )
            {
                if (GCS2.IsControllerReady(ControllerId,ref bRefDone ) == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError("Unable to retrieve controller ready state.");
                }
            }

            if ( GCS2.qATZ(ControllerId, axisIdentifier, autoZeroState) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to check auto zero state.");
            }

            axisEnumerableStates = GetAxisEnumerableStates(axisIdentifierSplit, autoZeroState, PI_TRUE);

            foreach (var axisStateTrue in axisEnumerableStates)
            {
                Console.WriteLine("Settings " + axisStateTrue.Axis + " has done auto zero.");
            }

            axisEnumerableStates = GetAxisEnumerableStates(axisIdentifierSplit, autoZeroState, PI_TRUE);

            foreach (var axisStateFalse in axisEnumerableStates)
            {
                Console.WriteLine("Settings " + axisStateFalse.Axis + " has not done auto zero.");
            }

        }

        private IEnumerable<AxisState> GetAxisEnumerableStates(string[] axisIdentifierSplit, IEnumerable<int> autoZeroState, int filterState)
        {
            return from state in axisIdentifierSplit
                    .Zip(autoZeroState, (axis, state) => new AxisState(axis, state))
                where state.State == filterState
                select state;
        }

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state ) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("SVO failed. Exiting");
            }
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
            Console.WriteLine("ERROR: " + e.Message + " - Reported error code: " + errorCode + ": " + errorMessage);
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
