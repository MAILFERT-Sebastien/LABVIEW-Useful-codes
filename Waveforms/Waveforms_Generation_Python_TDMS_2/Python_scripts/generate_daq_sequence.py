#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# generate_daq_sequence.py
# -----------------------------------------------------------------------------
# Genere les tableaux AO et DO pour les taches DAQmx finite du systeme
# LABVIEW_Polar_v2.0.
#
# Ordre d'acquisition : POS -> Z -> C -> POL
# (Les timepoints sont geres par CTR0 qui repete la sequence, pas par ce script)
#
# Tableaux produits :
#   ao_master.bin  : float64, shape (N, 4)
#       col 0 : AO0#1  laser 405 nm        (0-5 V)
#       col 1 : AO1#1  XY_X platine        (reserve, 0 V)
#       col 2 : AO2#1  Pockels             (-1 a +1 V)
#       col 3 : AO3#1  Z piezo             (0-10 V)
#
#   ao_slave.bin   : float64, shape (N, 3)
#       col 0 : AO0#2  laser 488 nm        (0-5 V)
#       col 1 : AO1#2  laser 561 nm        (0-5 V)
#       col 2 : AO2#2  laser 640 nm        (0-5 V)
#
#   do_master.bin  : uint8, shape (N, 8)
#       col 0 : P0.0   pulse filtre        (1=HIGH pendant N ms)
#       col 1 : P0.1   laser 405 ON/OFF    (reserve, 0)
#       col 2 : P0.2   laser 455 ON/OFF    (reserve, 0)
#       col 3 : P0.3   blanking            (1=lasers OFF, 0=lasers ON)
#       col 4 : P0.4   trigger camera      (1=exposition, 0=readout)
#       col 5 : P0.5   laser 488 ON/OFF    (reserve, 0)
#       col 6 : P0.6   laser 561 ON/OFF    (=1 pendant expo 561 nm)
#       col 7 : P0.7   laser 640 ON/OFF    (reserve, 0)
#
#   sequence_info.json : metadonnees completes
#
# Mapping laser -> voie AO :
#   405 nm -> ao_master col 0
#   455 nm -> ao_master col 1  (reserve XY_X, non utilisable comme laser)
#   488 nm -> ao_slave  col 0
#   561 nm -> ao_slave  col 1  + do_master col 6
#   640 nm -> ao_slave  col 2
#
# Puissance : power_pct (0-100%) -> V = power_pct / 100.0 * 5.0
#
# Polar array : matrice [N_pol lignes x 4 colonnes (lasers 405/488/561/640)]
#   Si canal polar_on  -> utilise colonne correspondant au laser, N_pol lignes
#   Si canal polar_off -> npol=1, Pockels=0V
#
# Pulse filtre :
#   canal_id x 10 + filter_number ms (ex: canal 2 filtre 3 -> 23 ms)
#   1 canal actif  -> pulse une seule fois au debut de toute la sequence
#   2+ canaux      -> pulse avant chaque canal a chaque Z
#
# Usage :
#   python generate_daq_sequence.py --config params.json --out ./output
#   python generate_daq_sequence.py --test --out ./output
#
# Auteur  : Sebastien MAILFERT - CNRS
# Version : v2.0
# -----------------------------------------------------------------------------

import argparse
import json
import datetime
import sys
from pathlib import Path

import numpy as np
try:
    from nptdms import TdmsWriter, ChannelObject
    HAS_TDMS = True
except ImportError:
    HAS_TDMS = False

SCRIPT_NAME    = "generate_daq_sequence.py"
SCRIPT_VERSION = "v2.0"

# ---------------------------------------------------------------------------
# Constantes
# ---------------------------------------------------------------------------

POWER_PCT_TO_V = 5.0 / 100.0   # 100% = 5V

# Colonnes ao_master
COL_AOM_LASER405 = 0
COL_AOM_XY_X     = 1   # reserve
COL_AOM_POCKELS  = 2
COL_AOM_Z        = 3
N_AO_MASTER      = 4

