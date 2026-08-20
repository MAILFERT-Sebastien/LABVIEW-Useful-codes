# -*- coding: utf-8 -*-
"""
PCO_assemble_stack.py
---------------------
Sauve un stack deja assemble en RAM cote LabVIEW (array 3D complet apres
acquisition) en TIFF multi-pages et/ou fichier MATLAB .mat, avec les
metadonnees d'acquisition embarquees.

AUCUNE retrocompatibilite avec l'ancien pipeline (CSV de TIFF individuels
ecrits un par un pendant l'acquisition) : LabVIEW doit desormais ecrire
l'array 3D complet en UN SEUL fichier binaire brut, en une seule fois,
apres l'acquisition (voir "Ce que LabVIEW doit ecrire" ci-dessous).

Usage :
    python PCO_assemble_stack.py <stack.bin> [--format tiff|mat|both]
                                              [--delete-originals]
                                              [--metadata-keys CLE1,CLE2,...]

--metadata-keys : restreint les metadonnees EMBARQUEES dans TIFF/.mat a
cette liste de cles (separees par des virgules), ex. :
    --metadata-keys date,exposure_time_s
Sans cette option (defaut), toutes les cles de metadata.json sont
embarquees comme avant — utile pour le pipeline principal (trajectory,
laser_power_mW, etc.). Avec cette option, utile par exemple pour un
controle PCO seul ou seules quelques cles simples ont un sens. "date"
est toujours conservee meme si absente de la liste (necessaire au
nommage des fichiers de sortie). Ne change rien a la lecture du .bin :
height/width/dtype/array_order restent lus normalement depuis
metadata.json quelle que soit cette option.

Ce que LabVIEW doit ecrire (a faire cote LabVIEW, pas dans ce script) :
    1. Le fichier binaire brut lui-meme : l'array 3D (N frames x H x W)
       ecrit en une seule fois avec "Write to Binary File" (pas d'en-tete,
       pas de structure TIFF — juste les pixels bruts, little-endian).
       Meme principe que les .bin de trajectoire deja utilises dans le
       projet (lus via des VI "Read from Binary File" chaines).
    2. Un fichier "metadata.json" dans le MEME dossier que CE SCRIPT
       (PCO_assemble_stack.py), PAS dans le dossier du .bin : LabVIEW
       ecrit toujours ce metadata.json au meme chemin fixe (a cote du
       .py), pas de plomberie de chemin dynamique necessaire cote
       LabVIEW pour cette ecriture-la — seul le chemin du .bin (passe en
       argument) change a chaque acquisition. Avec AU MINIMUM ces deux
       cles (sans elles, load_raw_stack refuse de
       lire le fichier — impossible de reconstruire la forme sinon) :
           "height"     : int  (H)
           "width"      : int  (W)
       num_frames n'est PAS necessaire : deduit automatiquement de la
       taille du fichier sur disque (h * w * taille_du_type). Si elle est
       presente quand meme (ex. recopiee depuis trajectory.num_points),
       elle sert juste de verification de coherence — c'est la taille du
       fichier qui fait foi en cas de desaccord.
       Cles optionnelles :
           "dtype"       : str, defaut "uint16" (voir _DTYPE_MAP pour les
                           valeurs reconnues)
           "array_order" : "NHW" (defaut, frame = axe le plus lent a
                           varier) ou "HWN" (frame = axe le plus rapide) —
                           A VERIFIER cote LabVIEW avant mise en prod (voir
                           note dans load_raw_stack), l'ordre d'ecriture
                           d'un array 3D LabVIEW en binaire n'etant pas
                           forcement le meme que l'ordre C de numpy.
           "date", "trajectory", "exposure_time_s", "laser_power_mW", etc.
           (n'importe quelle cle supplementaire est simplement recopiee
           dans les metadonnees embarquees / la struct MATLAB)

Formats de sortie (--format, defaut : tiff) :
    tiff : stack TIFF multi-pages, metadonnees en JSON dans le tag
           ImageDescription (lisible par tifffile, ImageJ/Fiji)
    mat  : fichier MATLAB .mat v7.3 (HDF5, non compresse) via hdf5storage,
           variables 'stack' (H, W, N) -> stack(:,:,k) dans MATLAB, et
           'metadata' (struct, imbriquee si besoin)
    both : les deux fichiers, meme indice auto-incremente
           (Scan_YYYYMMDD_005.tiff <-> Scan_YYYYMMDD_005.mat)

Convention de nommage (jamais d'ecrasement) :
    N frames  > 1 : Scan_YYYYMMDD_XXX.<ext>
    N frame  == 1 : Snap_YYYYMMDD_XXX.<ext>
    La date vient de metadata["date"] (ou date du jour si absente) ;
    l'indice XXX est le premier libre.

--delete-originals : supprime le fichier .bin source une fois le/les
fichier(s) de sortie verifies valides.

Dependances : numpy, tifffile (toujours) ; hdf5storage (formats mat/both).
Installees automatiquement au premier lancement si absentes (acces
internet requis dans ce cas — voir ensure_package).
"""

