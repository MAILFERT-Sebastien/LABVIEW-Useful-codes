function create_trajectories_standalone(pattern_index_str, result_file)
    idx = str2double(pattern_index_str);
    sp = DevicePack.ScanPattern();
    switch idx
        case 0, sp.rasterScanPattern();
        case 1, sp.snakeMovemnetScanPattern();
        case 2, sp.circularScanPattern();
        case 3, sp.circularTIR();
        case 4, sp.spiralScanPattern();
        otherwise, sp.rasterScanPattern();
    end
    v0 = double(sp.voltage_ch0_scan(:));
    v1 = double(sp.voltage_ch1_scan(:));
    n = int32(length(v0));
    fid = fopen(result_file, 'wb');
    fwrite(fid, n, 'int32');
    fwrite(fid, v0, 'double');
    fwrite(fid, v1, 'double');
    fclose(fid);
end