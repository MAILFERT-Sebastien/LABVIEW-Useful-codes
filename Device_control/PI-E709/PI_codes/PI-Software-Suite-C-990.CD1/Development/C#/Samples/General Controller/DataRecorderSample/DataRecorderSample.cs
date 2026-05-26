/** <copyright file="DataRecorderSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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

/**<remarks>
 *  !! MOTION WARNING !!
 *
 *  This sample connects to a controller using the PI Connection Dialog. The connection type can be easily changed.
 *  Then the data recorder is configured and samples of a step are captured.
 *  They are written to console and also to a file, names "test.dat".
 *  The stage has to be referenced before starting this sample.
 * </remarks>*/

using System;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using PI;

namespace DataRecorderSample
{
    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    /** <summary>
     *  This class does configuration of the data recorder, doing a step and reads the data from data recorder.
     *  Call the Run-method to execute this sample.
     *  Please choose the right configuration for your setup, there is one common configuration
     *  working with most of PI controllers and one configuration for the Hexapod controller C-887.
     * </summary>*/
    public class DataRecorderSample
    {
        /// <remarks>
        /// select the wave generator configuration here: 'Common' or 'C-887'
        /// </remarks>
        #region ControllerSpecificSettings
        private readonly DataRecorderSettings _dataRecorderSettings = DataRecorderConfigurations.Properties["Common"];
        #endregion ControllerSpecificSettings

        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        private const int PI_ON = 1;
        private const int PI_OFF = 0;
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();

                SetupDataRecorder(_dataRecorderSettings.Axis, _dataRecorderSettings);

                SetServoState(_dataRecorderSettings.Axis,new [] {PI_ON});
                DoStep(_dataRecorderSettings.Axis, new[] {1.0});
                SetServoState(_dataRecorderSettings.Axis, new[] {PI_OFF});

                WaitForRecordingToFinished(_dataRecorderSettings).Wait(7000);
                ReadDataRecorderData(_dataRecorderSettings);
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
        private void ReadDataRecorderData(DataRecorderSettings config)
        {
            var dataArray = Marshal.AllocHGlobal(0);
            var gcsHeader = new StringBuilder(1024);    // array storage for header

            // start reading asynchronously
            if (GCS2.qDRR(ControllerId
                            , config.TableIds
                            , config.ReadChannels
                            , 1
                            , config.NoOfSamplesToRead
                            , ref dataArray
                            , gcsHeader
                            , 300) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to start reading asynchronously.");
            }

            // The controller is now sending its recorded data. The function returns the Header and
            // a pointer to memory allocated in the dll. You could now analyze the Header information in GcsHeader.

            var destPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory,"test.dat");

            var gcsHeadStrings = gcsHeader.ToString().Split('\n').ToArray();
            File.WriteAllLines(destPath, gcsHeadStrings);


            Console.WriteLine("\nGCS Header:\n" + gcsHeader + "\n");
            Console.WriteLine("Reading...");

            // wait until the read pointer has reached the number of expected samples
            var cts = new CancellationTokenSource();
            WaitForNumberOfSamples(config, cts).Wait(cts.Token);

            // after data array is filled with data, you should process it or store it in a local variable,
            // as it will be cleared and/or overwritten with the next recording started by a motion command

            Console.WriteLine("Finished.\n");

            var dDataTable = new double[config.NoOfSamplesToRead*config.ReadChannels];
            Marshal.Copy(dataArray, dDataTable,0,  dDataTable.Length);

            var outputFile = File.AppendText(destPath);

            for (var row = 0; row <  config.NoOfSamplesToRead; row++)
            {
                // Print read data
                // The data columns
                // c1_1 c2_1 c3_1 c4_1
                // c1_2 c2_2 c3_2 c4_2
                // ...
                // c1_n c2_n c3_n c4_n
                // are aligned as follows:
                // OkFlag:
                // {c1_1,c2_1,c3_1,c4_1,c1_2,c2_2,...,c4_n}

                Console.Write(row + "\t");
                outputFile.Write(row + "\t");

                for (var column = 0; column < config.ReadChannels; column++)
                {
                    Console.Write("\t " + dDataTable[row * config.ReadChannels + column]);
                    outputFile.Write("\t" + dDataTable[row * config.ReadChannels + column]);
                }

                Console.Write("\n");
                outputFile.Write(row + "\n");
            }

            outputFile.Close();
            Console.WriteLine("Read and Write of recorded data is finished.");
        }

