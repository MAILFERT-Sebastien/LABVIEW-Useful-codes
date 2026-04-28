function [buffer_AO0, buffer_AO1, buffer_DO] = buildWaveforms(voltage_ch0, voltage_ch1, t_sleep_s, t_expo_s, t_readout_s, sample_rate_Hz)
% buildWaveforms : Construit les buffers AO0, AO1 et DO pour le scan DAQmx
%
% Pour chaque point i du scan, la séquence temporelle est :
%
%   |<--- t_sleep_s --->|<--- t_expo_s --->|<--- t_readout_s --->|
%   AO0, AO1 = V_i      AO0, AO1 = V_i     AO0, AO1 = V_i
%   DO = 0              DO = 1              DO = 0
%
%   - t_sleep_s   : temps de positionnement des miroirs (DO=0, pas d'acquisition)
%   - t_expo_s    : temps d'exposition caméra (DO=1, trigger TTL actif)
%   - t_readout_s : temps de lecture caméra (DO=0, pas d'acquisition)
%
% Un point final à 0V est ajouté en fin de buffer pour ramener les miroirs
% en position neutre après le scan.
%
% Entrées :
%   voltage_ch0   : vecteur ligne [1 x N] tensions miroir X (Volts)
%   voltage_ch1   : vecteur ligne [1 x N] tensions miroir Y (Volts)
%   t_sleep_s     : durée phase sleep (secondes)
%   t_expo_s      : durée phase exposition (secondes)
%   t_readout_s   : durée phase readout (secondes)
%   sample_rate_Hz: fréquence d'échantillonnage CTR0 (Hz)
%
% Sorties :
%   buffer_AO0 : vecteur ligne [1 x (N*n_per_point + 1)] tensions AO0 (Volts)
%   buffer_AO1 : vecteur ligne [1 x (N*n_per_point + 1)] tensions AO1 (Volts)
%   buffer_DO  : vecteur ligne [1 x (N*n_per_point + 1)] trigger TTL (uint8, 0 ou 1)

% Calcul du nombre d'échantillons par phase
n_sleep     = round(t_sleep_s   * sample_rate_Hz);
n_expo      = round(t_expo_s    * sample_rate_Hz);
n_readout   = round(t_readout_s * sample_rate_Hz);
n_per_point = n_sleep + n_expo + n_readout;

% Nombre de points du scan
N = length(voltage_ch0);

% Taille totale du buffer : N points * n_per_point échantillons + 1 point retour à 0V
buf_size = N * n_per_point + 1;

% Pré-allocation des buffers
buffer_AO0 = zeros(1, buf_size);
buffer_AO1 = zeros(1, buf_size);
buffer_DO  = zeros(1, buf_size, 'uint8');

% Construction du buffer point par point
for ii = 1:N
    
    % Indices de début et fin de chaque phase pour le point ii
    idx_start   = (ii-1) * n_per_point + 1;
    idx_sleep   = idx_start             : idx_start + n_sleep - 1;
    idx_expo    = idx_start + n_sleep   : idx_start + n_sleep + n_expo - 1;
    idx_readout = idx_start + n_sleep + n_expo : idx_start + n_per_point - 1;
    
    % AO0 et AO1 maintiennent la tension V_i pendant les trois phases
    buffer_AO0(idx_sleep)   = voltage_ch0(ii);
    buffer_AO0(idx_expo)    = voltage_ch0(ii);
    buffer_AO0(idx_readout) = voltage_ch0(ii);
    
    buffer_AO1(idx_sleep)   = voltage_ch1(ii);
    buffer_AO1(idx_expo)    = voltage_ch1(ii);
    buffer_AO1(idx_readout) = voltage_ch1(ii);
    
    % DO : trigger TTL actif uniquement pendant l'exposition
    buffer_DO(idx_sleep)   = 0;
    buffer_DO(idx_expo)    = 1;
    buffer_DO(idx_readout) = 0;
    
end

% Dernier échantillon : retour des miroirs à la position neutre (0V)
buffer_AO0(end) = 0;
buffer_AO1(end) = 0;
buffer_DO(end)  = 0;

end