from __future__ import annotations

import sys
import os
import re
import json
import argparse
import importlib
import subprocess
from datetime import datetime


# ---------------------------------------------------------------------------
# Auto-installation des dépendances manquantes
# ---------------------------------------------------------------------------
def ensure_package(module_name: str, pip_name: str = None) -> None:
    """
    Importe un package, en l'installant automatiquement via pip s'il manque.

    Utilise le pip de l'interpréteur COURANT (sys.executable -m pip), de
    sorte que le package s'installe dans le même Python que celui invoqué
    par LabVIEW via System Exec.vi (piège classique sous Windows quand
    plusieurs installations Python coexistent — voir aussi la note sur le
    PATH Windows plus bas si System Exec.vi appelle juste "python").

    Tente d'abord une installation normale, puis se replie sur
    --break-system-packages (environnements PEP 668) et enfin --user
    (comptes Windows sans droits admin). Accès réseau nécessaire au
    premier lancement uniquement.
    """
    pip_name = pip_name or module_name

    # Cas nominal : le package est déjà installé, l'import suffit.
    try:
        importlib.import_module(module_name)
        return
    except ImportError:
        pass

    print(f"[INFO] Package '{module_name}' absent — installation de "
          f"'{pip_name}' via pip...")
    base = [sys.executable, "-m", "pip", "install", pip_name]

    # Trois tentatives avec options de repli successives.
    for extra in ([], ["--break-system-packages"], ["--user"]):
        try:
            subprocess.run(base + extra, check=True,
                           stdout=subprocess.DEVNULL)
            break
        except subprocess.CalledProcessError:
            continue
    else:
        # Aucune tentative n'a abouti : message avec la commande manuelle.
        raise RuntimeError(
            f"Installation automatique de '{pip_name}' impossible. "
            f"Installer manuellement : {sys.executable} -m pip install {pip_name}"
        )

    # Invalider le cache d'import puis vérifier que le package est utilisable.
    importlib.invalidate_caches()
    importlib.import_module(module_name)  # lève ImportError si toujours KO
    print(f"[INFO] '{pip_name}' installe avec succes.")


# Dépendances de base, vérifiées avant tout import tiers.
ensure_package("numpy")
ensure_package("tifffile")

import numpy as np
import tifffile


# ---------------------------------------------------------------------------
# Métadonnées
# ---------------------------------------------------------------------------
def load_metadata(meta_path: str) -> dict:
    """Charge les métadonnées depuis le fichier JSON, ou valeurs minimales."""
    meta = {}

    if os.path.isfile(meta_path):
        with open(meta_path, encoding='utf-8-sig') as f:
            meta = json.load(f)
        print(f"[INFO] Metadonnees chargees depuis : {meta_path}")
    else:
        print("[INFO] Pas de fichier de metadonnees — valeurs minimales.")

    # Une date est toujours garantie (sert au nommage du fichier de sortie).
    if "date" not in meta:
        meta["date"] = datetime.now().isoformat()

    return meta


