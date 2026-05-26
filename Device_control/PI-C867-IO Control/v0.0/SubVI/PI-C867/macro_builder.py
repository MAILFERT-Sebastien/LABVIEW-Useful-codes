"""
macro_builder.py
----------------
Module de construction et d'envoi de macros GCS pour le contrôleur PI C-867.

Fournit deux fonctions :
  - build_macro() : genere le corps de la macro a partir d'un tableau de positions XY
  - send_macro()  : envoie la macro au controleur via RS-232

Auteur : Institut Fresnel / Photonics Platform
"""

import os
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

print(f"[DEBUG] DLL_PATH  = {DLL_PATH}")
print(f"[DEBUG] DLL exist = {os.path.exists(DLL_PATH)}")


def build_macro(positions, macro_name="MOVMAC"):
    lines = []
    for x, y in positions:
        lines.append("WAC DIO? 4 = 0")
        lines.append("WAC DIO? 4 = 1")
        lines.append(f"MOV 1 {x:.6f}")
        lines.append(f"MOV 2 {y:.6f}")
        lines.append("WAC ONT? 1 = 1")
        lines.append("WAC ONT? 2 = 1")
        lines.append("DIO 1 1")
        lines.append("DEL 10")
        lines.append("DIO 1 0")
    return lines


def send_macro(positions, macro_name="MOVMAC", comport=15, baudrate=9600, dry_run=False):
    body_lines = build_macro(positions, macro_name)

    commands = []
    commands.append(f"MAC BEG {macro_name}")
    commands.extend(body_lines)
    commands.append(f"MAC NSTART {macro_name}")
    commands.append(f"MAC END {macro_name}")
    commands.append(f"MAC NSTART {macro_name}")

    if dry_run:
        print("=== DRY RUN ===")
        for i, cmd in enumerate(commands):
            print(f"  {i+1:03d} : {cmd}")
        return

    with GCSDevice('C-867', gcsdll=DLL_PATH) as pidevice:
        pidevice.ConnectRS232(comport=comport, baudrate=baudrate)
        print(f"[C-867] Connecte : {pidevice.qIDN().strip()}")

        try:
            pidevice.GcsCommandset(f"MAC DEL {macro_name}")
            print(f"[C-867] Ancienne macro supprimee.")
        except Exception:
            print(f"[C-867] Aucune macro existante.")

        for cmd in commands:
            pidevice.GcsCommandset(cmd)
            print(f"[C-867] >> {cmd}")

        print(f"[C-867] Macro '{macro_name}' envoyee et demarree.")