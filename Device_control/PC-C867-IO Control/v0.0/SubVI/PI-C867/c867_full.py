"""
c867_full.py
------------
Script unique qui :
  1. Initialise le C-867 (SVO + FRF)
  2. Envoie la macro MOVMAC
  3. Démarre la macro
  4. Vérifie que la macro tourne
  5. Affiche la position courante

Appelé depuis LabVIEW via System Exec.vi.

Usage :
    python c867_full.py <positions_json> [comport] [baudrate] [macro_name]

Exemple :
    python c867_full.py "[[0.1,0.1],[0.2,0.2],[0.3,0.3]]" 15 9600

Codes de retour :
  0 : succès
  1 : erreur d'arguments
  2 : erreur de connexion
  3 : erreur d'initialisation
  4 : erreur d'envoi macro

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import time
import json
import os
from pipython import GCSDevice

# chemin vers la DLL 64 bits, relatif à l'emplacement de ce script
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

# paramètres de référencement
FRF_POLL_S    = 0.2
FRF_TIMEOUT_S = 30


def parse_positions(json_string):
    """
    Parse une chaîne JSON en liste de tuples (X, Y).
    Accepte [[X,Y]] ou [[X,Y,Z]] (Z ignoré).
    """
    try:
        data = json.loads(json_string)
    except json.JSONDecodeError as e:
        raise ValueError(f"Erreur JSON : {e}")

    positions = []
    for i, item in enumerate(data):
        if len(item) < 2:
            raise ValueError(f"Position {i} invalide : X et Y requis")
        positions.append((float(item[0]), float(item[1])))
    return positions


def build_macro(positions, macro_name):
    """Génère le corps de la macro GCS."""
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


def main():
    """Fonction principale."""

    # vérification des arguments
    if len(sys.argv) < 2:
        print("Usage : python c867_full.py <positions_json> [comport] [baudrate] [macro_name]")
        sys.exit(1)

    json_string = sys.argv[1]
    comport     = int(sys.argv[2]) if len(sys.argv) >= 3 else 15
    baudrate    = int(sys.argv[3]) if len(sys.argv) >= 4 else 9600
    macro_name  = sys.argv[4]      if len(sys.argv) >= 5 else "MOVMAC"

    # parsing des positions
    try:
        positions = parse_positions(json_string)
    except ValueError as e:
        print(f"[ERREUR] Positions invalides : {e}")
        sys.exit(1)

    print(f"[INFO] {len(positions)} position(s) reçue(s).")
    for i, (x, y) in enumerate(positions):
        print(f"  Position {i+1:02d} : X={x:.6f} mm  Y={y:.6f} mm")

    # connexion unique — tout se passe dans ce bloc
    try:
        with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:

            # --- connexion ---
            print(f"[INIT] Connexion sur COM{comport} à {baudrate} bauds...")
            dev.ConnectRS232(comport=comport, baudrate=baudrate)
            print(f"[INIT] Connecté : {dev.qIDN().strip()}")

            # --- servo ON ---
            dev.GcsCommandset("SVO 1 1")
            dev.GcsCommandset("SVO 2 1")
            print(f"[INIT] Servo ON.")

            # --- référencement ---
            dev.GcsCommandset("FRF 1 2")
            print(f"[INIT] Référencement lancé...")

            # --- attente fin de référencement ---
            debut = time.time()
            while True:
                frf   = dev.qFRF()
                frf_x = frf.get('1', False)
                frf_y = frf.get('2', False)
                if frf_x and frf_y:
                    print(f"[INIT] Référencement terminé.")
                    break
                if time.time() - debut > FRF_TIMEOUT_S:
                    print(f"[ERREUR] Timeout référencement.")
                    sys.exit(3)
                time.sleep(FRF_POLL_S)

            # --- position après référencement ---
            pos = dev.qPOS()
            print(f"[INIT] Position : X={pos.get('1',0):.6f}  Y={pos.get('2',0):.6f}")

            # --- construction de la macro ---
            body_lines = build_macro(positions, macro_name)
            commands = []
            commands.append(f"MAC BEG {macro_name}")
            commands.extend(body_lines)
            commands.append(f"MAC NSTART {macro_name}")
            commands.append(f"MAC END {macro_name}")
            commands.append(f"MAC NSTART {macro_name}")

            # --- suppression ancienne macro ---
            try:
                dev.GcsCommandset(f"MAC DEL {macro_name}")
                print(f"[MACRO] Ancienne macro supprimée.")
            except Exception:
                print(f"[MACRO] Aucune macro existante.")

            # --- envoi de la macro ---
            print(f"[MACRO] Envoi en cours...")
            for cmd in commands:
                dev.GcsCommandset(cmd)
            print(f"[MACRO] Macro envoyée et démarrée.")

            # --- vérification ---
            dev.GcsCommandset("MAC?")
            macros = dev.GcsGetAnswer().strip()
            print(f"[MACRO] Macros stockées : {macros}")

            dev.GcsCommandset("RMC?")
            running = dev.GcsGetAnswer().strip()
            print(f"[MACRO] Macros en cours : {running if running else 'aucune'}")

            # --- fermeture explicite ---
            dev.CloseConnection()
            print(f"[INFO] Connexion fermée. Macro active sur le contrôleur.")

    except Exception as e:
        print(f"[ERREUR] {e}")
        sys.exit(2)

    sys.exit(0)


if __name__ == "__main__":
    main()