# Colonnes ao_slave
COL_AOS_LASER488 = 0
COL_AOS_LASER561 = 1
COL_AOS_LASER640 = 2
N_AO_SLAVE       = 3

# Colonnes do_master
COL_DO_FILTRE    = 0
COL_DO_L405      = 1   # reserve
COL_DO_L455      = 2   # reserve
COL_DO_BLANKING  = 3
COL_DO_TRIGGER   = 4
COL_DO_L488      = 5   # reserve
COL_DO_L561      = 6   # actif pour laser 561
COL_DO_L640      = 7   # reserve
N_DO_MASTER      = 8

# Mapping laser_nm -> (ao_card, ao_col, do_col)
# do_col = -1 si pas de DO pour ce laser
# Note : laser 455 nm (index 1) est reserve pour XY_X, non utilisable comme laser
LASER_MAP = {
    405: ("master", COL_AOM_LASER405, -1),
    488: ("slave",  COL_AOS_LASER488, -1),
    561: ("slave",  COL_AOS_LASER561, COL_DO_L561),
    640: ("slave",  COL_AOS_LASER640, -1),
}


# ---------------------------------------------------------------------------
# Utilitaires
# ---------------------------------------------------------------------------

def ms_to_samples(ms: float, rate: int) -> int:
    """Convertit une duree en ms en samples (arrondi superieur, min 1)."""
    return max(1, int(np.ceil(ms * rate / 1000.0)))


def s_to_samples(s: float, rate: int) -> int:
    """Convertit une duree en secondes en samples (arrondi superieur, min 1)."""
    return ms_to_samples(s * 1000.0, rate)


# ---------------------------------------------------------------------------
# Parsing et validation des parametres d'entree
# ---------------------------------------------------------------------------

