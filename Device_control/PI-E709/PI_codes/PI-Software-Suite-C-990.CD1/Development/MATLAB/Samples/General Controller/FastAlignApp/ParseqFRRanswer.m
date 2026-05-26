% ParseqFRRanswer(qFRRstring)
% This code is provided by Physik Instrumente (PI) SE & Co. KG.
% You may alter it corresponding to your needs.
% Comments and Corrections are very welcome.
% Please contact us by mailing to support-software@pi.ws. Thank you.

function qFRRstructure = ParseqFRRanswer(qFRRstring)

qFRRcell = strsplit(qFRRstring,'\n');

resultIDsDefinition = GetResultIDs();

qFRRstructure = containers.Map;
routineResults = containers.Map;

isFirstIteration = true;
for qFRRline_ = qFRRcell
    qFRRline = qFRRline_{1};
    [match,token] = regexpi(qFRRline, '^(\d+)\s(\d+)\=(.*)', 'match', 'tokens');
    
    if isempty(match)
        % skip line
    else
        routineName = token{1}{1};
        if (isFirstIteration)
            oldRoutineName = routineName;
            isFirstIteration = false;
        end
        
        routineHasChanged = ~strcmpi(routineName, oldRoutineName);
        if routineHasChanged
            qFRRstructure(oldRoutineName) = routineResults;
            routineResults = containers.Map;
        end
        
        routineResultID = str2double(token{1}{2});
        routineResultValue = token{1}{3};
        
        routineResultIDstring = resultIDsDefinition(routineResultID);
        routineResults(routineResultIDstring) = routineResultValue;

        oldRoutineName = routineName;
    end
    
end
qFRRstructure(oldRoutineName) = routineResults;

% show structure
% for key_ = qFRRstructure.keys
%     key = key_{1};
%     routine = qFRRstructure(key);
%     for key2_ = routine.keys
%         key2 = key2_{1};
%         fprintf('%s ||| %s ||| %s', key, key2, routine(key2));
%     end
% end

end


function resultIDs = GetResultIDs()
keys = {1, 2, 3, 4, 5, 6, 7, 8, 9};
values = {'Success', 'Maximum value', 'Maximum position', 'Routine definition', ...
    'Scan time', 'Reason of abort', 'Radius', 'Number of direction changes', 'Gradient length'};
resultIDs = containers.Map(keys, values);

% show map
% map = resultIDs;
% for key_ = map.keys
%     key = key_{1};
%     fprintf('%d ||| %s\n', key, map(key));
% end

end