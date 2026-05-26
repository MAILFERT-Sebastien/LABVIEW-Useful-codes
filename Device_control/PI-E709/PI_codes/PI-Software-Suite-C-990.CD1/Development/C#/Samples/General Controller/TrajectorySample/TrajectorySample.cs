/** <copyright file="TrajectorySample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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

/** <remarks>
 *  !! MOTION WARNING !!
 *
 *  This sample connects to a controller using the PI Connection Dialog.
 *  After that, a trajectory is setup, transferred and started.
 *  The stage has to be initialized and referenced before starting this sample.
 * </remarks> */

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using PI;

namespace TrajectorySample
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

    internal class Position
    {
        public Position(double x, double y)
        {
            X = x;
            Y = y;
        }

        public double X { get; set; }

        public double Y { get; set; }
    }

    struct TrajectorySettings
    {
        public readonly int NumberOfTrajectoryPositions;
        public readonly double Amplitude;
        public readonly double TrajectoryTotalTimeInSeconds;
        public readonly List<Position> TrajectoryPositions;
        public readonly int[] TrajectoryIDs;

        public TrajectorySettings( int numberOfTrajectoryPositions
                                 , double amplitude
                                 , int[] trajectoryIDs
                                 , double trajectoryTotalTimeInSeconds)
        {
            NumberOfTrajectoryPositions = numberOfTrajectoryPositions;
            Amplitude = amplitude;
            TrajectoryPositions = new List<Position>();
            TrajectoryIDs = trajectoryIDs;
            TrajectoryTotalTimeInSeconds = trajectoryTotalTimeInSeconds;

            CalculatePositionsOfSampleTrajectory();
        }

        private void CalculatePositionsOfSampleTrajectory()
        {
            TrajectoryPositions.Clear();

            var centerPosition = new Position(0.0, 0.0);

            for (var i = 0; i < NumberOfTrajectoryPositions; ++i)
            {
                TrajectoryPositions.Add(new Position(
                    centerPosition.X + (Amplitude * Math.Sin(2 * Math.PI * i / NumberOfTrajectoryPositions)),
                    centerPosition.Y + (Amplitude * Math.Cos(2 * Math.PI * i / NumberOfTrajectoryPositions))));
            }
        }
    }

    /** <summary>
     *  This class runs the trajectory movement for a controller that supports this feature, i.e. C-884, C-867.
     *  Call the Run-method to execute this sample.
     *  This sample will calculate a circular trajectory given on the input
     *  provided in the controller specific configuration, transfer the trajectory to the controller buffer
     *  and start moving along the trajectory. Trajectory will start when the buffer is filled
     *  and the sample will keep filling the buffer after the start until all points transferred.
     * </summary>*/
    public class TrajectorySample
    {
        #region controller specific configuration
        private readonly TrajectorySettings _trajectorySettings
            = new TrajectorySettings ( 360
                , 1.0
                , new []{1,2}
                , 5.0 ) ;
        private const string PI_S_AXIS = "1 2";
        private const int PI_AXIS1 = 0;
        private const int PI_AXIS2 = 1;
        private const uint PI_PARAMETER_ID_SERVO_TIME = 0xE000200;
        private const uint PI_PARAMETER_ID_BUFFER_SIZE = 0x22000020;
        private const int PI_WAITING_TIME100_MS = 100;
        private const string PI_SYSTEM_PARAMETER_ITEM = "1";
        #endregion controller specific configuration

        #region fields
        private int _maxBufferSizeForTrajectory;
        private bool _isTrajectoryRunning;

        internal int ControllerId = -1;
        private const int PI_TRUE = 1;
        private const int PI_FALSE = 0;
        private const int PI_RESULT_FAILURE = 0;
        #endregion field

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                SetServoState(PI_S_AXIS, new[] {PI_TRUE, PI_TRUE});

                MoveToStartPositionOfSampleTrajectory();
                ConfigureTrajectoryTiming();
                ClearExistingTrajectoriesOnController();
                RunTrajectory().Wait(Convert.ToInt32(_trajectorySettings.TrajectoryTotalTimeInSeconds * 1.5));

                SetServoState(PI_S_AXIS, new[] { PI_FALSE, PI_FALSE });
            }
            catch (GcsCommandError e)
            {
                PrintErrorMessage(e);
            }
            catch (Exception e)
            {
                // Catch all Exception which ar not GCSErrors, i.e. for file handling
                Console.WriteLine(e);
            }
            finally
            {
                DisconnectController();
            }
        }
        #endregion public methods

        #region private methods
        private async Task RunTrajectory()
        {
            _isTrajectoryRunning = false;
            _maxBufferSizeForTrajectory = (int) ReadParameterFromController(PI_PARAMETER_ID_BUFFER_SIZE);
            var cts = new CancellationTokenSource();
            while (_trajectorySettings.TrajectoryPositions.Count > 0)
            {
                if (CanControllerStoreAnotherPointOfTrajectory())
                {
                    StoreTrajectoryPointOnController(_trajectorySettings.TrajectoryPositions[0]);
                    _trajectorySettings.TrajectoryPositions.RemoveAt(0);
                }
                else
                {
                    if (!_isTrajectoryRunning)
                    {
                        StartTrajectory();
                    }
                    else
                    {
                        await Task.Delay(PI_WAITING_TIME100_MS, cts.Token);
                    }
                }
            }

            if (!_isTrajectoryRunning)
            {
                StartTrajectory();
            }

            InformControllerThatTrajectoryIsComplete();
            cts.CancelAfter(8000);
            WaitUntilTrajectoryIsFinished().Wait(cts.Token);

            Console.WriteLine("Trajectory is finished.");
        }

        private void MoveToStartPositionOfSampleTrajectory()
        {
            if (_trajectorySettings.TrajectoryPositions.Count > 0)
            {
                double[] startPosition = { _trajectorySettings.TrajectoryPositions[0].X
                                         , _trajectorySettings.TrajectoryPositions[0].Y };

                if (GCS2.MOV(ControllerId, PI_S_AXIS, startPosition) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("MOV() failed"));
                }
                WaitForMotionDone(PI_S_AXIS).Wait(7000);
            }
            Console.WriteLine("Moving to start position of trajectory is finished!");
        }

        private async Task WaitForMotionDone (string axis)
        {
            int[] isMoving = {0, 0};

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

        private void ConfigureTrajectoryTiming()
        {
            var servoUpdateTime = ReadParameterFromController(PI_PARAMETER_ID_SERVO_TIME);
            var trajectoryRate = (int) (_trajectorySettings.TrajectoryTotalTimeInSeconds
                                        / _trajectorySettings.TrajectoryPositions.Count
                                        / servoUpdateTime);
            if ( GCS2.TGT(ControllerId, trajectoryRate) == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("TGT() failed"));
            }
            Console.WriteLine("Trajectory timing is configured!");
        }

        private double ReadParameterFromController(uint parameterId)
        {
            uint[] parameterIDs = { parameterId };
            double[] parameterValues = { 0.0 };
            if (GCS2.qSPA( ControllerId
                          , PI_SYSTEM_PARAMETER_ITEM
                          , parameterIDs
                          , parameterValues
                          , null, 0)
                == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("qSPA() failed"));
            }
            return parameterValues[0];
        }

        private void ClearExistingTrajectoriesOnController()
        {
            if (GCS2.TGC(ControllerId, _trajectorySettings.TrajectoryIDs, 2) == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("TGC() failed"));
            }
        }

        private int[] GetCurrentBufferLevelOfTrajectories()
        {
            int[] usedBufferForTrajectory = { 0, 0 };

            if (GCS2.qTGL( ControllerId
                            , _trajectorySettings.TrajectoryIDs
                            , usedBufferForTrajectory
                            , usedBufferForTrajectory.Length)
                == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("qTGL() failed"));
            }

            return usedBufferForTrajectory;
        }

        private bool CanControllerStoreAnotherPointOfTrajectory()
        {
            var usedBufferForTrajectory = GetCurrentBufferLevelOfTrajectories();

            Console.WriteLine("Number of values in buffer - axis 1: " + usedBufferForTrajectory[PI_AXIS1] + " - axis 2: " + usedBufferForTrajectory[PI_AXIS2]);

            return (usedBufferForTrajectory[PI_AXIS1] < _maxBufferSizeForTrajectory)
                   && (usedBufferForTrajectory[PI_AXIS2] < _maxBufferSizeForTrajectory);
        }

        private void StoreTrajectoryPointOnController(Position position)
        {
            double[] currentPosition = { position.X, position.Y };

            if (GCS2.TGA( ControllerId
                           , _trajectorySettings.TrajectoryIDs
                           , currentPosition
                           , currentPosition.Length)
                == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("TGA() failed"));
            }
        }

        private void StartTrajectory()
        {
            if (GCS2.TGS(ControllerId
                           , _trajectorySettings.TrajectoryIDs
                           , _trajectorySettings.TrajectoryIDs.Length)
                == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("TGS() failed"));
            }
            _isTrajectoryRunning = true;
        }

        private void InformControllerThatTrajectoryIsComplete()
        {
            if (GCS2.TGF(ControllerId
                           , _trajectorySettings.TrajectoryIDs
                           , _trajectorySettings.TrajectoryIDs.Length)
                == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("TGF() failed"));
            }
        }

        private async Task WaitUntilTrajectoryIsFinished()
        {
            while(true)
            {
                try
                {
                    var usedBufferForTrajectory = GetCurrentBufferLevelOfTrajectories();

                    Console.WriteLine("Number of values in buffer - axis 1: "
                                      + usedBufferForTrajectory[PI_AXIS1]
                                      + " - axis 2: "
                                      + usedBufferForTrajectory[PI_AXIS2]);
                    if (usedBufferForTrajectory[PI_AXIS1] > 0 || usedBufferForTrajectory[PI_AXIS2] > 0)
                    {
                        await Task.Delay(PI_WAITING_TIME100_MS);
                    }
                    else
                    {
                        _isTrajectoryRunning = false;
                        break;
                    }
                }
                catch (GcsCommandError e)
                {
                    PrintErrorMessage(e);
                    return;
                }
            }

            Console.WriteLine("Trajectory is finished.");
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

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state ) == PI_RESULT_FAILURE )
            {
                throw (new GcsCommandError("SVO failed. Exiting"));
            }
        }
        #endregion private methods
    }
}


