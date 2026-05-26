/** <copyright file="ReadCommandSample.cs" company="Physik Instrumente (PI) SE & Co. KG">
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
 *  This sample sends a command using the text interface (GcsCommandSet) and reads the answer from the controller
 * </remarks>*/

using System;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using PI;

namespace ReadCommandSample
{
    public enum ConnectionType
    {
        Dialog,
        Rs232,
        Tcpip,
        Usb
    }

    public class ReadCommandSample
    {
        #region fields
        internal int ControllerId = -1;
        private const int PI_RESULT_FAILURE = 0;
        #endregion

        #region public methods
        public void Run()
        {
            try
            {
                OpenConnection(ConnectionType.Dialog);
                PrintControllerIdentification();
                SendCommandAndReadAnswer();
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

        private void SendCommandAndReadAnswer()
        {
            string commandToSend = "HLP?";

            if (GCS2.GcsCommandset(ControllerId, commandToSend) == PI_RESULT_FAILURE)
            {
                throw (new GcsCommandError("command failed. Exiting"));
            }

            while (true)
            {
                int answerSize = 0;
                if (GCS2.GcsGetAnswerSize(ControllerId, ref answerSize) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("command failed. Exiting"));
                }

                if (answerSize == 0)
                {
                    break;
                }

                var answer = new StringBuilder(answerSize);
                if (GCS2.GcsGetAnswer(ControllerId, answer, answerSize) == PI_RESULT_FAILURE)
                {
                    throw (new GcsCommandError("command failed. Exiting"));
                }

                Console.WriteLine(answer.ToString().TrimEnd('\n'));
            }
        }
        #endregion public methods

        #region private methods

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