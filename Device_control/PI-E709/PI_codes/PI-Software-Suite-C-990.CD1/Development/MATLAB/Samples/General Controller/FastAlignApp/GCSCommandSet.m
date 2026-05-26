% function GCSCommandSet(PIdevice, gcsCommand)
% This code is provided by Physik Instrumente (PI) SE & Co. KG
% You may alter it corresponding to your needs.
% Comments and Corrections are very welcome.
% Please contact us by mailing to support-software@pi.ws. Thank you.

function answer = GCSCommandSet(PIdevice, gcsCommand)
PIdevice.GcsCommandset(gcsCommand);
lastLine = false;
answer = '';

while(~lastLine)
    answer = [answer, PIdevice.GcsGetAnswer()];
    
    if 1 == length(answer) % Answer is empty
        lastLine = true;
    elseif answer(end-1) == ' '
        lastLine = false;
    else
        lastLine = true;
    end
end

errorNumber = PIdevice.GetError();
if (0 ~= errorNumber), error(PIdevice.TranslateError(errorNumber)); end;

end