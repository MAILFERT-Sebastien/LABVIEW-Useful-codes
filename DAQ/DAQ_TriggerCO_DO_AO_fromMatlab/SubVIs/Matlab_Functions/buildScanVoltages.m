function [voltage_ch0, voltage_ch1] = buildScanVoltages(pattern_index, devpack_path)
% buildScanVoltages : Génère les trajectoires de tension pour les miroirs galvanométriques
%
% Entrées :
%   pattern_index : entier (0=Raster, 1=Snake, 2=Circular, 3=CircularTIR, 4=Spiral)
%   devpack_path  : chemin vers le dossier Matlab_Functions (contenant +DevicePack)
%
% Sorties :
%   voltage_ch0 : vecteur ligne [1 x N] tensions miroir X (Volts)
%   voltage_ch1 : vecteur ligne [1 x N] tensions miroir Y (Volts)
%
% Les paramètres de chaque pattern (rayon, nombre de points, plage de tension)
% sont définis dans DevicePack.ScanPattern avec leurs valeurs par défaut.

addpath(devpack_path);

% Instanciation de l'objet ScanPattern avec les paramètres par défaut
sp = DevicePack.ScanPattern();

% Sélection et calcul du pattern de scan
switch pattern_index
    case 0,  sp.rasterScanPattern();           % Balayage ligne par ligne
    case 1,  sp.snakeMovemnetScanPattern();    % Balayage en serpentin
    case 2,  sp.circularScanPattern();         % Cercles concentriques
    case 3,  sp.circularTIR();                 % Cercle unique (TIR)
    case 4,  sp.spiralScanPattern();           % Spirale d'Archimède
    otherwise
        error('buildScanVoltages : pattern_index invalide (%d). Valeurs acceptées : 0 à 4.', pattern_index);
end

% Retour en vecteurs lignes [1 x N] pour compatibilité LabVIEW
voltage_ch0 = double(sp.voltage_ch0_scan)';
voltage_ch1 = double(sp.voltage_ch1_scan)';

end