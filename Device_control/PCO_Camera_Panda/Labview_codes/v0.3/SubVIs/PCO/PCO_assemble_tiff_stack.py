import tifffile
import numpy as np
import os
import sys
import re
from datetime import datetime

# Ce script prend en argument le chemin d'un fichier CSV généré par LabVIEW,
# qui contient les chemins de toutes les images TIFF individuelles d'une acquisition.
# Il assemble ces images en un stack multi-TIFF, le sauvegarde avec un nom incrémental,
# puis nettoie les fichiers temporaires (images individuelles + CSV).
#
# Usage : python assemble_tiff_stack.py "C:\chemin\vers\Filenames.csv"

if len(sys.argv) < 2:
    print("Usage: python assemble_tiff_stack.py <chemin_vers_Filenames.csv>")
    sys.exit(1)

csv_path = sys.argv[1]

if not os.path.exists(csv_path):
    print(f"Fichier introuvable : {csv_path}")
    sys.exit(1)

# LabVIEW génère un CSV avec les chemins séparés par des tabulations sur une seule ligne
with open(csv_path, 'r') as f:
    content = f.read()
filepaths = [p.strip() for p in content.split('\t') if p.strip()]

# Le stack sera sauvegardé dans le même dossier que les images individuelles
# On lit ce dossier depuis le chemin de la première image dans le CSV
output_dir = os.path.dirname(filepaths[0])

print(f"{len(filepaths)} fichiers trouvés")

# Lecture de chaque image et assemblage dans une liste
# On affiche la progression tous les 50 fichiers pour ne pas rester dans le vide
images = []
for i, fp in enumerate(filepaths):
    img = tifffile.imread(fp)
    images.append(img)
    if (i+1) % 50 == 0:
        print(f"  {i+1}/{len(filepaths)} lues...")

# Conversion en tableau numpy 3D [N images, lignes, colonnes] en U16
stack = np.array(images, dtype=np.uint16)
print(f"Stack shape: {stack.shape}, dtype: {stack.dtype}")

# Nom du fichier de sortie : AAMMJJ_Acq_N.tif
# On regarde les fichiers déjà présents dans le dossier pour incrémenter N automatiquement
# Exemple : si 260506_Acq_1.tif et 260506_Acq_2.tif existent, on crée 260506_Acq_3.tif
date_str = datetime.now().strftime("%y%m%d")
base_pattern = re.compile(rf"^{date_str}_Acq_(\d+)\.tif$")

existing = [f for f in os.listdir(output_dir) if base_pattern.match(f)]
if existing:
    max_n = max(int(base_pattern.match(f).group(1)) for f in existing)
    next_n = max_n + 1
else:
    next_n = 1

output_path = os.path.join(output_dir, f"{date_str}_Acq_{next_n}.tif")

# Sauvegarde du stack en multi-TIFF 16 bits niveaux de gris
tifffile.imwrite(output_path, stack, photometric='minisblack')
print(f"Stack sauvegardé : {output_path}")

# Nettoyage : suppression des images individuelles et du CSV
# Ces fichiers sont temporaires, le stack contient tout
for fp in filepaths:
    if os.path.exists(fp):
        os.remove(fp)
os.remove(csv_path)
print(f"{len(filepaths)} fichiers individuels supprimés")