def filter_metadata_keys(metadata: dict, keys: list[str] | None) -> dict:
    """
    Restreint les metadonnees a EMBARQUER dans TIFF/.mat a la liste
    'keys' si elle est fournie (option --metadata-keys). 'date' est
    toujours conservee (necessaire au nommage des fichiers de sortie).

    keys=None (option absente) : aucun filtrage, toutes les cles sont
    embarquees — comportement historique, utilise par le pipeline
    principal.

    Ne modifie PAS le dict passe en entree : le dict complet (avec
    height/width/dtype/array_order) reste disponible ailleurs pour la
    lecture du .bin, seul ce qui est ECRIT dans les fichiers de sortie
    est restreint ici.
    """
    if keys is None:
        return metadata

    allowed = set(keys) | {"date"}
    ignored = sorted(k for k in metadata if k not in allowed)
    if ignored:
        print(f"[INFO] Cles ignorees (hors --metadata-keys, non "
              f"embarquees) : {', '.join(ignored)}")

    return {k: v for k, v in metadata.items() if k in allowed}


# ---------------------------------------------------------------------------
# Lecture du stack (array 3D deja assemblee cote LabVIEW, un seul .bin)
# ---------------------------------------------------------------------------
_DTYPE_MAP = {
    "uint8": np.uint8, "uint16": np.uint16, "uint32": np.uint32,
    "int8": np.int8, "int16": np.int16, "int32": np.int32,
    "float32": np.float32, "float64": np.float64,
}


def _spatial_coherence_score(stack: np.ndarray, sample_frames: int = 5) -> float:
    """
    Mesure a quel point les pixels voisins d'un stack sont correles
    spatialement (comme dans une vraie image de camera, a cause de
    l'optique/PSF), sur un petit echantillon de frames pour rester rapide
    meme sur un gros stack.

    Score bas = coherent (differences pixel-a-pixel petites par rapport a
    la variance globale de la frame) = vraie image.
    Score proche de 1 = incoherent (aucune correlation locale) = octets
    d'un ordre NHW/HWN incorrect, melanges sans rapport spatial.
    """
    n = stack.shape[0]
    idxs = np.unique(np.linspace(0, n - 1, min(sample_frames, n)).astype(int))
    scores = []
    for i in idxs:
        f = stack[i].astype(np.float64)
        var = f.var()
        if var < 1e-9:
            continue  # frame plate (toutes valeurs egales) : pas informatif
        diff = np.diff(f, axis=1)  # difference entre pixels voisins (largeur)
        scores.append(diff.var() / var)
    return float(np.mean(scores)) if scores else 1.0


def _detect_array_order(raw: np.ndarray, n: int, h: int, w: int) -> str:
    """
    Determine automatiquement si le stack a ete ecrit en ordre 'NHW' ou
    'HWN' (voir load_raw_stack), en comparant la coherence spatiale des
    deux interpretations possibles — pas besoin de test manuel prealable
    cote LabVIEW.

    Les deux vues sont construites sans copie (reshape/transpose sont des
    vues numpy), seul le petit echantillon de frames utilise pour le score
    est reellement lu — reste rapide meme sur un stack de plusieurs Go.
    """
    stack_nhw = raw.reshape(n, h, w)
    stack_hwn = raw.reshape(h, w, n).transpose(2, 0, 1)

    score_nhw = _spatial_coherence_score(stack_nhw)
    score_hwn = _spatial_coherence_score(stack_hwn)

    order = "NHW" if score_nhw <= score_hwn else "HWN"
    print(f"[INFO] Detection auto de l'ordre du stack : NHW (score="
          f"{score_nhw:.3f}) vs HWN (score={score_hwn:.3f}) -> retenu : "
          f"{order} (score le plus bas = image spatialement coherente)")
    return order


