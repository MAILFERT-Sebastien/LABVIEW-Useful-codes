/** <copyright file="MoveSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 *  It shows how to determine min/max position limits, also how to do absolute and relative motions.
 *  The stage has to be referenced before starting this sample.
 *
 *</remarks>*/

using System;
using System.Text;
using System.Threading;
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
     *  This class executes reference move with EAX command for GCS2.1.
     *  Call Run-method for executing this sample.
     *  This is the configuration working with PI GCS2.1 controllers.
     * </summary>*/
    public class MoveSample
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
        private const int MODE_OF_OPERATION_OPEN_LOOP_POSITION = 1;
        private const int MODE_OF_OPERATION_CLOSED_LOOP_POSITION = 2;

        #endregion

        #region public methods
        public void Run ()
        {
            try
            {
                OpenConnection (ConnectionType.Dialog);
                PrintControllerIdentification ();

                const string sAxis = "AXIS_1";

                SetModeOfOperation (sAxis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);

                // get min/max position limits
                var minPosLimit = GetMinPositionLimit (sAxis);
                var maxPosLimit = GetMaxPositionLimit (sAxis);

                var range = maxPosLimit - minPosLimit;

                // do absolute motion to near minimum position
                var targetPos = minPosLimit + (range * 0.1);
                MoveToAbsoluteTarget (ControllerId, sAxis, targetPos);
                WaitForMotionDone (sAxis);
                Console.WriteLine ("Successfully moved near minimum position limit, to " + targetPos.ToString ("F3"));

                // do absolute motion to near maximum position
                targetPos = maxPosLimit - (range * 0.1);
                MoveToAbsoluteTarget (ControllerId, sAxis, targetPos);
                WaitForMotionDone (sAxis);
                Console.WriteLine ("Successfully moved near maximum position limit, to " + targetPos.ToString ("F3"));

                var offset = range * 0.5 * -1;
                RelativeMove (ControllerId, sAxis, offset);
                WaitForMotionDone (sAxis);
                Console.WriteLine ("Successfully executed relative move backward by " + offset.ToString ("F3"));

                offset = offset * -1;
                RelativeMove (ControllerId, sAxis, offset);
                WaitForMotionDone (sAxis);
                Console.WriteLine ("Successfully executed relative move forward by " + offset.ToString ("F3"));

                SetModeOfOperation (sAxis, MODE_OF_OPERATION_OPEN_LOOP_POSITION);
            }
            catch (GcsCommandError e)
            {
                PrintErrorMessage (e);
            }
            finally
            {
                DisconnectController ();
                Console.WriteLine("Press any key to stop...");
                Console.ReadKey();
            }
        }
        #endregion

        #region private methods

        private void MoveToAbsoluteTarget (int id, string axis, double targetPos)
        {
            if (GCS2.MOV (id, axis, new [] {targetPos}) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
            }
        }

        private void RelativeMove (int id, string axis, double offset)
        {
            var currentTarget = new [] { 0.0 };
            if (GCS2.qMOV (id, axis, currentTarget) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Cannot read current target.");
            }

            var target    = currentTarget[0] + offset;
            if (GCS2.MOV(id, axis, new[] { target }) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Cannot move to target.");
            }
        }

        private bool WaitForMotionDone (string axis)
        {
            var isMoving = new int [] {1};

            while (isMoving [0] == 1)
            {
                Console.WriteLine("Waiting for movement to end ...");

                if (GCS2.IsMoving (ControllerId, axis, isMoving) == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError("Unable to retrieve motion state.");
                }

                Thread.Sleep (10000);
            }

            Console.WriteLine("Movement is finished.");
            return true;
        }

        private double GetMaxPositionLimit (string axis)
        {
            var paramMaxPosLimit = new Parameter (axis, "-", "0x122");
            return ReadParameterValue (ControllerId, "RAM", paramMaxPosLimit);
        }

        private double GetMinPositionLimit (string axis)
        {
            var paramMinPosLimit = new Parameter (axis, "-", "0x121");
            return ReadParameterValue (ControllerId, "RAM", paramMinPosLimit);
        }

        private void SetModeOfOperation (string axis, int mode)
        {
            if (GCS2.SAM (ControllerId, axis, mode) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("SAM failed. Exiting");
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
