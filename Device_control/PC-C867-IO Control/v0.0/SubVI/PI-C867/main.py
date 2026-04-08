"""
main.py
-------
Point d'entrée appelé par LabVIEW via System Exec.vi.
Reçoit les positions XY en JSON et envoie la macro au C-867.

Usage :
    python main.py "[[X0,Y0],[X1,Y1],...]" [serial] [macro_name]

Auteur : Institut Fresnel / Photonics Platform
"""

import sys
import json
from macro_builder import send_macro


def parse_positions(json_string):
    """
    Parse une chaîne JSON en liste de tuples (X, Y).

    Accepte les formats :
      [[X0,Y0], [X1,Y1], ...]          (sans Z)
      [[X0,Y0,Z0], [X1,Y1,Z1], ...]   (avec Z ignoré)
    """
    try:
        data = json.loads(json_string)
    except json.JSONDecodeError as e:
        raise ValueError(f"Erreur JSON : {e}")

    positions = []
    for i, item in enumerate(data):
        if len(item) < 2:
            raise ValueError(f"Position {i} : X et Y requis, reçu {item}")
        positions.append((float(item[0]), float(item[1])))

    return positions


def main():
    """
    Fonction principale — parse les arguments et envoie la macro.

    Codes de retour :
      0 : succès
      1 : erreur d'argument
      2 : erreur d'envoi
    """

    if len(sys.argv) < 2:
        print("Usage : python main.py <positions_json> [serial] [macro_name]")
        sys.exit(1)

    json_string = sys.argv[1]
    serial      = sys.argv[2] if len(sys.argv) >= 3 else "0122085469"
    macro_name  = sys.argv[3] if len(sys.argv) >= 4 else "MOVMAC"

    # parsing des positions
    try:
        positions = parse_positions(json_string)
    except ValueError as e:
        print(f"[ERREUR] Positions invalides : {e}")
        sys.exit(1)

    print(f"[INFO] {len(positions)} position(s) reçue(s) depuis LabVIEW.")
    for i, (x, y) in enumerate(positions):
        print(f"  Position {i+1:02d} : X={x:.6f} mm  Y={y:.6f} mm")

    # envoi de la macro
    try:
        send_macro(
            positions=positions,
            macro_name=macro_name,
            serial=serial,
            dry_run=False
        )
    except Exception as e:
        print(f"[ERREUR] Envoi macro impossible : {e}")
        sys.exit(2)

    sys.exit(0)


if __name__ == "__main__":
    main()