def load_raw_stack(bin_path: str, metadata: dict) -> np.ndarray:
    """
    Lit le stack ecrit par LabVIEW en un seul fichier binaire brut
    (little-endian, pas d'en-tete). La forme et le type doivent etre
    fournis dans metadata.json — voir le docstring du module.

    Un seul np.fromfile() : lecture sequentielle unique, la plus rapide
    possible, pas de lecture fichier par fichier.

    L'ordre d'aplatissement (NHW ou HWN, voir _detect_array_order) est
    detecte AUTOMATIQUEMENT par coherence spatiale, sauf si "array_order"
    est explicitement fourni dans metadata.json (auquel cas il est
    utilise tel quel, sans detection — utile si la detection auto se
    trompe sur un type de donnees inhabituel).
    """
    for key in ("height", "width"):
        if key not in metadata:
            raise RuntimeError(
                f"metadata.json incomplet : cle '{key}' manquante "
                f"(attendu : height, width, et optionnellement "
                f"num_frames/dtype/array_order)."
            )

    h, w = metadata["height"], metadata["width"]
    dtype_name = metadata.get("dtype", "uint16")
    if dtype_name not in _DTYPE_MAP:
        raise RuntimeError(
            f"dtype '{dtype_name}' non reconnu dans metadata.json. "
            f"Valeurs valides : {sorted(_DTYPE_MAP)}"
        )
    dtype = _DTYPE_MAP[dtype_name]

    if not os.path.isfile(bin_path):
        raise RuntimeError(f"Fichier binaire introuvable : {bin_path}")

    itemsize = np.dtype(dtype).itemsize
    frame_bytes = h * w * itemsize
    actual_bytes = os.path.getsize(bin_path)

    # num_frames n'est PAS requis : deduit directement de la taille du
    # fichier sur disque (binaire brut sans en-tete = taille exacte =
    # n * h * w * itemsize). Evite de dupliquer une info deja presente
    # ailleurs (ex. trajectory.num_points) et tout risque d'incoherence
    # entre les deux.
    if actual_bytes % frame_bytes != 0:
        raise RuntimeError(
            f"Taille du fichier binaire ({actual_bytes} octets) non "
            f"divisible par la taille d'une frame ({h}x{w} en "
            f"{dtype_name} = {frame_bytes} octets). Verifier height/"
            f"width/dtype dans metadata.json."
        )
    n = actual_bytes // frame_bytes

    # Si num_frames est quand meme present (ex. recopie depuis
    # trajectory.num_points), on verifie juste la coherence sans
    # bloquer dessus — le fichier sur disque fait foi.
    if "num_frames" in metadata and metadata["num_frames"] != n:
        print(f"[WARNING] num_frames dans metadata.json "
              f"({metadata['num_frames']}) ne correspond pas au nombre de "
              f"frames deduit de la taille du fichier ({n}) — c'est {n} "
              f"qui est utilise (source de verite = taille du fichier).")

    raw = np.fromfile(bin_path, dtype=dtype)

    array_order = metadata.get("array_order")
    if array_order is None:
        array_order = _detect_array_order(raw, n, h, w)
    else:
        print(f"[INFO] array_order impose par metadata.json : {array_order} "
              f"(detection automatique desactivee)")

    if array_order == "NHW":
        stack = raw.reshape(n, h, w)
    elif array_order == "HWN":
        stack = raw.reshape(h, w, n).transpose(2, 0, 1)  # -> (N, H, W)
    else:
        raise RuntimeError(
            f"array_order '{array_order}' non reconnu (attendu 'NHW' ou "
            f"'HWN')."
        )

    print(f"[INFO] Stack lu depuis le binaire brut : {bin_path}  "
          f"{stack.shape}  dtype={stack.dtype}  (1 seule lecture sequentielle)")
    return stack


# ---------------------------------------------------------------------------
# Écriture TIFF
# ---------------------------------------------------------------------------
def save_stack_with_metadata(stack: np.ndarray, output_path: str,
                             metadata: dict):
    """
    Sauve le stack en TIFF multi-pages avec les métadonnées embarquées
    en JSON.
    """
    meta_json = json.dumps(metadata, indent=2, ensure_ascii=False)

    # TIFF compatible ImageJ, métadonnées dans le tag 'Info' d'ImageJ
    # (lisible via tif.imagej_metadata['Info'] dans tifffile,
    #  et via Image > Show Info dans ImageJ/FIJI)
    tifffile.imwrite(
        output_path,
        stack,
        imagej=True,
        metadata={'Info': meta_json},
    )

    print(f"[INFO] Stack sauve : {output_path}")
    print(f"[INFO] Metadonnees embarquees :\n{meta_json}")


