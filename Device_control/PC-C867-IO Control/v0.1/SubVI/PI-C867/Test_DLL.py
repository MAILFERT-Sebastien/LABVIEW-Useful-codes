from pipython import GCSDevice
import os

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
    devices = dev.EnumerateUSB(mask='C-867')
    print(f"DLL OK : {DLL_PATH}")
    print(f"Devices : {devices}")