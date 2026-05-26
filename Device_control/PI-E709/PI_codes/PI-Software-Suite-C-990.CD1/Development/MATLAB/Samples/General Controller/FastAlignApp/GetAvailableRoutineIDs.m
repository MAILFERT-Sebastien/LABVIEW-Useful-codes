% function GetAvailableRoutineIDs (PIdevice)
% This code is provided by Physik Instrumente (PI) SE & Co. KG.
% You may alter it corresponding to your needs.
% Comments and Corrections are very welcome.
% Please contact us by mailing to support-software@pi.ws. Thank you.

function routineIDs = GetAvailableRoutineIDs (PIdevice)
qFRRstring = GCSCommandSet(PIdevice, 'FRR?');
lines = strsplit(qFRRstring,'\n');
routineIDs = {};

for idx = 1 : length(lines)
    [match,token] = regexpi(lines{idx}, '^(\w+)\s\d+\s*\=\s*(FDG|FDL|FDR)', 'match', 'tokens');
    
    if ~isempty(match)
        routineIDs{end+1} = token{1}{1};
    end
end
end