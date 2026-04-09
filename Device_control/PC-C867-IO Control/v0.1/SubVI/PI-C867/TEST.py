import os
import time
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
    dev.ConnectRS232(comport=15, baudrate=115200)
    print(f"Connecte : {dev.qIDN().strip()}")
    
    err = dev.qERR()
    print(f"ERR? = {err}")
    
    macros = dev.qMAC()
    print(f"MAC? = {macros}")
    
    running = dev.qRMC()
    print(f"RMC? = {running}")
    
    pos = dev.qPOS()
    print(f"POS? = X={pos.get('1',0):.6f}  Y={pos.get('2',0):.6f}")