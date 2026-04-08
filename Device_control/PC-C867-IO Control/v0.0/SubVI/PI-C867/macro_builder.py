"""
macro_builder.py
----------------
Module de construction et d'envoi de macros GCS pour le contrôleur PI C-867.

Auteur : Institut Fresnel / Photonics Platform
"""

import os
from pipython import GCSDevice

# chemin vers la DLL 64 bits, relatif à l'emplacement de ce script
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DLL_PATH    = os.path.join(_SCRIPT_DIR, "C867-VIs", "Low Level", "PI_GCS2_DLL_x64.dll")


def build_macro(positions):
    """
    Génère le corps de la macro GCS à partir d'un tableau de positions XY.

    Chaque position produit un bloc de 9 commandes GCS :
      - Attente front descendant sur GO (entrée TTL 4)
      - Attente front montant sur GO (entrée TTL 4)
      - Déplacement simultané axes 1 (X) et 2 (Y)
      - Attente fin de mouvement sur les deux axes (on-target)
      - Impulsion READY sur sortie TTL 1 (10 ms)

    Paramètres
    ----------
    positions : list of tuples
        Liste de tuples (X, Y) en mm.

    Retourne
    --------
    list of str
        Commandes GCS constituant le corps de la macro.
    """
    lines = []

    for x, y in positions:

        # attente signal GO depuis NI-6353
        lines.append("WAC DIO? 4 = 0")   # sécurité : attendre que GO soit bas
        lines.append("WAC DIO? 4 = 1")   # attendre front montant

        # déplacement simultané XY
        lines.append(f"MOV 1 {x:.6f}")   # axe 1 = X
        lines.append(f"MOV 2 {y:.6f}")   # axe 2 = Y

        # attente stabilisation en position
        lines.append("WAC ONT? 1 = 1")
        lines.append("WAC ONT? 2 = 1")

        # impulsion READY vers NI-6353
        lines.append("DIO 1 1")   # sortie TTL 1 → HIGH
        lines.append("DEL 10")    # 10 ms
        lines.append("DIO 1 0")   # sortie TTL 1 → LOW

    return lines


def send_macro(positions, macro_name="MOVMAC", serial="0122085469", dry_run=False):
    """
    Construit et envoie la macro GCS au contrôleur C-867 via USB.

    Paramètres
    ----------
    positions : list of tuples
        Liste de tuples (X, Y) en mm.
    macro_name : str
        Nom de la macro. Défaut : "MOVMAC".
    serial : str
        Numéro de série USB du C-867. Défaut : "0122085469".
    dry_run : bool
        Si True, affiche sans envoyer. Défaut : False.
    """
    body_lines = build_macro(positions)

    # assemblage séquence complète
    commands = []
    commands.append(f"MAC BEG {macro_name}")     # ouverture session
    commands.extend(body_lines)                   # corps de la macro
    commands.append(f"MAC NSTART {macro_name}")  # rebouclage (dans le corps)
    commands.append(f"MAC END {macro_name}")     # fermeture session
    commands.append(f"MAC NSTART {macro_name}")  # démarrage effectif

    if dry_run:
        print("=== DRY RUN — commandes qui seraient envoyées au C-867 ===")
        for i, cmd in enumerate(commands):
            print(f"  {i+1:03d} : {cmd}")
        print("=== FIN ===")
        return

    # envoi réel via PIPython + DLL explicite
    with GCSDevice('C-867', gcsdll=DLL_PATH) as pidevice:

        pidevice.ConnectUSB(serialnum=serial)
        print(f"[C-867] Connecté : {pidevice.qIDN().strip()}")

        # suppression ancienne macro (erreur ignorée si inexistante)
        try:
            pidevice.GcsCommandset(f"MAC DEL {macro_name}")
            print(f"[C-867] Ancienne macro '{macro_name}' supprimée.")
        except Exception:
            print(f"[C-867] Aucune macro '{macro_name}' à supprimer.")

        # envoi ligne par ligne
        for cmd in commands:
            pidevice.GcsCommandset(cmd)
            print(f"[C-867] >> {cmd}")

        print(f"[C-867] Macro '{macro_name}' envoyée et démarrée.")