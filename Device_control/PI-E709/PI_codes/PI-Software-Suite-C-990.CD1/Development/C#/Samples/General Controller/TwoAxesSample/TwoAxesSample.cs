/** <copyright file="TwoAxisSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 * !! MOTION WARNING !!
 *
 * This sample connects to a controller with at least 2 axes
 * and demonstrate how to use 2 axis.
 *
 * Command a absolute movement with 2 axes with one command and afterwards
 * demonstrate a movement with 2 axes sequentially.
 *
 * Both axes have to be initialized and referenced.
 * </remarks> */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using PI;

namespace TwoAxisSample
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

    /** <summary>
    *  This class runs the sample for a controller with at least 2 axis, i.e. C-884, C-887.
    *  Call the Run-method to execute this sample.
    *  Please choose the right configuration for your setup, there is one common configuration
    *  working with most of PI controllers and one configuration for the Hexapod controller C-887
    * </summary>*/
    public class TwoAxisSample
    {
        /// <remarks>
        /// select the wave generator configuration here: 'Common' or 'C-887'
        /// </remarks>
        #region controller specific configuration
        public static Dictionary<string, string> AxisIdentifier { get; }
            = new Dictionary<string, string>
            {
                {"Common", "1 2"},
                {"C-887", "X Y"},
            };

        private static readonly string Axis = AxisIdentifier["Common"];
        private const int PI_NUMBER_OF_AXIS = 2;
        #endregion

        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_TRUE = 1;
        private const int PI_FALSE = 0;
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                var state = Enumerable.Repeat(PI_TRUE, PI_NUMBER_OF_AXIS).ToArray();
                SetServoState(Axis, state);

                double[] minPosLimits = GetMinPositionLimit(Axis);
                double[] maxPosLimits = GetMaxPositionLimit(Axis);
                var travelRange = CalculateTravelRange(minPosLimits, maxPosLimits);

                MoveAbsoluteToNearMinimum(minPosLimits, travelRange);
                WaitForMotionDone(Axis).Wait(8000);
                RelativeMoves( maxPosLimits, travelRange );

                state = Enumerable.Repeat(PI_FALSE, PI_NUMBER_OF_AXIS).ToArray();
                SetServoState(Axis, state);
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
        private void MoveAbsoluteToNearMinimum(double[] minPosLimits, IEnumerable<double> travelRange)
        {
            if (travelRange == null) throw new ArgumentNullException(nameof(travelRange));
            if (minPosLimits == null) throw new ArgumentNullException(nameof(minPosLimits));
            var targetPos = CalculateTargetPosition(minPosLimits, travelRange, 1.0);
            MoveToAbsoluteTarget(targetPos);
        }

        private void RelativeMoves(double[] maxPosLimits, IEnumerable<double> travelRange)
        {
            var aAxis = Axis.Split(' ').ToArray();
            var aTravelRange = travelRange.ToArray();
            var distances = Array.ConvertAll(aTravelRange, x => x * 0.1);

            while (true)
            {
                for (var idx = 0; idx < aAxis.Length; idx++)
                {
                    Console.WriteLine($"Moving axis {aAxis[idx]} for {distances[idx]} units");

                    try
                    {
                        RelativeMove(aAxis[idx], new[] { distances[idx] });
                    }
                    catch (GcsCommandError e)
                    {
                        // we expect this to fail, since the distances are increased every time
                        Console.WriteLine($"MVR failed.Current axis: {aAxis[idx]}; Value: {distances[idx]}");
                        PrintErrorMessage(e);
                        return;
                    }

                    WaitForMotionDone(aAxis[idx]).Wait(8000);
                }

                distances = Array.ConvertAll(distances, x => x * -1.0 * 1.2);
            }
        }

        private double[] CalculateTravelRange(double[] minPosLimits, double[] maxPosLimits)
        {
            var travelRange = (
                    from position
                        in minPosLimits.Zip(maxPosLimits, (min, max) => new {Min = min, Max = max})
                    let result = Math.Abs(position.Max) + Math.Abs(position.Min)
                    select result
                )
                .ToArray();
            return travelRange;
        }

        private void MoveToAbsoluteTarget(double[] targetPos)
        {
            if (GCS2.MOV(ControllerId, Axis, targetPos) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
            }
        }

        private void RelativeMove(string axis, double[] targetPos)
        {
            if (GCS2.MVR(ControllerId, axis, targetPos) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
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

        private static double[] CalculateTargetPosition(IEnumerable<double> minPosLimits
            , IEnumerable<double> travelRange
            , double modifier )
        {
            var targetPos = (
                    from position
                        in minPosLimits.Zip(travelRange, (limit, range) => new {Limit = limit, Range = range})
                    let result = position.Limit + (position.Range * modifier * 0.1)
                    select result
                )
                .ToArray();
            return targetPos;
        }

        private double[] GetMaxPositionLimit(string axis)
        {
            var maxPosLimits = new double[PI_NUMBER_OF_AXIS];

            if (GCS2.qTMX (ControllerId, axis, maxPosLimits) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to get maximum position limit.");
            }
            return maxPosLimits;
        }

        private double[] GetMinPositionLimit(string axis)
        {
            var minPosLimits = new double[PI_NUMBER_OF_AXIS];

            if (GCS2.qTMN (ControllerId, axis, minPosLimits) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to get minimum position limit.");
            }

            return minPosLimits;
        }

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state) == PI_RESULT_FAILURE)
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
}