def parse_params(raw: dict) -> dict:
    """
    Valide et normalise les parametres d'entree depuis le JSON LabVIEW.

    Cles JSON LabVIEW attendues :
      "Sample rate OUT"              : int   (Hz)
      "Start/Live (0/1) OUT"         : bool  (true=live, false=acquisition)
      "Channel_Cluster_Array OUT"    : list  (3 canaux)
        "Channel ON-OFF"             : bool
        "Laser"                      : int   (0=405,1=455,2=488,3=561,4=640)
        "Power (%)"                  : float (0-100)
        "Exposure time (s)"          : float
        "Polar ON-OFF"               : bool
        "Filter"                     : int   (1-6)
      "Readout time (s) OUT"         : float
      "Extra time for FW (s) OUT"    : float
      "Polar_Array OUT"              : [[N_pol x 5]] (cols: 405,455,488,561,640)
      "Z_Array OUT"                  : [float, ...]  (V, longueur = NZ)
      "Multipositions current OUT"   : [x_v, y_v]   (position actuelle platine)
      "Multipositions array OUT"     : [[x,y], ...]  (vide = pas de multipos)
    """
    # Mapping index laser LabVIEW -> nm
    LASER_IDX_TO_NM = {0: 405, 1: 455, 2: 488, 3: 561, 4: 640}

    # Mapping laser_nm -> index colonne dans Polar_Array [N_pol x 5]
    LASER_NM_TO_POLAR_COL = {405: 0, 455: 1, 488: 2, 561: 3, 640: 4}

    rate = int(raw.get("Sample rate OUT", 10000))
    if rate <= 0:
        raise ValueError(f"Sample rate invalide : {rate}")

    live_mode = bool(raw.get("Start/Live (0/1) OUT", False))

    # -- Canaux --------------------------------------------------------------
    raw_channels = raw.get("Channel_Cluster_Array OUT", [])
    if len(raw_channels) != 3:
        raise ValueError(
            f"'Channel_Cluster_Array OUT' doit contenir 3 entrees, recu {len(raw_channels)}"
        )

    channels_active = []
    for i, ch in enumerate(raw_channels):
        if not ch.get("Channel ON-OFF", False):
            continue
        laser_idx = int(ch["Laser"])
        if laser_idx not in LASER_IDX_TO_NM:
            raise ValueError(f"Canal {i+1} : Laser index={laser_idx} inconnu (0-4).")
        laser_nm = LASER_IDX_TO_NM[laser_idx]
        if laser_nm not in LASER_MAP:
            raise ValueError(
                f"Canal {i+1} : laser {laser_nm}nm non supporte par le hardware "
                f"(laser 455nm reserve XY)."
            )
        fn = int(ch.get("Filter", 0))
        # LabVIEW envoie un index 0-based (0-5), on convertit en 1-based (1-6)
        if fn < 0 or fn > 5:
            raise ValueError(f"Canal {i+1} : Filter={fn} invalide (0-5).")
        fn = fn + 1   # 0-based -> 1-based
        power_v = float(ch.get("Power (%)", 0)) * POWER_PCT_TO_V
        channels_active.append({
            "channel_id":    i + 1,
            "laser_nm":      laser_nm,
            "laser_idx":     laser_idx,
            "power_v":       power_v,
            "t_expo_s":      float(ch["Exposure time (s)"]),
            "polar_on":      bool(ch.get("Polar ON-OFF", False)),
            "filter_number": fn,
            "pulse_ms":      (i + 1) * 10 + fn,
        })

    if not channels_active:
        raise ValueError("Aucun canal actif.")

    # -- Polar array ---------------------------------------------------------
    # Format LabVIEW : [N_pol lignes x 5 colonnes (405,455,488,561,640)]
    raw_polar = raw.get("Polar_Array OUT", None)
    if raw_polar is not None and len(raw_polar) > 0:
        polar_array = np.array(raw_polar, dtype=np.float64)
        if polar_array.ndim != 2 or polar_array.shape[1] != 5:
            raise ValueError(
                f"Polar_Array OUT doit etre [N_pol x 5], recu shape={polar_array.shape}"
            )
    else:
        polar_array = np.zeros((1, 5), dtype=np.float64)

    # Affecter npol et pockels_v a chaque canal
    for ch in channels_active:
        if ch["polar_on"]:
            pcol = LASER_NM_TO_POLAR_COL[ch["laser_nm"]]
            ch["npol"]      = polar_array.shape[0]
            ch["pockels_v"] = polar_array[:, pcol].tolist()
        else:
            ch["npol"]      = 1
            ch["pockels_v"] = [0.0]

    # -- Z array -------------------------------------------------------------
    # Longueur du tableau fait foi :
    #   [v]           -> NZ=1, pas de Z stack
    #   [v1, v2, ...] -> NZ=n, Z stack actif
    # En live mode, on utilise uniquement la premiere valeur.
    z_raw = np.array(raw.get("Z_Array OUT", [0.0]), dtype=np.float64).flatten()
    z_array = z_raw   # Z stack toujours pris en compte, meme en live
    NZ = len(z_array)

    # -- XY array ------------------------------------------------------------
    # Position actuelle (toujours presente) : utilisee quand pas de multipos
    xy_current = np.array(
        raw.get("Multipositions current OUT", [0.0, 0.0]),
        dtype=np.float64
    ).flatten()
    if len(xy_current) != 2:
        xy_current = np.array([0.0, 0.0], dtype=np.float64)

    # Tableau multipositions : vide ou [[]] = pas de multiposition
    raw_xy = raw.get("Multipositions array OUT", [[]])
    # Filtrer les lignes vides (LabVIEW envoie [[]] quand vide)
    xy_rows = [row for row in raw_xy if len(row) == 2]

    if len(xy_rows) == 0 or live_mode:
        # Pas de multiposition : rester a la position actuelle
        xy_array = xy_current.reshape(1, 2)
    else:
        xy_array = np.array(xy_rows, dtype=np.float64)
        if xy_array.shape[1] != 2:
            raise ValueError(f"Multipositions array OUT doit etre [NPOS x 2]")
    NPOS = len(xy_array)

    # -- Timing --------------------------------------------------------------
    t_readout_s = float(raw.get("Readout time (s) OUT", 0.01))
    extratime_s = float(raw.get("Extra time for FW (s) OUT", 0.15))

    # Regle pulse filtre :
    # 1 canal actif  -> pulse une seule fois au debut (avant Z1)
    # 2+ canaux      -> pulse avant chaque canal a chaque Z
    single_channel_mode = (len(channels_active) == 1)

    return {
        "sample_rate_hz":      rate,
        "live_mode":           live_mode,
        "channels_active":     channels_active,
        "polar_array":         polar_array,
        "z_array":             z_array,
        "xy_array":            xy_array,
        "xy_current":          xy_current,
        "NZ":                  NZ,
        "NPOS":                NPOS,
        "t_readout_s":         t_readout_s,
        "extratime_s":         extratime_s,
        "single_channel_mode": single_channel_mode,
    }


