/** <copyright file="ParameterSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 *  This sample connects a controller using the connection dialog.
 *  The value of the parameter for the maximum velocity is read and
 *  the value is modified and set as velocity of the axis.
 *  The stage has to be referenced before starting this sample.
 *
 *</remarks>*/

using System;
using System.Text;
using PI;

namespace ParameterSample
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

    public class ParameterSample
    {
        #region controller specific configuration

        public readonly struct Parameter
        {
            internal string ContainerUnitId { get; }
            internal string FunctionUnitId { get; }
            internal string ParameterId { get; }

            public Parameter (string containerUnitId, string functionUnitId, string parameterId)
            {
                ContainerUnitId = containerUnitId;
                FunctionUnitId  = functionUnitId;
                ParameterId     = parameterId;
            }
        }

        #endregion

        #region fields

        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_TRUE = 1;
        private const int PI_FALSE = 0;

        #endregion

        #region public methods

        public void Run ()
        {
            try
            {
                OpenConnection (ConnectionType.Dialog);
                PrintControllerIdentification ();

                var paramVelocityAxis1    = new Parameter ("AXIS_1", "TRAJ_1", "0x104");
                var paramMaxVelocityAxis1 = new Parameter ("AXIS_1", "TRAJ_1", "0x105");
                var maxVelocity           = ReadParameterValue (ControllerId, "RAM", paramMaxVelocityAxis1);
                Console.WriteLine ("Maximum velocity of AXIS_1 is " + maxVelocity);

                var velocity = maxVelocity / 2;
                WriteParameterValue (ControllerId, "RAM", paramVelocityAxis1, velocity);
                Console.WriteLine ("Set velocity of AXIS_1 to " + velocity);
            }
            catch (GcsCommandError e)
            {
                PrintErrorMessage (e);
            }
            finally
            {
                DisconnectController();
                Console.WriteLine("Press any key to stop...");
                Console.ReadKey();
            }
        }

        #endregion

        #region private methods

        private void PrintControllerIdentification ()
        {
            var controllerIdentification = new StringBuilder (1024);

            if (GCS2.qIDN (ControllerId, controllerIdentification, controllerIdentification.Capacity) ==
                PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError ("qIDN failed. Exiting."));
            }

            Console.WriteLine ("qIDN returned: " + controllerIdentification);
        }

        private void OpenConnection (ConnectionType type, string hostName = "localhost")
        {
            Console.WriteLine ("-- Try to Connect --");
            switch (type)
            {
                case ConnectionType.Dialog:
                    ControllerId = GCS2.InterfaceSetupDlg ("");
                    break;
                case ConnectionType.Rs232:
                    ControllerId = GCS2.ConnectRS232 (1, 115200);
                    break;
                case ConnectionType.Tcpip:
                    ControllerId = GCS2.ConnectTCPIP (hostName, 50000);
                    break;
                case ConnectionType.Usb:
                {
                    var connectedUsbController = new StringBuilder (1024);
                    var noDevices              = GCS2.EnumerateUSB (connectedUsbController, 1024, "");

                    Console.WriteLine ("Found: " + noDevices + " USB controllers, connecting to the first one:");
                    Console.Write (connectedUsbController + "\n");

                    ControllerId = GCS2.ConnectUSB (
                        connectedUsbController
                            .ToString ()
                            .Split (new[] {Environment.NewLine}, StringSplitOptions.None)
                            [0]);
                    break;
                }
                default:
                    throw new ArgumentOutOfRangeException (nameof (type), type, null);
            }

            if (0 <= ControllerId) return;

            var connType = type.Equals (ConnectionType.Dialog) ? type.ToString () : type.ToString ().ToUpper ();
            throw new GcsCommandError ("Connection to " + connType + " failed. Exiting.");
        }

        private void DisconnectController ()
        {
            if (ControllerId >= 0)
            {
                GCS2.CloseConnection (ControllerId);
                ControllerId = -1;
            }

            Console.WriteLine ("Close connection.");
        }

        private void PrintErrorMessage (GcsCommandError e)
        {
            var errorCode    = GCS2.GetError (ControllerId);
            var errorMessage = new StringBuilder (1024);
            GCS2.TranslateError (errorCode, errorMessage, errorMessage.Capacity);
            Console.WriteLine (
                "ERROR: " + e.Message + "\n - Reported error code: " + errorCode + ": \n\t" +
                errorMessage);
        }

        private double ReadParameterValue (int iD, string memoryType, Parameter parameter)
        {
            double value = 0.0;
            if (GCS2.qSPV_Double (
                iD,
                memoryType,
                parameter.ContainerUnitId,
                parameter.FunctionUnitId,
                parameter.ParameterId,
                ref value) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Could not read parameter value.");
            }

            return value;
        }

        private void WriteParameterValue (int iD, string memoryType, Parameter parameter, double value)
        {
            if (GCS2.SPV_Double (
                iD,
                memoryType,
                parameter.ContainerUnitId,
                parameter.FunctionUnitId,
                parameter.ParameterId,
                value) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Could not write parameter value.");
            }
        }

        #endregion
    }
}
