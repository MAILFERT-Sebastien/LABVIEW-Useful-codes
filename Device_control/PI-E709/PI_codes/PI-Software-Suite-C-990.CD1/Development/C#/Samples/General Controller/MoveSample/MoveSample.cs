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
 *  This sample connects a controller using the connection dialog.
 *  It shows how to determine min/max position limits, also how to do absolute and relative motions.
 *  The stage has to be referenced before starting this sample.
 *
 *</remarks>*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
     *  This class executes reference move with EAX command i.e. for C-891.
     *  Call Run-method for executing this sample.
     *  Please choose the right configuration for your setup, there is one common configuration
     *  working with most of PI controllers and one configuration for the Hexapod controller C-887.
     * </summary>*/
    public class MoveSample
    {
        #region controller specific configuration
        public static Dictionary<string, string> AxisIdentifier { get; }
            = new Dictionary<string, string>
            {
                {"Common", "1"},
                {"C-887", "X"},
            };

        private static readonly string Axis = AxisIdentifier["Common"];
        private const int PI_NUMBER_OF_AXIS = 1;
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

                // get min/max position limits
                var minPosLimit = GetMinPositionLimit ( Axis);
                var maxPosLimit = GetMaxPositionLimit ( Axis);
                var travelRange = (
                        from position
                            in minPosLimit.Zip(maxPosLimit, (min, max) => new {Min = min, Max = max})
                            let result = position.Max - position.Min
                            select result
                            )
                    .ToArray();

                var state = Enumerable.Repeat(PI_TRUE, PI_NUMBER_OF_AXIS).ToArray();
                SetServoState(Axis, state);

                // do absolute motion to near minimum position
                var targetPos = CalculateTargetPosition(minPosLimit, travelRange, 1.0);
                MoveToAbsoluteTarget(targetPos);
                WaitForMotionDone(Axis).Wait(8000);
                Console.WriteLine("Successfully moved near minimum position limit, to "
                                  + string.Join(" ", targetPos.Select( element => $"{element:F3}" ) ));

                // do absolute motion to near maximum position
                targetPos = CalculateTargetPosition(maxPosLimit, travelRange, -1.0);
                MoveToAbsoluteTarget(targetPos);
                WaitForMotionDone(Axis).Wait(8000);
                Console.WriteLine("Successfully moved near maximum position limit, to "
                                  + string.Join(" ", targetPos.Select( element => $"{element:F3}" ) ));

                // do relative motions backwards
                targetPos = (from position
                        in maxPosLimit
                        let result = position * 0.25 * -1
                        select result).ToArray();
                MoveToRelativeTarget(targetPos);
                WaitForMotionDone(Axis).Wait(8000);
                Console.WriteLine("Successfully did relative move back for "
                                  + string.Join(" ", targetPos.Select( element => $"{element:F3}" ) ));

                MoveToRelativeTarget(targetPos);
                WaitForMotionDone(Axis).Wait(8000);
                Console.WriteLine("Successfully did relative move back for "
                                  + string.Join(" ", targetPos.Select( element => $"{element:F3}" ) ));

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
        private void MoveToAbsoluteTarget(double[] targetPos)
        {
            if (GCS2.MOV(ControllerId, Axis, targetPos) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
            }
        }

        private void MoveToRelativeTarget(double[] targetPos)
        {
            if (GCS2.MVR(ControllerId, Axis, targetPos) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move to target position.");
            }
        }

        private async Task WaitForMotionDone ( string axis)
        {
            var isMoving = Enumerable.Repeat(1, PI_NUMBER_OF_AXIS).ToArray();

            Console.WriteLine("Waiting for movement to end.");

            while ( Array.Exists(isMoving, element => element != 0 ))
            {
                if (GCS2.IsMoving(ControllerId, axis, isMoving ) == PI_RESULT_FAILURE )
                {
                    throw new GcsCommandError("Unable to query movement status.");
                }

                await Task.Delay(10);
            }

            Console.WriteLine("Movement is finished.");

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

        private double[] GetMaxPositionLimit(string axis)
        {
            var maxPosLimit = new double[PI_NUMBER_OF_AXIS];

            if (GCS2.qTMX (ControllerId, axis, maxPosLimit) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to get maximum position limit.");
            }
            return maxPosLimit;
        }

        private double[] GetMinPositionLimit(string axis)
        {
            var minPosLimit = new double[PI_NUMBER_OF_AXIS];

            if (GCS2.qTMN (ControllerId, axis, minPosLimit) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to get minimum position limit.");
            }

            return minPosLimit;
        }

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("SVO failed. Exiting");
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