# ---------------------------------------------------------------------------
# Generateur de sequence
# ---------------------------------------------------------------------------

class SequenceBuilder:
    """
    Construit les tableaux AO et DO selon l'ordre POS -> Z -> C -> POL.
    Les timepoints sont geres par CTR0, pas par ce script.
    """

    def __init__(self, p: dict):
        self.p    = p
        self.rate = p["sample_rate_hz"]

    def _ns(self, s):  return s_to_samples(s,  self.rate)
    def _nm(self, ms): return ms_to_samples(ms, self.rate)

    def _n_pulse(self, ch):   return self._nm(ch["pulse_ms"])
    def _n_extratime(self):   return self._ns(self.p["extratime_s"])
    def _n_expo(self, ch):    return self._ns(ch["t_expo_s"])
    def _n_readout(self):     return self._ns(self.p["t_readout_s"])

    def _n_filter_total(self, ch) -> int:
        """
        Duree totale du changement de filtre = extratime_s.
        Structure :
          P0.0=HIGH pendant pulse_ms
          P0.0=LOW  pendant (extratime_s - pulse_ms/1000)  [blanking reste a 1]
        Total = extratime_s (constant quel que soit le canal/filtre).
        """
        return self._n_extratime()

    def _n_pulse_high(self, ch) -> int:
        """Duree du pulse HIGH (P0.0=1) = canal x 10 + filtre (ms)."""
        return self._n_pulse(ch)

    def _n_pulse_low(self, ch) -> int:
        """Duree du reste du blanking apres le pulse (P0.0=0, blanking=1)."""
        return self._n_extratime() - self._n_pulse(ch)

    def _n_channel(self, ch) -> int:
        """Samples pour un canal a un Z donne (sans changement de filtre)."""
        return ch["npol"] * (self._n_expo(ch) + self._n_readout())

    def _n_z_block(self, include_filter: bool) -> int:
        """Samples pour un bloc Z complet."""
        n = 0
        for ch in self.p["channels_active"]:
            if include_filter:
                n += self._n_filter_total(ch)
            n += self._n_channel(ch)
        return n

    def _total_samples(self) -> int:
        p   = self.p
        NZ  = p["NZ"]
        NPOS= p["NPOS"]
        sc  = p["single_channel_mode"]

        if sc:
            # Changement de filtre une seule fois au debut par POS
            n_filter_total = self._n_filter_total(p["channels_active"][0])
            n_per_pos = n_filter_total + NZ * self._n_z_block(include_filter=False)
        else:
            # Changement de filtre avant chaque canal a chaque Z
            n_per_pos = NZ * self._n_z_block(include_filter=True)

        return NPOS * n_per_pos

    def build(self) -> tuple:
        """
        Retourne (ao_master, ao_slave, do_master).
        ao_master : float64 (N, 4)
        ao_slave  : float64 (N, 3)
        do_master : uint8   (N, 8)
        """
        p    = self.p
        N    = self._total_samples()
        ao_m = np.zeros((N, N_AO_MASTER), dtype=np.float64)
        ao_s = np.zeros((N, N_AO_SLAVE),  dtype=np.float64)
        do_m = np.zeros((N, N_DO_MASTER), dtype=np.uint8)

        # Blanking a 1 par defaut (lasers OFF)
        do_m[:, COL_DO_BLANKING] = 1

        cur = 0
        sc  = p["single_channel_mode"]

        for pos_idx in range(p["NPOS"]):

            # -- Mode single canal : changement filtre une seule fois par POS --
            if sc:
                ch  = p["channels_active"][0]
                nph = self._n_pulse_high(ch)
                npl = self._n_pulse_low(ch)
                # Pulse HIGH
                do_m[cur:cur+nph, COL_DO_FILTRE]   = 1
                do_m[cur:cur+nph, COL_DO_BLANKING] = 1
                cur += nph
                # Reste blanking LOW (P0.0=0, blanking reste 1)
                do_m[cur:cur+npl, COL_DO_FILTRE]   = 0
                do_m[cur:cur+npl, COL_DO_BLANKING] = 1
                cur += npl

            for z_idx in range(p["NZ"]):
                z_v = p["z_array"][z_idx]

                for ch in p["channels_active"]:
                    nph = self._n_pulse_high(ch)
                    npl = self._n_pulse_low(ch)
                    ne  = self._n_expo(ch)
                    nr  = self._n_readout()

                    ao_card, ao_col, do_col_laser = LASER_MAP[ch["laser_nm"]]

                    # -- Changement de filtre (sauf mode single canal) --------
                    if not sc:
                        ao_m[cur:cur+nph, COL_AOM_Z]      = z_v
                        do_m[cur:cur+nph, COL_DO_FILTRE]   = 1
                        do_m[cur:cur+nph, COL_DO_BLANKING] = 1
                        cur += nph
                        # Reste blanking (P0.0=0, blanking reste 1)
                        ao_m[cur:cur+npl, COL_AOM_Z]      = z_v
                        do_m[cur:cur+npl, COL_DO_FILTRE]   = 0
                        do_m[cur:cur+npl, COL_DO_BLANKING] = 1
                        cur += npl

                    # -- Expositions ------------------------------------------
                    for pol_idx in range(ch["npol"]):
                        v_pockels = ch["pockels_v"][pol_idx]

                        # Exposition : blanking=0, laser ON, trigger=1
                        ao_m[cur:cur+ne, COL_AOM_Z]       = z_v
                        ao_m[cur:cur+ne, COL_AOM_POCKELS]  = v_pockels
                        if ao_card == "master":
                            ao_m[cur:cur+ne, ao_col] = ch["power_v"]
                        else:
                            ao_s[cur:cur+ne, ao_col] = ch["power_v"]
                        if do_col_laser >= 0:
                            do_m[cur:cur+ne, do_col_laser] = 1   # 561 DO
                        do_m[cur:cur+ne, COL_DO_BLANKING] = 0
                        do_m[cur:cur+ne, COL_DO_TRIGGER]  = 1
                        cur += ne

                        # Readout : blanking=1, laser=0, Pockels=0, trigger=0
                        ao_m[cur:cur+nr, COL_AOM_Z]        = z_v
                        do_m[cur:cur+nr, COL_DO_BLANKING]  = 1
                        do_m[cur:cur+nr, COL_DO_TRIGGER]   = 0
                        cur += nr

        assert cur == N, f"Incoherence curseur : cur={cur}, N={N}"
        return ao_m, ao_s, do_m


