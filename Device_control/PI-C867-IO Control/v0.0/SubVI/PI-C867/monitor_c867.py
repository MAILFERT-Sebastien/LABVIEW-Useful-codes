"""
monitor_c867.py
---------------
Affiche en continu la position des axes 1 et 2 du C-867.
Utile pour vérifier les mouvements sans PIMikroMove.
Ctrl+C pour arrêter.

Usage :
    python monitor_c867.py [comport] [baudrate]

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import time
import os
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")


if __name__ == "__main__":
    comport  = int(sys.argv[1]) if len(sys.argv) >= 2 else 15
    baudrate = int(sys.argv[2]) if len(sys.argv) >= 3 else 9600

    print(f"[MONITOR] Connexion sur COM{comport} à {baudrate} bauds...")

    with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:
        dev.ConnectRS232(comport=comport, baudrate=baudrate)
        print(f"[MONITOR] Connecté : {dev.qIDN().strip()}")
        print(f"[MONITOR] Affichage position toutes les 200ms — Ctrl+C pour arrêter\n")

        try:
            while True:
                pos = dev.qPOS()
                x = pos.get('1', 0.0)
                y = pos.get('2', 0.0)
                print(f"  X = {x:.6f} mm   Y = {y:.6f} mm", end='\r')
                time.sleep(0.2)
        except KeyboardInterrupt:
            print(f"\n[MONITOR] Arrêté.")