# ---------------------------------------------------------------------------
# Écriture MATLAB .mat
# ---------------------------------------------------------------------------
def _sanitize_matlab_fieldname(name: str) -> str:
    """
    Convertit une clé JSON en nom de champ de struct MATLAB valide
    (lettres/chiffres/underscore, commence par une lettre, <= 63 car.).
    Les clés actuelles de metadata.json sont déjà valides ; ceci est un
    filet de sécurité pour d'éventuelles clés futures.
    """
    field = re.sub(r'[^0-9a-zA-Z_]+', '_', str(name)).strip('_')
    if not field:
        field = 'field'
    if not field[0].isalpha():
        field = 'f_' + field
    return field[:63]


def _metadata_to_matlab(metadata: dict) -> dict:
    """
    Prépare le dict de métadonnées pour l'écriture .mat :
      - les dicts imbriqués (ex. "trajectory") deviennent des structs
        MATLAB imbriquées (metadata.trajectory.type)
      - les valeurs None deviennent des chaînes vides (None non stockable)
      - les noms de champs sont assainis en identifiants MATLAB valides
    """
    struct = {}
    for key, value in metadata.items():
        field = _sanitize_matlab_fieldname(key)
        if value is None:
            value = ''
        elif isinstance(value, dict):
            # Récursion : dict imbriqué -> struct imbriquée
            value = _metadata_to_matlab(value)
        elif isinstance(value, (list, tuple)):
            # Liste numérique -> tableau ; liste hétérogène -> chaîne JSON
            try:
                value = np.asarray(value, dtype=float)
            except (ValueError, TypeError):
                value = json.dumps(value)
        struct[field] = value
    return struct


def save_stack_as_mat(stack: np.ndarray, output_path: str, metadata: dict):
    """
    Sauve le stack + métadonnées en fichier .mat.

    - stack transposé en (H, W, N) : convention MATLAB stack(:,:,k)
    - une frame unique est sauvée en tableau 2D (H, W)
    - les métadonnées sont sauvées en struct (éventuellement imbriquée)
    - format : toujours .mat v7.3 (HDF5, non compressé) via hdf5storage
    """
    # hdf5storage est vérifié/installé en amont dans main(), mais on garde
    # ce garde-fou si la fonction est appelée depuis un autre contexte.
    try:
        import hdf5storage
    except ImportError:
        raise RuntimeError(
            "--format mat necessite le package hdf5storage "
            "(sortie .mat v7.3) : pip install hdf5storage"
        )

    # Réorientation vers la convention MATLAB.
    if stack.ndim == 3 and stack.shape[0] == 1:
        mat_stack = stack[0]                        # (H, W) : snap
    elif stack.ndim == 3:
        mat_stack = np.transpose(stack, (1, 2, 0))  # (H, W, N) : scan
    else:
        mat_stack = stack                           # déjà 2D

    # Toujours en .mat v7.3 (HDF5) : les acquisitions dépassent couramment
    # la limite de 2 Go/variable du format v5 (>230 frames en 2048x2048
    # uint16). Compression volontairement DESACTIVEE : sur des données
    # caméra 12 bits bruitées elle ne gagne que ~20 % de taille mais rend
    # l'écriture ~5x plus lente, ce qui compte sur la machine
    # d'acquisition. MATLAB lit le v7.3 de façon transparente avec load().
    options = hdf5storage.Options(
        matlab_compatible=True,
        store_python_metadata=False,
        compress=False,
    )
    hdf5storage.writes(
        {'/stack': mat_stack,
         '/metadata': _metadata_to_matlab(metadata)},
        filename=output_path,
        options=options,
    )

    print(f"[INFO] Fichier MAT sauve : {output_path}  "
          f"(forme du stack dans MATLAB : {mat_stack.shape})")


