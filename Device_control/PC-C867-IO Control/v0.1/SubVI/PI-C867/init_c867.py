"""
init_c867.py
------------
Initialisation du contrôleur PI C-867 :
  1. Connexion RS-232
  2. Activation du servo sur les axes X et Y
  3. Référencement (Fast Reference Move)
  4. Attente de fin de référencement avec timeout

Appelé depuis LabVIEW via System Exec.vi en début d'expérience.

Usage :
    python init_c867.py [comport] [baudrate]

Codes de retour :
  0 : succès
  1 : erreur de connexion
  2 : erreur de servo
  3 : erreur de référencement
  4 : timeout référencement

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import time
import os
from pipython import GCSDevice

# chemin vers la DLL 64 bits, relatif à l'emplacement de ce script
_SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
DLL_PATH     = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")

# paramètres de référencement
FRF_POLL_S    = 0.2    # intervalle de polling (secondes)
FRF_TIMEOUT_S = 30     # timeout maximum (secondes)


def init_c867(comport=15, baudrate=9600):
    """
    Initialise le C-867 : connexion RS-232, servo ON, référencement.

    Paramètres
    ----------
    comport : int
        Numéro du port COM. Défaut : 15.
    baudrate : int
        Vitesse RS-232. Défaut : 9600.

    Retourne
    --------
    int : code de retour (0 = succès)
    """

    print(f"[INIT] Connexion au C-867 sur COM{comport} à {baudrate} bauds...")

    # connexion RS-232
    try:
        pidevice = GCSDevice('C-867', gcsdll=DLL_PATH)
        pidevice.ConnectRS232(comport=comport, baudrate=baudrate)
        print(f"[INIT] Connecté : {pidevice.qIDN().strip()}")
    except Exception as e:
        print(f"[ERREUR] Connexion impossible : {e}")
        return 1

    try:
        # activation du servo sur les deux axes
        pidevice.GcsCommandset("SVO 1 1")
        pidevice.GcsCommandset("SVO 2 1")
        print(f"[INIT] Servo ON — axes 1 et 2.")
    except Exception as e:
        print(f"[ERREUR] Activation servo impossible : {e}")
        pidevice.CloseConnection()
        return 2

    try:
        # lancement du référencement
        pidevice.GcsCommandset("FRF 1 2")
        print(f"[INIT] Référencement lancé...")
    except Exception as e:
        print(f"[ERREUR] Référencement impossible : {e}")
        pidevice.CloseConnection()
        return 3

    try:
        # attente de fin de référencement avec timeout
        debut = time.time()
        while True:

            # interroger l'état via la méthode PIPython native
            frf   = pidevice.qFRF()
            frf_x = frf.get('1', False)
            frf_y = frf.get('2', False)

            if frf_x and frf_y:
                print(f"[INIT] Référencement terminé.")
                break

            # vérifier le timeout
            if time.time() - debut > FRF_TIMEOUT_S:
                print(f"[ERREUR] Timeout référencement ({FRF_TIMEOUT_S}s dépassé).")
                pidevice.CloseConnection()
                return 4

            time.sleep(FRF_POLL_S)

    except Exception as e:
        print(f"[ERREUR] Attente référencement : {e}")
        pidevice.CloseConnection()
        return 3

    # fermeture propre — chaque script est autonome
    pidevice.CloseConnection()
    print(f"[INIT] Initialisation terminée avec succès.")
    return 0


if __name__ == "__main__":
    comport  = int(sys.argv[1]) if len(sys.argv) >= 2 else 15
    baudrate = int(sys.argv[2]) if len(sys.argv) >= 3 else 9600
    code = init_c867(comport, baudrate)
    sys.exit(code)