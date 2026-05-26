from pipython import GCSDevice
import os

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
    dev.ConnectRS232(comport=15, baudrate=9600)
    print(f"OK : {dev.qIDN().strip()}")