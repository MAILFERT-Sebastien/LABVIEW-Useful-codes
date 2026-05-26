"""
stop_c867.py
------------
Arrêt propre du contrôleur PI C-867 :
  1. Connexion RS-232
  2. Arrêt immédiat de tous les mouvements (STP)
  3. Suppression de la macro en cours
  4. Servo OFF
  5. Fermeture de la connexion

Appelé depuis LabVIEW via System Exec.vi en fin d'expérience
ou en cas d'arrêt d'urgence.

Usage :
    python stop_c867.py [comport] [baudrate] [macro_name]

Codes de retour :
  0 : succès
  1 : erreur de connexion
  2 : erreur d'arrêt

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import os
from pipython import GCSDevice

# chemin vers la DLL 64 bits, relatif à l'emplacement de ce script
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")


def stop_c867(comport=15, baudrate=9600, macro_name="MOVMAC"):
    """
    Arrête proprement le C-867 : stop mouvement, stop macro,
    servo OFF, déconnexion.

    Paramètres
    ----------
    comport : int
        Numéro du port COM. Défaut : 15.
    baudrate : int
        Vitesse RS-232. Défaut : 9600.
    macro_name : str
        Nom de la macro à supprimer. Défaut : "MOVMAC".

    Retourne
    --------
    int : code de retour (0 = succès)
    """

    print(f"[STOP] Connexion au C-867 sur COM{comport}...")

    try:
        pidevice = GCSDevice('C-867', gcsdll=DLL_PATH)
        pidevice.ConnectRS232(comport=comport, baudrate=baudrate)
        print(f"[STOP] Connecté : {pidevice.qIDN().strip()}")
    except Exception as e:
        print(f"[ERREUR] Connexion impossible : {e}")
        return 1

    try:
        # arrêt immédiat de tous les mouvements
        pidevice.GcsCommandset("STP")
        print(f"[STOP] Tous les axes arrêtés.")

        # suppression de la macro pour éviter un redémarrage accidentel
        try:
            pidevice.GcsCommandset(f"MAC DEL {macro_name}")
            print(f"[STOP] Macro '{macro_name}' supprimée.")
        except Exception:
            print(f"[STOP] Aucune macro '{macro_name}' à supprimer.")

        # servo OFF sur les deux axes — libère les moteurs
        pidevice.GcsCommandset("SVO 1 0")
        pidevice.GcsCommandset("SVO 2 0")
        print(f"[STOP] Servo OFF.")

    except Exception as e:
        print(f"[ERREUR] Arrêt impossible : {e}")
        pidevice.CloseConnection()
        return 2

    pidevice.CloseConnection()
    print(f"[STOP] Connexion fermée. C-867 arrêté proprement.")
    return 0


if __name__ == "__main__":
    comport    = int(sys.argv[1]) if len(sys.argv) >= 2 else 15
    baudrate   = int(sys.argv[2]) if len(sys.argv) >= 3 else 9600
    macro_name = sys.argv[3]      if len(sys.argv) >= 4 else "MOVMAC"
    code = stop_c867(comport, baudrate, macro_name)
    sys.exit(code)