# ---------------------------------------------------------------------------
# Vérification des fichiers de sortie
# ---------------------------------------------------------------------------
def verify_output_file(path: str) -> bool:
    """Vérifie qu'un fichier de sortie existe, est non vide et semble valide."""
    if not os.path.isfile(path) or os.path.getsize(path) == 0:
        return False
    try:
        low = path.lower()
        if low.endswith(('.tiff', '.tif')):
            # TIFF valide = au moins une page lisible
            with tifffile.TiffFile(path) as tif:
                return len(tif.pages) > 0
        if low.endswith('.mat'):
            with open(path, 'rb') as f:
                header = f.read(8)
            # En-tête v5 : 'MATLAB...' ; v7.3 : signature HDF5 \x89HDF
            return header.startswith(b'MATLAB') or header.startswith(b'\x89HDF')
    except Exception:
        return False
    return True


# ---------------------------------------------------------------------------
# Nommage auto-incrémenté (jamais d'écrasement)
# ---------------------------------------------------------------------------
# Prefixes et extensions connus, utilises pour detecter les indices deja
# pris — INDEPENDAMMENT du type d'acquisition (Scan/Snap) ou du format
# demande pour l'appel courant. Numerotation partagee : si Scan_..._003
# existe deja (meme si c'etait un .mat genere avec --format mat), le
# prochain Snap comme le prochain Scan prennent 004, jamais un Snap_001
# repartant de zero juste parce que c'est le premier Snap du dossier.
_ACQUISITION_PREFIXES = ("Scan", "Snap")
_KNOWN_EXTENSIONS = (".tiff", ".mat")


def next_available_path(output_dir: str, date_str: str, prefix: str) -> str:
    """
    Construit le chemin de sortie de base {prefix}_YYYYMMDD_XXX (sans
    extension), où XXX est un compteur à 3 chiffres démarrant à 001,
    PARTAGÉ entre tous les préfixes d'acquisition (_ACQUISITION_PREFIXES)
    et tous les formats de sortie connus (_KNOWN_EXTENSIONS) pour une
    même date/dossier — une seule séquence chronologique, quel que soit
    le type d'acquisition ou le format utilisé à chaque fois.

    Le compteur est incrémenté jusqu'à trouver un indice n pour lequel
    AUCUN fichier {prefix}_{date}_{n}{ext} n'existe déjà, quel que soit
    prefix parmi _ACQUISITION_PREFIXES et ext parmi _KNOWN_EXTENSIONS —
    de sorte que Scan_..._005.tiff et Scan_..._005.mat désignent
    toujours la même acquisition quand --format both est utilisé, et
    qu'un Snap_..._005 ne puisse jamais coexister avec un Scan_..._005.
    """
    # Extraction de YYYYMMDD depuis la date des métadonnées, robuste aux
    # formats ISO date seule ou date+heure.
    try:
        date_part = date_str.split("T")[0]
        date_compact = date_part.replace("-", "")
        datetime.strptime(date_compact, "%Y%m%d")  # validation du format
    except (ValueError, AttributeError, IndexError):
        print(f"[WARNING] Date '{date_str}' non interpretable pour le nom "
              f"de sortie — utilisation de la date du jour.")
        date_compact = datetime.now().strftime("%Y%m%d")

    n = 1
    while True:
        taken = any(
            os.path.exists(os.path.join(
                output_dir, f"{p}_{date_compact}_{n:03d}{ext}"))
            for p in _ACQUISITION_PREFIXES
            for ext in _KNOWN_EXTENSIONS
        )
        if not taken:
            return os.path.join(output_dir, f"{prefix}_{date_compact}_{n:03d}")
        n += 1


# ---------------------------------------------------------------------------
# Suppression optionnelle du fichier source
# ---------------------------------------------------------------------------
def delete_original(bin_path: str, output_paths: list[str]) -> None:
    """
    Supprime le fichier .bin source, avec garde-fous :
      - vérifie que CHAQUE fichier de sortie (tiff et/ou mat) existe,
        est non vide et semble valide avant toute suppression
      - ne supprime jamais un fichier de sortie lui-même (défensif)
    """
    bad = [p for p in output_paths if not verify_output_file(p)]
    if bad:
        print(f"[ERROR] Fichier(s) de sortie manquant(s), vide(s) ou "
              f"invalide(s) ({', '.join(bad)}) — suppression du .bin "
              f"source annulee par securite.")
        return

    bin_abs = os.path.abspath(bin_path)
    if bin_abs in {os.path.abspath(p) for p in output_paths}:
        # Ne devrait jamais arriver, mais on ne supprime jamais une sortie
        return

    try:
        os.remove(bin_path)
        print(f"[INFO] Fichier source supprime : {bin_path}")
    except OSError as e:
        print(f"[WARNING] Suppression impossible {bin_path} : {e}")


