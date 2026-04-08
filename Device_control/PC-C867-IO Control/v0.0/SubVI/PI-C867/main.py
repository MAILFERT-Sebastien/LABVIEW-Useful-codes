"""
main.py
-------
Point d'entree du programme de pilotage multi-positions du C-867.
Envoie la macro et ferme la connexion immediatement.
Utiliser check_c867.py separement pour lire la position.

Usage :
    python main.py "[[0.1,0.1],[0.2,0.2]]" 15 115200

Codes de retour :
    0 : succes
    1 : erreur d'arguments
    2 : erreur d'envoi

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import json
import os
import time
from pipython import GCSDevice

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")


def parse_positions(json_string):
    """
    Parse une chaine JSON en liste de tuples (X, Y).
    Accepte [[X,Y]] ou [[X,Y,Z]] (Z ignore).

    Parametres
    ----------
    json_string : str
        Chaine JSON representant le tableau de positions.

    Retourne
    --------
    list of tuples
        Liste de tuples (X, Y) en mm.

    Leve
    ----
    ValueError
        Si la chaine JSON est invalide ou mal formee.
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


def build_macro(positions):
    """
    Genere le corps de la macro GCS.
    9 commandes par position.

    Syntaxe validee sur firmware C-867.2U2 v03.015 :
      JRC 0 DIO? 4 = 1  : boucle sur lui-meme tant que GO=1
      JRC -1 DIO? 4 = 0 : remonte d'une ligne tant que GO=0

    Impulsion GO minimum : 500ms.

    Parametres
    ----------
    positions : list of tuples
        Liste de tuples (X, Y) en mm.

    Retourne
    --------
    list of str
        Liste des commandes GCS du corps de la macro.
    """
    lines = []
    for x, y in positions:
        lines.append("JRC 0 DIO? 4 = 1")    # attendre que GO soit bas
        lines.append("JRC -1 DIO? 4 = 0")   # attendre front montant GO
        lines.append(f"MOV 1 {x:.6f}")       # axe X
        lines.append(f"MOV 2 {y:.6f}")       # axe Y
        lines.append("WAC ONT? 1 = 1")       # attendre stabilisation X
        lines.append("WAC ONT? 2 = 1")       # attendre stabilisation Y
        lines.append("DIO 1 1")               # READY HIGH
        lines.append("DEL 10")                # 10 ms
        lines.append("DIO 1 0")               # READY LOW
    return lines


def send_macro(dev, positions, macro_name):
    """
    Envoie la macro au controleur et la demarre.
    Verifie qMAC() avant MAC DEL pour eviter l'erreur 20.

    Parametres
    ----------
    dev : GCSDevice
        Instance connectee du controleur.
    positions : list of tuples
        Liste de tuples (X, Y) en mm.
    macro_name : str
        Nom de la macro.

    Retourne
    --------
    bool : True si succes, False si exception.
    """

    # position initiale
    pos = dev.qPOS()
    print(f"[POS] Position initiale : X={pos.get('1', 0):.6f} mm  Y={pos.get('2', 0):.6f} mm")

    # servo ON
    print(f"[MACRO] Servo ON...")
    dev.GcsCommandset("SVO 1 1")
    time.sleep(0.05)
    dev.GcsCommandset("SVO 2 1")
    time.sleep(0.05)

    # suppression ancienne macro si elle existe
    print(f"[MACRO] Verification macros existantes...")
    macros = dev.qMAC()
    print(f"[MACRO] Macros stockees : {macros if macros else 'aucune'}")
    if macro_name in str(macros):
        dev.GcsCommandset(f"MAC DEL {macro_name}")
        time.sleep(0.3)
        print(f"[MACRO] Macro '{macro_name}' supprimee.")
    else:
        print(f"[MACRO] Aucune macro '{macro_name}' a supprimer.")

    # construction de la sequence
    body_lines = build_macro(positions)
    commands = []
    commands.append(f"MAC BEG {macro_name}")
    commands.extend(body_lines)
    commands.append(f"MAC END {macro_name}")
    commands.append(f"MAC NSTART {macro_name}")  # demarrage effectif

    # envoi en rafale
    print(f"[MACRO] Envoi de {len(commands)} commandes...")
    try:
        for cmd in commands:
            dev.GcsCommandset(cmd)
            print(f"[C-867] >> {cmd}")
            time.sleep(0.05)
    except Exception as e:
        print(f"[ERREUR] Exception pendant envoi : {e}")
        return False

    time.sleep(0.5)
    print(f"[MACRO] Macro '{macro_name}' envoyee et demarree.")
    print(f"[INFO] Envoyer GO (HIGH 500ms puis LOW) sur PFI13.")
    print(f"[INFO] Utiliser check_c867.py pour lire la position apres chaque GO.")
    return True


def main():
    """Fonction principale."""

    if len(sys.argv) < 2:
        print("Usage : python main.py <positions_json> [comport] [baudrate] [macro_name]")
        sys.exit(1)

    json_string = sys.argv[1]
    comport     = int(sys.argv[2]) if len(sys.argv) >= 3 else 15
    baudrate    = int(sys.argv[3]) if len(sys.argv) >= 4 else 115200
    macro_name  = sys.argv[4]      if len(sys.argv) >= 5 else "MOVMAC"

    try:
        positions = parse_positions(json_string)
    except ValueError as e:
        print(f"[ERREUR] Positions invalides : {e}")
        sys.exit(1)

    print(f"[INFO] {len(positions)} position(s) recue(s).")
    for i, (x, y) in enumerate(positions):
        print(f"  Position {i+1:02d} : X={x:.6f} mm  Y={y:.6f} mm")

    try:
        with GCSDevice('C-867', gcsdll=DLL_PATH) as dev:

            print(f"[INFO] Connexion sur COM{comport} a {baudrate} bauds...")
            dev.ConnectRS232(comport=comport, baudrate=baudrate)
            print(f"[INFO] Connecte : {dev.qIDN().strip()}")

            if not send_macro(dev, positions, macro_name):
                print(f"[ERREUR] Echec envoi macro.")
                dev.CloseConnection()
                sys.exit(2)

            # fermeture immediate — la macro tourne de facon autonome
            dev.CloseConnection()
            print(f"[INFO] Connexion fermee. Macro active sur le controleur.")

    except Exception as e:
        print(f"[ERREUR] {e}")
        sys.exit(2)

    sys.exit(0)


if __name__ == "__main__":
    main()