# ---------------------------------------------------------------------------
# Sauvegarde binaire
# ---------------------------------------------------------------------------

def save_binary(
    out_dir: Path,
    ao_master: np.ndarray,
    ao_slave: np.ndarray,
    do_master: np.ndarray,
    params_raw: dict,
    p: dict,
) -> None:
    """
    Sauvegarde les tableaux :
      - AO : binaire pur float64 (ao_master.bin, ao_slave.bin)
      - DO : TDMS (do_master.tdms) — une waveform par voie, lu nativement
             en LabVIEW comme tableau de waveforms numeriques sans conversion

    Structure TDMS DO :
      Groupe "DO_Master", canaux : filtre, L405, L455, blanking, trigger, L488, L561, L640
      Chaque canal = tableau 1D uint8 de N_samples valeurs (0 ou 1)
      Propriete wf_increment = 1/sample_rate_hz pour le dt en LabVIEW
    """
    if not HAS_TDMS:
        raise RuntimeError("npTDMS non installe. Lancez : pip install npTDMS")

    out_dir.mkdir(parents=True, exist_ok=True)

    N    = do_master.shape[0]
    rate = p["sample_rate_hz"]
    dt   = 1.0 / rate

    AO_MASTER_NAMES = ["laser_405nm", "XY_X", "Pockels", "Z_piezo"]
    AO_SLAVE_NAMES  = ["laser_488nm", "laser_561nm", "laser_640nm"]
    DO_MASTER_NAMES = ["filtre", "L405", "L455", "blanking", "trigger", "L488", "L561", "L640"]

    tdms_path = out_dir / "daq_sequence.tdms"
    with TdmsWriter(str(tdms_path)) as writer:
        channels = []

        # -- Groupe AO_Master ------------------------------------------------
        for col_idx, ch_name in enumerate(AO_MASTER_NAMES):
            channels.append(ChannelObject(
                "AO_Master", ch_name,
                ao_master[:, col_idx].astype(np.float64),
                properties={"wf_increment": dt, "wf_start_offset": 0.0},
            ))

        # -- Groupe AO_Slave -------------------------------------------------
        for col_idx, ch_name in enumerate(AO_SLAVE_NAMES):
            channels.append(ChannelObject(
                "AO_Slave", ch_name,
                ao_slave[:, col_idx].astype(np.float64),
                properties={"wf_increment": dt, "wf_start_offset": 0.0},
            ))

        # -- Groupes DO : un groupe par voie pour lecture LabVIEW waveform par waveform
        for col_idx, ch_name in enumerate(DO_MASTER_NAMES):
            channels.append(ChannelObject(
                f"DO_{ch_name}", "data",
                do_master[:, col_idx].astype(np.float64),
                properties={"wf_increment": dt, "wf_start_offset": 0.0},
            ))

        writer.write_segment(channels)

    N    = ao_master.shape[0]
    rate = p["sample_rate_hz"]

    info = {
        "script":         SCRIPT_NAME,
        "version":        SCRIPT_VERSION,
        "generated":      datetime.datetime.now().isoformat(timespec="seconds"),
        "sample_rate_hz": rate,
        "total_samples":  int(N),
        "duration_s":     round(N / rate, 6),
        "tdms_file":      "daq_sequence.tdms",
        "tdms_groups": {
            "AO_Master": {
                "channels": ["laser_405nm", "XY_X", "Pockels", "Z_piezo"],
                "dtype":    "float64",
                "mapping":  ["AO0#1","AO1#1","AO2#1","AO3#1"],
            },
            "AO_Slave": {
                "channels": ["laser_488nm", "laser_561nm", "laser_640nm"],
                "dtype":    "float64",
                "mapping":  ["AO0#2","AO1#2","AO2#2"],
            },
            "DO_Master": {
                "channels": ["filtre","L405","L455","blanking","trigger","L488","L561","L640"],
                "dtype":    "float64",
                "mapping":  ["P0.0","P0.1","P0.2","P0.3","P0.4","P0.5","P0.6","P0.7"],
            },
        },
        "acquisition": {
            "NPOS":               p["NPOS"],
            "NZ":                 p["NZ"],
            "channels_active":    p["channels_active"],
            "single_channel_mode":p["single_channel_mode"],
            "t_readout_s":        p["t_readout_s"],
            "extratime_s":        p["extratime_s"],
            "z_array_v":          p["z_array"].tolist(),
            "xy_array_v":         p["xy_array"].tolist(),
        },
    }

    with open(out_dir / "sequence_info.json", "w", encoding="utf-8") as f:
        json.dump(info, f, ensure_ascii=False, indent=2)

    total_mb = (ao_master.nbytes + ao_slave.nbytes + do_master.nbytes) / 1e6
    print(f"[OK] daq_sequence.tdms  {N} samples"
          f"  AO_Master(4) + AO_Slave(3) + DO_Master(8)"
          f"  (~{total_mb:.1f} MB)")
    print(f"[OK] sequence_info.json")
    print(f"[OK] Duree sequence  {N/rate*1000:.1f} ms  ({N} samples @ {rate} Hz)")


