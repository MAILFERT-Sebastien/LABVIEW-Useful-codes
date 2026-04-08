from pipython import GCSDevice
import os

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

for baud in [9600, 19200, 38400, 57600, 115200]:
    try:
        with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
            dev.ConnectRS232(comport=15, baudrate=baud)
            print(f"OK baudrate={baud} : {dev.qIDN().strip()}")
            break
    except Exception as e:
        print(f"ECHEC baudrate={baud} : {e}")