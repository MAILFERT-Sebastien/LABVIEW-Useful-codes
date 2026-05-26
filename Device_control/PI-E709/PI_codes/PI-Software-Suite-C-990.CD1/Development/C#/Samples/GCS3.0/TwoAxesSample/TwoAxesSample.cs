/** <copyright file="TwoAxisSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 * !! MOTION WARNING !!
 *
 * This sample connects to a two-axis controller and references both axes.
 * Afterwards, it moves the two axes forward and backward repeatedly.
 *
 * Both axes have to be initialized and referenced.
 * </remarks> */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using PI;

namespace TwoAxesSample
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
        public GcsCommandError( string message )
            : base(message)
        {
        }
    }

    public readonly struct Parameter
    {
        internal string ContainerUnitId { get; }
        internal string FunctionUnitId { get; }
        internal string ParameterId { get; }

        public Parameter(string containerUnitId, string functionUnitId, string parameterId)
        {
            ContainerUnitId = containerUnitId;
            FunctionUnitId  = functionUnitId;
            ParameterId     = parameterId;
        }
    }

    /** <summary>
    *  This sample connects to a two-axis controller.
    *  It moves the two axes forward and backward repeatedly.
    * </summary>*/
    public class TwoAxesSample
    {
        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int MODE_OF_OPERATION_OPEN_LOOP_POSITION = 1;
        private const int MODE_OF_OPERATION_CLOSED_LOOP_POSITION = 2;
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                // based on your controller type, use the appropriate constant
                var axes = new [] { "AXIS_1", "AXIS_2"};

                SetModeOfOperation (ControllerId, axes, MODE_OF_OPERATION_CLOSED_LOOP_POSITION);

                // get min/max position limits
                var minPosLimits = GetMinPositionLimits (ControllerId, axes);
                var maxPosLimits = GetMaxPositionLimits (ControllerId, axes);
                var travelRange  = CalculateTravelRange (minPosLimits,  maxPosLimits);

                // do absolute motion to near minimum position with both axes
                MoveAbsoluteToNearMinimum (axes, minPosLimits, travelRange);
                // move axes sequentially with relative moves
                RelativeMoves (ControllerId, axes, travelRange );

                SetModeOfOperation (ControllerId, axes, MODE_OF_OPERATION_OPEN_LOOP_POSITION);
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
        private void MoveAbsoluteToNearMinimum (string[] axes, double[] minPosLimit, IEnumerable<double> travelRange)
        {
            if (travelRange == null) throw new ArgumentNullException(nameof(travelRange));
            if (minPosLimit == null) throw new ArgumentNullException(nameof(minPosLimit));
            var targetPos = CalculateTargetPosition(minPosLimit, travelRange, 1.0);
            MoveToAbsoluteTarget (axes, targetPos);
            WaitForMotionDone (string.Join(" ", axes)).Wait(8000);
            Console.WriteLine("Successfully moved near minimum position limit, to "
                              + string.Join(" ", targetPos.Select(element => $"{element:F3}")));
        }

        private void RelativeMoves (int controllerId, string[] axes, double[] travelRange)
        {
            var distances = Array.ConvertAll (travelRange, x => x * 0.1);

            try
            {
                while (true)
                {
                    for (var idx = 0; idx < axes.Length; idx++)
                    {
                        Console.WriteLine ("Moving axis {0} for {1} units", axes[idx], distances[idx]);
                        RelativeMove (controllerId, axes[idx], distances[idx]);
                        WaitForMotionDone (axes[idx]).Wait (16000);
                    }

                    distances = Array.ConvertAll (distances, x => x * -1.0 * 1.3);
                }
            }
            catch (GcsCommandError e)
            {
                // we expect this to fail, since the distances are increased every time
                PrintErrorMessage(e);
            }
        }

        private double[] CalculateTravelRange (double[] minPosLimit, double[] maxPosLimit)
        {
            var travelRange = (
                    from position
                        in minPosLimit.Zip (maxPosLimit, (min, max) => new {Min = min, Max = max})
                    let result = Math.Abs (position.Max - position.Min)
                    select result
                )
                .ToArray ();

            return travelRange;
        }

        private double[] GetCurrentPositions (int controllerId, string[] axes)
        {
            var currentPositions = new List<double> ();

            foreach (var axis in axes)
            {
                var position = new [] {0.0};
                if (GCS2.qPOS (ControllerId, axis, position) == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError ("Unable to query to current position.");
                }
                currentPositions.Add (position[0]);
            }

            return currentPositions.ToArray ();

        }

        private void MoveToAbsoluteTarget (string[] axes, double[] targetPos)
        {
            var axis = string.Join (" ", axes);
            if (GCS2.MOV (ControllerId, axis, targetPos) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Unable to move to target position.");
            }
        }

        private void RelativeMove (int id, string axis, double offset)
        {
            var currentTarget = new[] {0.0};
            if (GCS2.qMOV (id, axis, currentTarget) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Cannot read current target.");
            }

            var target = currentTarget[0] + offset;
            if (GCS2.MOV (id, axis, new[] {target}) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError ("Cannot move to target.");
            }
        }

        private async Task WaitForMotionDone ( string axis )
        {
            var isMoving = Enumerable.Repeat(1, axis.Split(' ').Length ).ToArray();

            while ( Array.Exists(isMoving, element => element != 0 ))
            {
                if (GCS2.IsMoving(ControllerId, axis, isMoving ) == PI_RESULT_FAILURE )
                {
                    throw new GcsCommandError("Unable to query movement status.");
                }

                await Task.Delay(10);
            }

        }

        private static double[] CalculateTargetPosition(IEnumerable<double> minPosLimit
            , IEnumerable<double> travelRange
            , double modifier )
        {
            var targetPos = (
                    from position
                        in minPosLimit.Zip(travelRange, (limit, range) => new {Limit = limit, Range = range})
                    let result = position.Limit + (position.Range * modifier * 0.1)
                    select result
                )
                .ToArray();
            return targetPos;
        }

        private double GetMaxPositionLimit (int controllerId, string axis)
        {
            var paramMaxPosLimit = new Parameter (axis, "-", "0x122");
            return ReadParameterValue (controllerId, "RAM", paramMaxPosLimit);
        }

        private double GetMinPositionLimit (int controllerId, string axis)
        {
            var paramMinPosLimit = new Parameter (axis, "-", "0x121");
            return ReadParameterValue (controllerId, "RAM", paramMinPosLimit);
        }

        private double[] GetMinPositionLimits (int controllerId, string[] axes)
        {
            var minPosLimits = new List<double> ();
            foreach (var axis in axes)
            {
                minPosLimits.Add (GetMinPositionLimit (controllerId, axis));
            }

            return minPosLimits.ToArray ();
        }

        private double[] GetMaxPositionLimits (int controllerId, string[] axes)
        {
            var maxPosLimits = new List<double> ();
            foreach (var axis in axes)
            {
                maxPosLimits.Add (GetMaxPositionLimit (controllerId, axis));
            }

            return maxPosLimits.ToArray ();
        }

        private void SetModeOfOperation (int controllerId, string[] axes, int mode)
        {
            foreach(var axis in axes)
            {
                if (GCS2.SAM(ControllerId, axis, mode) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("SAM failed. Exiting"));
                }
            };
            Console.WriteLine("Set mode of operation: {0}", mode);
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
}
