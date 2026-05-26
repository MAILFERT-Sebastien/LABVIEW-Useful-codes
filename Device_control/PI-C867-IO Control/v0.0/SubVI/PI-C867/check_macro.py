"""
check_macro.py
--------------
Vérifie si la macro MOVMAC est stockée et tourne sur le C-867.

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import os
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

if __name__ == "__main__":
    comport  = int(sys.argv[1]) if len(sys.argv) >= 2 else 15
    baudrate = int(sys.argv[2]) if len(sys.argv) >= 3 else 9600

    with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
        dev.ConnectRS232(comport=comport, baudrate=baudrate)
        print(f"Connecté : {dev.qIDN().strip()}")

        # lister les macros stockées
        dev.GcsCommandset("MAC?")
        rep = dev.GcsGetAnswer()
        print(f"MAC? = {rep.strip()}")

        # lister les macros en cours d'exécution
        dev.GcsCommandset("RMC?")
        rep = dev.GcsGetAnswer()
        print(f"RMC? = {rep.strip()}")

        # position courante
        pos = dev.qPOS()
        print(f"X = {pos.get('1', 0.0):.6f} mm   Y = {pos.get('2', 0.0):.6f} mm")