        private void DoStep(string sAxis, double[] amplitude)
        {
            if (GCS2.STE(ControllerId, sAxis, amplitude) == PI_RESULT_FAILURE )
            {
                throw new GcsCommandError("Unable to perform step.");
            }

            Console.WriteLine("Moved for a " + string.Join(" ", amplitude.Select( element => $"{element:F3}" ) ) + " step.");
            WaitForMotionDone(sAxis).Wait(7000);
        }

        private void SetupDataRecorder(string sAxis, DataRecorderSettings config)
        {
            Console.WriteLine("Setting up data recorder...");

            // configure data recorder
            if (GCS2.RTR(ControllerId, config.TableRate) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to set record table rate.");
            }

            var recordSourceIDs = sAxis + " " + sAxis;

            if (GCS2.DRC(ControllerId, config.TableIds, recordSourceIDs, config.Option) == PI_RESULT_FAILURE)
            {
                throw new GcsCommandError("Unable to set data recorder configuration.");
            }

            // for all controllers, except a few, the RecordChannelIdArray for DRT starts with '0'
            // try which drtTable ID suits the controller
            // 0 - default, data recording is triggered with IMP
            // 1 - any command changing target position or open-loop control value

            int[] drtTables = {0, 1};

            if (GCS2.DRT(ControllerId
                    , drtTables
                    , config.TriggerSource
                    , config.TriggerOption
                    , 1)
                != PI_RESULT_FAILURE)
            {
                Console.WriteLine("Supported record channel id for DRT starts with "
                    + string.Join(" ", drtTables.Select( element => $"{element:D}" ) ) + " step.");
            }
            else
            {
                // try again with Table IDs 1,2
                drtTables = new[] {1, 2};
                if (GCS2.DRT(ControllerId
                        , drtTables
                        , config.TriggerSource
                        , config.TriggerOption
                        , 1)
                    == PI_RESULT_FAILURE)
                {
                    throw new GcsCommandError("Unable to get supported record channel id for DRT.");
                }
            }

            WaitForControllerReady().Wait(7000);

        }

        private async Task WaitForControllerReady()
        {
            var ctlReady = 0;

            while ( ctlReady == 0 )
            {
                if (GCS2.IsControllerReady(ControllerId, ref ctlReady ) == PI_RESULT_FAILURE )
                {
                    throw new GcsCommandError("Unable to retrieve controller ready state.");
                }

                await Task.Delay(10);
            }

            Console.WriteLine("Controller is ready.");

        }

        private async Task WaitForMotionDone ( string axis)
        {
            int[] isMoving = {1};

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

        private async Task WaitForRecordingToFinished(DataRecorderSettings config)
        {
            int[] numberOfRecorderValues = { 0, 0 };

            while ( Array.Exists(numberOfRecorderValues, item => item <= config.NoOfSamplesToRead ) )
            {
                if (GCS2.qDRL( ControllerId
                              , config.TableIds
                              , numberOfRecorderValues
                              , config.ReadChannels ) == PI_RESULT_FAILURE )
                {
                    throw new GcsCommandError("Unable to query movement status.");
                }

                Console.WriteLine( "Reading values "
                    + string.Join(" ", numberOfRecorderValues.Select( element => $"{element:D}" ) ) );
                await Task.Delay(10);
            }

            Console.WriteLine("Recording is finished.");
        }

        private async Task WaitForNumberOfSamples(DataRecorderSettings config, CancellationTokenSource ctsTokenSource)
        {
            var index = -1;
            var timeout = 20;

            do
            {
                var oldIndex = index;
                await Task.Delay(100);

                // While the controller sends data, the buffer index is increasing,
                // so the data array is filled with valid data
                index = GCS2.GetAsyncBufferIndex(ControllerId);

                // If the index does not change for about 2 seconds, there is a problem
                if (index == oldIndex)
                {
                    if (timeout < 0)
                    {
                        Console.WriteLine("Finished reading at "
                                          + index + " of "
                                          + config.NoOfSamplesToRead
                                          + " samples.\nStop reading now.\n");
                        config.NoOfSamplesToRead = (index - 1) / config.ReadChannels;
                        ctsTokenSource.Cancel();
                    }
                    else
                    {
                        timeout--;
                    }
                }
                else
                {
                    timeout = 20;
                }

                Console.WriteLine(index + "\n");
            } while (index < (config.NoOfSamplesToRead * config.ReadChannels));
        }

        private void SetServoState(string axis, int[] state)
        {
            if (GCS2.SVO(ControllerId, axis, state ) == PI_RESULT_FAILURE )
            {
                throw (new GcsCommandError("SVO failed. Exiting"));
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
        #endregion private methods
    }

    internal class GcsCommandError : Exception
    {
        public GcsCommandError( string message )
            : base(message)
        {
        }
    }
}