# ---------------------------------------------------------------------------
# Affichage timeline
# ---------------------------------------------------------------------------

def print_timeline(ao_m, do_m, p):
    N    = do_m.shape[0]
    rate = p["sample_rate_hz"]
    t_ms = np.arange(N) * 1000.0 / rate

    trigger = do_m[:, COL_DO_TRIGGER].astype(int)
    filtre  = do_m[:, COL_DO_FILTRE].astype(int)

    expo_starts   = np.where(np.diff(trigger, prepend=0) ==  1)[0]
    expo_ends     = np.where(np.diff(trigger, prepend=0) == -1)[0]
    filter_starts = np.where(np.diff(filtre,  prepend=0) ==  1)[0]
    filter_ends   = np.where(np.diff(filtre,  prepend=0) == -1)[0]

    print()
    print("-- Timeline resumee ---------------------------------------------")
    print(f"   Duree : {N*1000.0/rate:.1f} ms  |  {N} samples @ {rate} Hz")
    print(f"   NPOS={p['NPOS']}  NZ={p['NZ']}"
          f"  canaux={len(p['channels_active'])}"
          f"  single_ch={p['single_channel_mode']}")

    exp_i = 0
    flt_i = 0

    for pos_idx in range(p["NPOS"]):
        print(f"\n  +-- POS {pos_idx+1} ------------------------------------------")

        if p["single_channel_mode"] and flt_i < len(filter_starts):
            ch = p["channels_active"][0]
            fs = filter_starts[flt_i]
            fe = filter_ends[flt_i] if flt_i < len(filter_ends) else fs
            print(f"  |  [pulse filtre unique] C{ch['channel_id']}"
                  f" f={ch['filter_number']}"
                  f"  {t_ms[fs]:.1f}->{t_ms[fe]:.1f} ms"
                  f"  ({(fe-fs)*1000.0/rate:.1f} ms)")
            flt_i += 1

        for z_idx in range(p["NZ"]):
            z_v = p["z_array"][z_idx]
            print(f"  |  +- Z={z_idx+1} ({z_v:.3f}V) -------------------------")

            for ch in p["channels_active"]:
                if not p["single_channel_mode"] and flt_i < len(filter_starts):
                    fs = filter_starts[flt_i]
                    fe = filter_ends[flt_i] if flt_i < len(filter_ends) else fs
                    print(f"  |  |  C{ch['channel_id']} {ch['laser_nm']}nm"
                          f" f={ch['filter_number']}"
                          f"  pulse {t_ms[fs]:.1f}->{t_ms[fe]:.1f} ms"
                          f"  ({(fe-fs)*1000.0/rate:.1f} ms)")
                    flt_i += 1

                for pol in range(ch["npol"]):
                    if exp_i < len(expo_starts):
                        es = expo_starts[exp_i]
                        ee = expo_ends[exp_i] if exp_i < len(expo_ends) else es
                        vp = ao_m[es, COL_AOM_POCKELS]
                        dur= (ee - es) * 1000.0 / rate
                        print(f"  |  |    POL{pol+1}: {t_ms[es]:.1f}->{t_ms[ee]:.1f} ms"
                              f"  ({dur:.1f} ms)  Pockels={vp:.3f}V")
                        exp_i += 1

            print(f"  |  +-------------------------------------------------")
        print(f"  +----------------------------------------------------------")

    print(f"\n  Expositions   : {len(expo_starts)}")
    print(f"  Pulses filtre : {len(filter_starts)}")
    print("-----------------------------------------------------------------")
    print()