# ---------------------------------------------------------------------------
# Programme principal
# ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Sauve un stack deja assemble en RAM (LabVIEW) en "
                    "TIFF et/ou MATLAB .mat."
    )
    parser.add_argument(
        "bin_path",
        help="Fichier binaire brut contenant le stack (array 3D ecrite "
             "en une seule fois par LabVIEW). metadata.json doit se "
             "trouver a cote de ce script (PAS dans le dossier du .bin), "
             "avec au moins height/width — voir le docstring du module.",
    )
    parser.add_argument(
        "--format",
        dest="fmt",
        default="tiff",
        choices=["tiff", "mat", "both"],
        help="Format de sortie : 'tiff' (defaut), 'mat', ou 'both'.",
    )
    parser.add_argument(
        "--delete-originals",
        action="store_true",
        help="Supprime le fichier .bin source apres sauvegarde et "
             "verification du/des fichier(s) de sortie.",
    )
    parser.add_argument(
        "--metadata-keys",
        dest="metadata_keys",
        default=None,
        help="Liste de cles (separees par des virgules, ex. "
             "'date,exposure_time_s') a restreindre pour les "
             "metadonnees embarquees dans TIFF/.mat. Par defaut : pas "
             "de restriction, toutes les cles de metadata.json sont "
             "embarquees.",
    )
    args = parser.parse_args()

    # Vérifie/installe la dépendance .mat TOT (avant de charger des
    # gigaoctets en RAM) : un package manquant échoue en quelques
    # secondes, pas après plusieurs minutes.
    if args.fmt in ("mat", "both"):
        ensure_package("hdf5storage")

    bin_path = args.bin_path

    if not os.path.isfile(bin_path):
        print(f"[ERROR] Fichier binaire introuvable : {bin_path}")
        sys.exit(1)

    # metadata.json est cherche a cote de CE SCRIPT (chemin fixe, ecrit
    # par LabVIEW a chaque run), PAS dans le dossier du .bin — seul le
    # .bin a un chemin dynamique par acquisition (passe en argument).
    bin_dir = os.path.dirname(os.path.abspath(bin_path))
    script_dir = os.path.dirname(os.path.abspath(__file__))
    meta_path = os.path.join(script_dir, "metadata.json")
    metadata = load_metadata(meta_path)

    # --- Lecture du stack (une seule lecture sequentielle) ---
    stack = load_raw_stack(bin_path, metadata)
    metadata["num_frames"] = stack.shape[0]

    # --- Chemin de sortie (numerotation partagee Scan_/Snap_) ---
    prefix = "Snap" if stack.shape[0] == 1 else "Scan"
    base = next_available_path(bin_dir, metadata["date"], prefix=prefix)

    # --- Metadonnees a embarquer (eventuellement restreintes) ---
    metadata_keys = None
    if args.metadata_keys:
        metadata_keys = [k.strip() for k in args.metadata_keys.split(",")
                         if k.strip()]
    embedded_metadata = filter_metadata_keys(metadata, metadata_keys)

    # --- Sauvegarde ---
    output_paths = []
    if args.fmt in ("tiff", "both"):
        save_stack_with_metadata(stack, base + ".tiff", embedded_metadata)
        output_paths.append(base + ".tiff")
    if args.fmt in ("mat", "both"):
        save_stack_as_mat(stack, base + ".mat", embedded_metadata)
        output_paths.append(base + ".mat")

    # --- Nettoyage optionnel du fichier source ---
    if args.delete_originals:
        delete_original(bin_path, output_paths)

    print("\n[DONE]")


if __name__ == "__main__":
    main()
