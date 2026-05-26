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
 *  It shows how to determine min/max position limits, how to do absolute moves and how to recover from an axis error.
 *  The stage has to be referenced before starting this sample.
 *
 *</remarks>*/

using System;
using System.Configuration;
using System.Text;
using System.Threading;
using PI;

namespace ResetErrorSample
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
    public class ResetErrorSample
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

        public struct PID
        {
            public double P;
            public double I;
            public double D;
        }

        #endregion

        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int MODE_OF_OPERATION_OPEN_LOOP_POSITION = 1;
        private const int MODE_OF_OPERATION_CLOSED_LOOP_POSITION = 2;

        private const int START_BIT_DSM = 0;  // Start bit drive state machine
        private const int START_BIT_MOD = 8;  // Start bit mode of operation
        private const uint DSM_BIT_MASK  = 0xFF;
        private const uint DSM_ERROR_BIT = 1;
        private const uint DSM_FAULT_REACTION_BITS = 3;

        #endregion

        #region public methods
        public void Run ()
        {
            try
            {
                OpenConnection (ConnectionType.Dialog);
                PrintControllerIdentification ();

                const string sAxis = "AXIS_1";
                const string pidAxis = "CNTR_1";

                SetModeOfOperation (sAxis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);

                // get current position and min/max position limits
                var originalPosition = GetPosition(sAxis);
                var minPosLimit = GetMinPositionLimit (sAxis);
                var maxPosLimit = GetMaxPositionLimit (sAxis);

                var range = maxPosLimit - minPosLimit;

                var pid = PrepareMotionError(sAxis, pidAxis); // Set PID = 0 which will generate motion error on next movement command.

                // do absolute motion to middle of travel range
                var targetPos = minPosLimit + (range * 0.25); // Motion error will not be triggered if movement is too small.
                MoveToAbsoluteTarget(sAxis, targetPos);
                // let axis "accelerate". Because PID = 0 the axis will stay at current position until motion error is generated.
                Thread.Sleep (2000);

                // Note that by default, all axes are grouped via the “Group” configuration parameter (address: Axis_x - 0x50).
                // If an axis from a group goes into the “fault” state, all other axes of the group also go into the “fault” state
                // and require also the error handling.
                RecoverFromError(sAxis, pidAxis, pid); // Restore previous PID values. This allows movement commands without motion error.
                MoveToAbsoluteTarget(sAxis, targetPos); // Continue with absolute motion

                WaitForMotionDone(sAxis);
                Console.WriteLine ("Successfully moved to first quarter of travel range " + targetPos.ToString ("F3"));

                MoveToAbsoluteTarget(sAxis, originalPosition); // Go back to original position
                WaitForMotionDone(sAxis);
                Console.WriteLine("Successfully moved back to original position " + originalPosition.ToString("F3"));

                SetModeOfOperation(sAxis, MODE_OF_OPERATION_OPEN_LOOP_POSITION);
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

        private void MoveToAbsoluteTarget (string axis, double targetPos)
        {
            if (GCS2.MOV (ControllerId, axis, new [] {targetPos}) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
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

        private double GetPosition (string axis)
        {
            var position = new double[] { 0.0 };

            if (GCS2.qPOS (ControllerId, axis, position) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("POS? failed. Exiting");
            }

            return position[0];
        }

        private double GetMaxPositionLimit (string axis)
        {
            var paramMaxPosLimit = new Parameter (axis, "-", "0x122");
            return ReadParameterValue ("RAM", paramMaxPosLimit);
        }

        private double GetMinPositionLimit (string axis)
        {
            var paramMinPosLimit = new Parameter (axis, "-", "0x121");
            return ReadParameterValue ("RAM", paramMinPosLimit);
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

        private double ReadParameterValue (string memoryType, Parameter parameter)
        {
            double value = 0.0;
            if (GCS2.qSPV_Double (
                ControllerId,
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

        private void WriteParameterValue (string memoryType, Parameter parameter, double value)
        {
            if (GCS2.SPV_Double(
                    ControllerId,
                    memoryType,
                    parameter.ContainerUnitId,
                    parameter.FunctionUnitId,
                    parameter.ParameterId,
                    value) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Could not write parameter value.");
            }
        }

        private PID PrepareMotionError(string axis, string pid)
        {
            // Higher user level is required for writing parameters. See password in your controller manual.
            if (GCS2.UCL (ControllerId, "ADV", "advanced") == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to change user level.");
            }
            Console.WriteLine ("Changed user level to ADV.");

            PID result;
            result.P = ReadParameterValue ("RAM", new Parameter (axis, pid, "0x120,1"));
            result.I = ReadParameterValue ("RAM", new Parameter (axis, pid, "0x121,1"));
            result.D = ReadParameterValue ("RAM", new Parameter (axis, pid, "0x122,1"));
            WriteParameterValue ("RAM", new Parameter (axis, pid, "0x120,1"), 0.0);
            WriteParameterValue ("RAM", new Parameter (axis, pid, "0x121,1"), 0.0);
            WriteParameterValue ("RAM", new Parameter (axis, pid, "0x122,1"), 0.0);
            return result;
        }

        private void RecoverFromError (string axis, string pidAxis, PID oldPID)
        {
            if (!AxisHasError(axis))
            {
                throw new GcsCommandError ("Axis expected to be in error state. Did PrepareMotionError() fail or is absolute movement too small?");
            }
            Console.WriteLine ("Axis is now in error state.");

            ResetAxesErrors ();

            // Restore PID parameters to allow movement
            WriteParameterValue ("RAM", new Parameter (axis, pidAxis, "0x120,1"), oldPID.P);
            WriteParameterValue ("RAM", new Parameter (axis, pidAxis, "0x121,1"), oldPID.I);
            WriteParameterValue ("RAM", new Parameter (axis, pidAxis, "0x122,1"), oldPID.D);

            // Enable axis
            if (GCS2.EAX(ControllerId, axis, new []{ 1 }) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to enable axis.");
            }

            SetModeOfOperation(axis, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);
        }

        private void ResetAxesErrors()
        {
            var axesString = new StringBuilder(1024);
            if (GCS2.qSAI(ControllerId, axesString, axesString.Capacity) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to get list of axes.");
            }

            string[] axes = axesString.ToString().Replace(" \n", ";").Replace("\n", "").Split(';');

            foreach (var axis in axes)
            {
                if (AxisHasError(axis))
                {
                    while (AxisInFaultReaction (axis))
                    {
                        Thread.Sleep (10);
                    }
                    // Reset axis
                    if (GCS2.RES(ControllerId, axis) == PI_RESULT_FAILURE)
                    {
                        throw new GcsCommandError("Unable to reset error.");
                    }
                }
            }
        }

        private bool AxisHasError(string axis)
        {
            var status = new uint[] { 1 };
            if (GCS2.qSTV(ControllerId, axis, status) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to read axis status.");
            }
            return ((status[0] >> START_BIT_DSM) & DSM_ERROR_BIT) == DSM_ERROR_BIT;
        }

        private bool AxisInFaultReaction (string axis)
        {
            var status = new uint[] { 0 };
            if (GCS2.qSTV(ControllerId, axis, status) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to read axis status.");
            }
            return ((status[0] >> START_BIT_DSM) & DSM_BIT_MASK) == DSM_FAULT_REACTION_BITS;
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