# ---------------------------------------------------------------------------
# Configuration de test integree
# ---------------------------------------------------------------------------

def make_test_config() -> dict:
    """
    NPOS=1, NZ=3
    Canal 1 : 488 nm, 90%, expo=50ms, polar ON, filtre 2
    Canal 2 : 561 nm, 50%, expo=70ms, polar ON, filtre 3
    Canal 3 : OFF
    polar_array : 4 etats, tensions pour 488 et 561
    t_readout=10ms, extratime=0.15s
    Z=[0.0, 0.5, 1.0]V
    """
    return {
        "sample_rate_hz": 10000,
        "live_mode":      False,
        "multipos_active":False,
        "channels": [
            {
                "channel_on":    True,
                "laser_nm":      488,
                "power_pct":     90,
                "t_expo_s":      0.05,
                "polar_on":      True,
                "filter_number": 2,
            },
            {
                "channel_on":    True,
                "laser_nm":      561,
                "power_pct":     50,
                "t_expo_s":      0.07,
                "polar_on":      True,
                "filter_number": 3,
            },
            {
                "channel_on":    False,
                "laser_nm":      640,
                "power_pct":     0,
                "t_expo_s":      0.05,
                "polar_on":      False,
                "filter_number": 1,
            },
        ],
        "t_readout_s":  0.01,
        "extratime_s":  0.15,
        # polar_array [4 pol x 4 lasers (405,488,561,640)]
        "polar_array": [
            [0.00, 0.00, 0.00, 0.00],
            [0.25, 0.30, 0.28, 0.00],
            [0.50, 0.55, 0.52, 0.00],
            [0.75, 0.80, 0.77, 0.00],
        ],
        "z_array":  [0.0, 0.5, 1.0],
        "xy_array": [[0.0, 0.0]],
    }


