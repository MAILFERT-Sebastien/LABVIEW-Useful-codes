classdef ScanPattern < handle

    properties (Access = public)
        no_scan_ch0 = 11;
        sval_ch0_min = -1.5;
        sval_ch0_max = 1.5;
        no_scan_ch1  = 11;
        sval_ch1_min = -2;
        sval_ch1_max = 2;
        sval_offset_ch0 = 0;
        sval_offset_ch1 = 0;
        radiusCircle = 2.30;
        radiusSpiral = 2.25;
        phiOutMost;
        radiusCircleTIR = 2.60;
        phiStepSize = 30;
        voltage_ch0_scan
        voltage_ch1_scan
    end

    methods
        function obj = ScanPattern()
            obj.snakeMovemnetScanPattern();
        end

        function rasterScanPattern(obj)
            ch0_scan = linspace(obj.sval_ch0_min, obj.sval_ch0_max, obj.no_scan_ch0);
            ch1_scan = linspace(obj.sval_ch1_max, obj.sval_ch1_min, obj.no_scan_ch1);

            ch0_scan_matrix = zeros(obj.no_scan_ch0, obj.no_scan_ch0);
            for ii=1:obj.no_scan_ch0
                for jj = 1:obj.no_scan_ch0
                    ch0_scan_matrix(jj,ii) = ch0_scan(ii);
                end
            end

            ch1_scan_matrix = zeros(obj.no_scan_ch1, obj.no_scan_ch1);
            for ii=1:obj.no_scan_ch1
                for jj = 1:obj.no_scan_ch1
                    ch1_scan_matrix(ii,jj) = ch1_scan(ii);
                end
            end

            voltage_ch0_scan = ch0_scan_matrix';
            obj.voltage_ch0_scan = voltage_ch0_scan(:);
            voltage_ch1_scan = ch1_scan_matrix';
            obj.voltage_ch1_scan = voltage_ch1_scan(:);
            disp('Raster scan is selected');
        end

        function snakeMovemnetScanPattern(obj)
            ch0_scan = linspace(obj.sval_ch0_min, obj.sval_ch0_max, obj.no_scan_ch0);
            ch1_scan = linspace(obj.sval_ch1_max, obj.sval_ch1_min, obj.no_scan_ch1);

            ch0_scan_matrix = zeros(obj.no_scan_ch0, obj.no_scan_ch0);
            for ii=1:obj.no_scan_ch0
                for jj = 1:obj.no_scan_ch0
                    ch0_scan_matrix(jj,ii) = ch0_scan(ii);
                end
            end

            ch1_scan_matrix = zeros(obj.no_scan_ch1, obj.no_scan_ch1);
            for ii=1:obj.no_scan_ch1
                for jj = 1:obj.no_scan_ch1
                    ch1_scan_matrix(ii,jj) = ch1_scan(ii);
                end
            end

            sz_row = size(ch0_scan_matrix,1);
            sz_col = size(ch0_scan_matrix,1);
            for ii = 1:sz_row
                for jj = 1:sz_col
                    if(mod(ii,2) == 0)
                        mask0_odd(ii,jj) = 1;
                        mask0_even(ii,jj) = 0;
                    else
                        mask0_odd(ii,jj) = 0;
                        mask0_even(ii,jj) = 1;
                    end
                end
            end

            temp_mat1_ch0 = ch0_scan_matrix.*mask0_odd;
            temp_mat2_ch0 = ch0_scan_matrix.*mask0_even;
            temp_mat1_ch0 = fliplr(temp_mat1_ch0);
            temp_mat1_ch1 = ch1_scan_matrix.*mask0_odd;
            temp_mat2_ch1 = ch1_scan_matrix.*mask0_even;
            temp_mat1_ch1 = fliplr(temp_mat1_ch1);

            ch0_scan_matrix = temp_mat1_ch0 + temp_mat2_ch0;
            ch1_scan_matrix = temp_mat1_ch1 + temp_mat2_ch1;

            voltage_ch0_scan = ch0_scan_matrix';
            obj.voltage_ch0_scan = voltage_ch0_scan(:);
            voltage_ch1_scan = ch1_scan_matrix';
            obj.voltage_ch1_scan = voltage_ch1_scan(:);
            disp('Snake motion type pattern is selected');
        end

        function circularScanPattern(obj)
            radius = obj.radiusCircle;
            no_circle = 5;
            r_step = radius/no_circle;
            r = radius:-r_step:0;

            th_outmost = 0:10:350;
            th_inner1  = 0:10:350;
            th_inner2  = 0:15:345;
            th_inner3  = 0:20:340;
            th_inner4  = 0:30:330;
            th_inmost  = 0;

            for ii= 1:size(th_outmost,2)
                volt_th_outmost_ch0(ii) = r(1)*sind(th_outmost(ii));
                volt_th_outmost_ch1(ii) = r(1)*sqrt(1.7777)*cosd(th_outmost(ii));
            end
            for ii= 1:size(th_inner1,2)
                volt_th_inner1_ch0(ii) = r(2)*sind(th_inner1(ii));
                volt_th_inner1_ch1(ii) = r(2)*sqrt(1.7777)*cosd(th_inner1(ii));
            end
            for ii= 1:size(th_inner2,2)
                volt_th_inner2_ch0(ii) = r(3)*sind(th_inner2(ii));
                volt_th_inner2_ch1(ii) = r(3)*sqrt(1.7777)*cosd(th_inner2(ii));
            end
            for ii= 1:size(th_inner3,2)
                volt_th_inner3_ch0(ii) = r(4)*sind(th_inner3(ii));
                volt_th_inner3_ch1(ii) = r(4)*sqrt(1.7777)*cosd(th_inner3(ii));
            end
            for ii= 1:size(th_inner4,2)
                volt_th_inner4_ch0(ii) = r(5)*sind(th_inner4(ii));
                volt_th_inner4_ch1(ii) = r(5)*sqrt(1.7777)*cosd(th_inner4(ii));
            end
            for ii= 1:size(th_inmost,2)
                volt_th_inmost_ch0(ii) = r(6)*sind(th_inmost(ii));
                volt_th_inmost_ch1(ii) = r(6)*sqrt(1.7777)*cosd(th_inmost(ii));
            end

            volt_ch0 = [volt_th_outmost_ch0, volt_th_inner1_ch0, volt_th_inner2_ch0, ...
                volt_th_inner3_ch0, volt_th_inner4_ch0, volt_th_inmost_ch0];
            volt_ch1 = [volt_th_outmost_ch1, volt_th_inner1_ch1, volt_th_inner2_ch1, ...
                volt_th_inner3_ch1, volt_th_inner4_ch1, volt_th_inmost_ch1];

            obj.voltage_ch0_scan = (volt_ch0 + 0.15)';
            obj.voltage_ch1_scan = volt_ch1';
            disp('Circular pattern is selected');
        end

        function circularTIR(obj)
            radius = obj.radiusCircleTIR;
            offsetVoltageCh0 = obj.sval_offset_ch0;
            offsetVoltageCh1 = obj.sval_offset_ch1;

            th_outmost = 0:obj.phiStepSize:360;

            for ii= 1:size(th_outmost,2)
                volt_th_outmost_ch0(ii) = radius*cosd(th_outmost(ii));
                volt_th_outmost_ch1(ii) = radius*sqrt(1.7777)*sind(th_outmost(ii));
            end

            obj.voltage_ch0_scan = (volt_th_outmost_ch0 + offsetVoltageCh0)';
            obj.voltage_ch1_scan = (volt_th_outmost_ch1 + offsetVoltageCh1)';
            obj.phiOutMost = th_outmost;
            disp('Circular pattern (out most circle) is selected');
        end

        function spiralScanPattern(obj)
            a = 0.0012;
            no_spiral = 5;

            th_inner1 = 0:30:360;
            th_inner2 = 0:20:360;
            th_inner3 = 0:15:360;
            th_inner4 = 0:10:360;
            th_inner5 = 0:5:360;

            x_old = 0;
            mm = cell(1, no_spiral);
            nn = cell(1, no_spiral);

            theta_all = {th_inner1, th_inner2, th_inner3, th_inner4, th_inner5};

            for ii = 1:no_spiral
                theta_temp = theta_all{ii};
                r_temp = a*theta_temp + x_old;
                mm{ii} = r_temp.*cosd(theta_temp);
                nn{ii} = sqrt(1.7777)*r_temp.*sind(theta_temp);
                x_old = mm{ii}(end);
            end

            obj.voltage_ch0_scan = [mm{1} mm{2} mm{3} mm{4} mm{5}]';
            obj.voltage_ch1_scan = [nn{1} nn{2} nn{3} nn{4} nn{5}]';
            disp('Spiral pattern is selected');
        end

        function phiOutmost = getPhiValue(obj)
            phiOutmost = obj.phiOutMost;
        end
    end
end