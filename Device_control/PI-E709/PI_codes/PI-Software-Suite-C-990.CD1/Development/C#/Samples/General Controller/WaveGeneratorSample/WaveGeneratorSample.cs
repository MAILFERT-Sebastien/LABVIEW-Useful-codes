/** <copyright file="WavegeneratorSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 *   !! MOTION WARNING !!
 *
 *   This sample connects to a controller using the PI Connection Dialog. The connection type can be easily changed.
 *   Then the wave generator is configured and started.
 *   The stage has to be initialized and referenced before starting this sample.
 *   </remarks> */


using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using PI;

namespace WaveGeneratorSample
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

    /** <summary>
     *  This class runs the wave generator for a controller that supports this feature, i.e. E-727, C-887.
     *  Call the Run-method to execute this sample.
     *  Please choose the right configuration for your setup, there is one common configuration
     *  working with most of PI controllers which support the wave generator feature
     *  and one configuration for the Hexapod controller C-887
     * </summary>*/
    class WaveGeneratorSample
    {
        /// <remarks>
        /// select the wave generator configuration here: 'Common' or 'C-887'
        /// </remarks>
        #region ControllerSpecificSettings
        private readonly WaveGeneratorSettings _waveGeneratorSettings = WaveGeneratorConfigurations.Properties["Common"];
        #endregion

        #region fields
        private const int PI_TRUE = 1;
        private const int PI_FALSE = 0;
        private const int PI_RESULT_FAILURE = 0;
        private const uint PI_PARAMETER_ID_SERVO_TIME = 0xE000200;
        internal int ControllerId = -1;
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                HasWaveGenerator();
                ConfigureWaveGenerator();

                var state = new[] {PI_TRUE};
                SetServoState(_waveGeneratorSettings.Axis, state);
                GoToStartPosition();
                StartWaveGenerator();
                state = new[] {PI_FALSE};
                SetServoState(_waveGeneratorSettings.Axis, state);

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
                Environment.ExitCode = -1;
            }
        }
        #endregion

        #region private methods
        private void StartWaveGenerator()
        {
            // restart recording as soon as wave generator output starts running
            if ( GCS2.WGR(ControllerId) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Failed to restart recording.");
            }

            // start wave generator output, data recorder starts simultaneously
            if ( GCS2.WGO( ControllerId
                            , _waveGeneratorSettings.WaveGeneratorId
                            , _waveGeneratorSettings.StartMode
                            , 1)
                 == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to start wave generator output.");
            }

            WaitForWaveGeneratorDone().Wait(7000);
        }

        private async Task WaitForWaveGeneratorDone()
        {
            // initialize provisional array for WG WaveGeneratorId,
            // since PI_IsGeneratorRunning expects an array as parameter
            // status of wave generator (0 = not running, 1 = running)
            var isWgRunning = Enumerable.Repeat( 1 , _waveGeneratorSettings.NumberOfAxis).ToArray();
            var waveGeneratorIdArray = _waveGeneratorSettings.WaveGeneratorId;

            // query wave generator status
            while (isWgRunning[0] == 1)
            {
                await Task.Delay(100);

                if ( GCS2.IsGeneratorRunning( ControllerId
                                            , waveGeneratorIdArray
                                            , isWgRunning
                                            , 1)
                     == PI_RESULT_FAILURE )
                {
                    throw new GcsCommandError("Unable to query wave generator status.");
                }
            }
        }

        private void GoToStartPosition()
        {
            Console.WriteLine("Starting wave generator...");

            // move stage to offset of wave prior to starting wave generator
            if (GCS2.MOV( ControllerId
                    , _waveGeneratorSettings.Axis
                    , _waveGeneratorSettings.OffsetOfWave)
                == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to move stage to start position.");
            }

            WaitForMotionDone(_waveGeneratorSettings.Axis).Wait(7000);
        }

        private async Task WaitForMotionDone (string axis)
        {
            int[] isMoving = {0};

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

        private void ConfigureWaveGenerator()
        {
            Console.WriteLine("Setting up wave generator: Frequency: "
                              + _waveGeneratorSettings.Frequency
                              + " Hz, Amplitude: "
                              + _waveGeneratorSettings.AmplitudeOfWave
                              + " mm");
            // define waveform

            var servoCycleTime = GetServoCycleTime();

            // calculate number of point in wave table from given frequency
            var numberOfPoints = (int) (1 / (servoCycleTime[0] * _waveGeneratorSettings.Frequency));

            // wave table contains one segment
            var segmentLength = numberOfPoints;

            // curve center point is the middle of the segment
            var centerPointOfWave = numberOfPoints / 2;

            // send wave table to controller

            if (_waveGeneratorSettings.WaveGeneratorId.Any(id => GCS2.WAV_SIN_P(ControllerId
                                                        , id // 1
                                                        , _waveGeneratorSettings.OffsetOfFirstPointInWaveTable // 0
                                                        , numberOfPoints // 2000
                                                        , _waveGeneratorSettings.AppendWave // 0
                                                        , centerPointOfWave // 1000
                                                        , _waveGeneratorSettings.AmplitudeOfWave // 0.01
                                                        , _waveGeneratorSettings.OffsetOfWave[0] //0.0
                                                        , segmentLength //2000
                                                                        )
                                                    == PI_RESULT_FAILURE) )
            {
                throw new GcsCommandError("Unable to set up waveform.");
            }

            // link wave table to wave generator
            if (GCS2.WSL( ControllerId
                    , _waveGeneratorSettings.WaveGeneratorId
                    , _waveGeneratorSettings.WaveTableId
                    , 1)
                == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to link wave table to wave generator.");
            }

            // set wave table rate
            if (GCS2.WTR( ControllerId
                        , new []{0}
                        , _waveGeneratorSettings.TableRate
                        , _waveGeneratorSettings.InterpolationType
                        , _waveGeneratorSettings.TableRate.Length)
                == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to set wave table rate.");
            }

            // set wave generator output cycles
            if (GCS2.WGC( ControllerId
                        , _waveGeneratorSettings.WaveGeneratorId
                        , _waveGeneratorSettings.NumberOfCycles
                        , 1)
                == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to set wave generator output cycles.");
            }
        }

        private double[] GetServoCycleTime()
        {
            // query servo cycle time
            double[] servoCycleTime = {0.0}; // servo cycle time of controller
            uint[] parametersToQuery = {PI_PARAMETER_ID_SERVO_TIME};

            if (GCS2.qSPA( ControllerId
                    , _waveGeneratorSettings.Axis
                    , parametersToQuery
                    , servoCycleTime
                    , null
                    , 0)
                == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to query servo update time.");
            }

            return servoCycleTime;
        }

        private void HasWaveGenerator()
        {
            if (!CheckNumberOfWaveGenerators())
            {
                throw new GcsCommandError("Controller has no wave Generators");
            }
        }

        private bool CheckNumberOfWaveGenerators()
        {
            var numberOfWaveGenerators = 0;

            if (GCS2.qTWG(ControllerId, ref numberOfWaveGenerators ) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable retrieve number of wave generators.");
            }

            return (numberOfWaveGenerators > 0);
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

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state ) == PI_RESULT_FAILURE )
            {
                throw (new GcsCommandError("SVO failed. Exiting"));
            }
        }
        #endregion
    }
}