def make_test_config_single() -> dict:
    """Test single canal (pulse filtre une seule fois)."""
    cfg = make_test_config()
    cfg["channels"][1]["channel_on"] = False
    return cfg


# ---------------------------------------------------------------------------
# Point d'entree
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description=f"{SCRIPT_NAME} {SCRIPT_VERSION} — Generation sequences DAQ"
    )
    ap.add_argument("--out",    default="./daq_sequence",
                    help="Dossier de sortie")
    ap.add_argument("--test",   action="store_true",
                    help="Config de test 2 canaux")
    ap.add_argument("--test1",  action="store_true",
                    help="Config de test 1 canal (pulse filtre unique)")
    ap.add_argument("--config", default="",
                    help="Fichier JSON de parametres")
    args = ap.parse_args()

    if args.test:
        print("[INFO] Mode test — 2 canaux (488+561), NZ=3, 4 pol")
        raw = make_test_config()
    elif args.test1:
        print("[INFO] Mode test — 1 canal (488), NZ=3, 4 pol")
        raw = make_test_config_single()
    elif args.config:
        cfg_path = Path(args.config)
        if not cfg_path.exists():
            print(f"[ERROR] Config introuvable : {cfg_path}", file=sys.stderr)
            sys.exit(1)
        with open(cfg_path, "r", encoding="utf-8-sig") as f:
            raw = json.load(f)
    else:
        print("[ERROR] Specifie --test, --test1 ou --config <fichier.json>",
              file=sys.stderr)
        ap.print_help()
        sys.exit(1)

    print("[INFO] Validation des parametres...")
    p = parse_params(raw)

    print(f"[INFO] NPOS={p['NPOS']}  NZ={p['NZ']}"
          f"  canaux_actifs={len(p['channels_active'])}"
          f"  single_ch={p['single_channel_mode']}")
    for ch in p["channels_active"]:
        print(f"       C{ch['channel_id']} {ch['laser_nm']}nm"
              f"  {ch['power_v']:.2f}V"
              f"  expo={ch['t_expo_s']*1000:.0f}ms"
              f"  npol={ch['npol']}"
              f"  filtre={ch['filter_number']}"
              f"  pulse={ch['pulse_ms']}ms")

    print("[INFO] Calcul de la sequence...")
    builder = SequenceBuilder(p)
    ao_master, ao_slave, do_master = builder.build()

    print_timeline(ao_master, do_master, p)

    out_dir = Path(args.out)
    save_binary(out_dir, ao_master, ao_slave, do_master, raw, p)


if __name__ == "__main__":
    try:
        main()
    except Exception as ex:
        print(f"[ERROR] {ex}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(2)
