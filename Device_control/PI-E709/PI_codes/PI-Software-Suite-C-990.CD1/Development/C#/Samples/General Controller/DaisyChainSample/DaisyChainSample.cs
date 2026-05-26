/** <copyright file="StaticLoadSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 *
 *  This sample shows how to use the C#-Wrapper for the PI_GCS2_DLL.dll and its methods,
 *  connect to a controller using connection dialog and read IDN.
 *
 *  </remarks> */

using System;
using System.Text;
using static System.Linq.Enumerable;
using PI;
using System.Linq;
using System.Collections.Generic;

namespace DaisyChainSample
{
    public enum ConnectionType
    {
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

    public class DaisyChainSample
    {
        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        internal int numberOfDaisyChainController  = 0;
        internal List<int> connections = new List<int>();
        #endregion fields

        #region public methods
        public void Run()
        {
            try
            {
                OpenDaisyChainMaster(ConnectionType.Rs232);
                ConnectDaisyChainDevices ();
                PrintControllerIdentification();
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
        #endregion public methods

        #region private methods

        private void ConnectDaisyChainDevices()
        {
            foreach ( int ID in Range(1, numberOfDaisyChainController) )
            {
                int connection = GCS2.ConnectDaisyChainDevice(ControllerId, ID);
                if ( connection >= 0 )
                {
                    connections.Add( connection );
                }
                else
                {
                    throw (new GcsCommandError("Connection to devices failed. Exiting."));
                }

            }
        }

        private void OpenDaisyChainMaster(ConnectionType type, string hostName = "localhost")
        {
            var IDNDaisyChainController = new StringBuilder(2024);

            Console.WriteLine("-- Try to Connect --");
            switch (type)
            {
                case ConnectionType.Rs232:
                    ControllerId = GCS2.OpenRS232DaisyChain(  1
                                                            , 115200
                                                            , ref numberOfDaisyChainController
                                                            , IDNDaisyChainController
                                                            , IDNDaisyChainController.MaxCapacity);
                    break;
                case ConnectionType.Tcpip:
                    ControllerId = GCS2.OpenTCPIPDaisyChain ( hostName
                                                            , 50000
                                                            , ref numberOfDaisyChainController
                                                            , IDNDaisyChainController
                                                            , IDNDaisyChainController.MaxCapacity );
                    break;
                case ConnectionType.Usb:
                {
                    var connectedUsbController = new StringBuilder(1024);
                    var noDevices = GCS2.EnumerateUSB(connectedUsbController, 1024, "");

                    Console.WriteLine("Found: " + noDevices + " USB controllers, connecting to the first one:");
                    Console.Write(connectedUsbController + "\n");

                    ControllerId = GCS2.OpenUSBDaisyChain(
                        connectedUsbController
                            .ToString()
                            .Split(new[] {Environment.NewLine}, StringSplitOptions.None)
                            [0]
                        , ref numberOfDaisyChainController
                        , IDNDaisyChainController
                        , IDNDaisyChainController.MaxCapacity);
                    break;
                }
                default:
                    throw new ArgumentOutOfRangeException(nameof(type), type, null);
            }
        }

        private void PrintControllerIdentification()
        {
            var controllerIdentification = new StringBuilder(1024);

            foreach ( int connection in connections )
            {
                if (GCS2.qIDN(ControllerId, controllerIdentification, controllerIdentification.Capacity) ==
                PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("qIDN failed. Exiting."));
                }

                Console.WriteLine("qIDN returned: " + controllerIdentification);
            }

        }

        private void DisconnectController()
        {
            if (ControllerId >= 0)
            {
                foreach ( int connection in connections )
                {
                    GCS2.CloseConnection(connection);
                }
                connections.Clear();
                GCS2.CloseDaisyChain(ControllerId);

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
        #endregion private methods
    }
}
