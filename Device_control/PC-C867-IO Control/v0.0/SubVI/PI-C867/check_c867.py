"""
check_c867.py
-------------
Lit et affiche la position courante du C-867.
A lancer apres chaque GO pour verifier le deplacement.

Usage :
    python check_c867.py [comport] [baudrate]

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import os
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

comport  = int(sys.argv[1]) if len(sys.argv) >= 2 else 15
baudrate = int(sys.argv[2]) if len(sys.argv) >= 3 else 115200

with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
    dev.ConnectRS232(comport=comport, baudrate=baudrate)
    pos = dev.qPOS()
    x   = pos.get('1', 0)
    y   = pos.get('2', 0)
    print(f"Position courante : X={x:.6f} mm  Y={y:.6f} mm")
    dev.CloseConnection()