% filter_names = {'D','T','A','B','G','I','E','SPW','R','R1'};
% filter_bands={[1 4],[4 8],[8 13],[13 30],[30 80],[60 80],[80 120],[12 30],[110 250],[100 200]};
% filter_LPF = cellfun(@(x) x(1),filter_bands);

filters={};fil_idx=1;

fil=[];
fil.name = 'D';
fil.bands = [1.5,5.5];
fil.lpf = 6;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'T';
fil.bands = [4,8];
fil.lpf = 4;
fil=[];

fil=[];
fil.name = 'A';
fil.bands = [8,13];
fil.lpf = 4;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'SPW';
fil.bands = [12,30];
fil.lpf = 5;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'B';
fil.bands = [13,30];
fil.lpf = 10;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil.name = 'G';
fil.bands = [30,80];
fil.lpf = 15;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'I';
fil.bands = [60,80];
fil.lpf = 30;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'E';
fil.bands = [80,120];
fil.lpf = 40;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R1';
fil.bands = [100,200];
fil.lpf = 50;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R';
fil.bands = [110,250];
fil.lpf = 55;
filters{fil_idx}=fil;fil_idx=fil_idx+1;


Fs = 1000;  % Sampling Frequency
N   = 1;   % Order

Hds = cell(length(fil),3);
for idx=1:length(filters)
    freqs = filters{idx}.bands;
    name = filters{idx}.name;
    Fc1 = freqs(1);   % First Cutoff Frequency
    Fc2 = freqs(2);  % Second Cutoff Frequency
    Fl = filters{idx}.lpf;
    % Construct an FDESIGN object and call its BUTTER method.
    h  = fdesign.bandpass('N,F3dB1,F3dB2', N*2, Fc1, Fc2, Fs);
    Hd = design(h, 'butter');
    set(Hd,'arithmetic','single');
    % set(Hd,'OptimizeScaleValues',)
    filename = "fdacoefs_BPF_"+name+"_" + num2str(Fc1) + '_' +num2str(Fc2) + "Hz@1000_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,name,Hd.sosMatrix,Hd.ScaleValues);
    Hds{idx,1}=Hd;
    % Construct HT filter
    test_data = data_raw;
%     test_data = rand(round(fs*100/Fc1),1)*32768;
    test_data = filter(Hd,test_data);
    [Hd_best,Hd_all,accuracy]=CE32_generateHilbertTransformer(test_data,Fs,25,6,1);
    filename = "fdacoefs_HTF_"+name+"_" + num2str(Fc1) + '_' +num2str(Fc2) + "Hz@1000_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,["HTF_"+name],Hd_best.sosMatrix,Hd_best.ScaleValues);
    Hds{idx,2}=Hd_best;
    % Construct an FDESIGN object and call its BUTTER method.
    h  = fdesign.lowpass('N,F3dB', N, Fl, Fs);
    Hd = design(h, 'butter');
    if(name=='D')
        b=1;
        a=1;
        sos=tf2sos(b,a);
        Hd.sosMatrix = sos;
        Hd.ScaleValues = [1 1];
    end
    set(Hd,'arithmetic','single');
    filename = "fdacoefs_LPF_"+name+"_" + num2str(Fl) + "Hz@1000_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,["LPF_"+name],Hd.sosMatrix,Hd.ScaleValues);
    Hds{idx,3}=Hd;
end

%% filter test


% ripple
filter_id = 3;
file = 'W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat';
[Nch,fs_raw,Nsamples,ch_sel] = DAT_xmlread(file);
ts =  fs_raw*3651;
te = ts+1*fs_raw;
data = readmulti_frank(file,Nch,ch_sel(50),ts,te)*0.195;
data = resample(data,Fs,fs_raw);
data_fil = filter(Hds{filter_id,1},data);
data_hilbiir = filter(Hds{filter_id,2},data_fil);
data_hilbiir_env =  sqrt(data_fil.^2+data_hilbiir.^2); %calculate envelop by getting norm of analytic signal (original signal + 90 phase shift signal*j)
data_hilbiir_angle  = angle(data_fil + data_hilbiir*1i);
data_hilbert = hilbert(data_fil);
data_hilbert_env = abs(data_hilbert);
t=(0:(length(data)-1))/Fs;
subplot(511);
plot(t,data); %plot raw data
title('Raw')
subplot(512);
plot(t,data_fil); %plot filtered signal
title('band-pass filtering')
%rectification
data_DSP_env = abs(data_fil);
subplot(513);
plot(t,data_DSP_env); %plot rectified signal
title('Rectifying')
% smoothing by low pass filter
[b,a] = butter(2,fl/fs*2,'low');
data_DSP_env = filter(Hds{filter_id,3},data_DSP_env);
subplot(514);
plot(t,data_DSP_env); %plot extracted envelop
hold on;
plot(t,data_hilbiir_env);
plot(t,data_hilbert_env); 
hold off
legend({'DSP','hilbiir','HT'})
title('Smoothing')

subplot(515);
plot(t,data_hilbiir_angle);
hold on;
plot(t,angle(data_hilbert)); %plot extracted envelop
hold off
legend({'hilbiir','HT'})
title('Phase')

disp("Phase accuracy"+ num2str(corr(data_hilbiir_angle,angle(data_hilbert))));
[xc_hilbiir,lags_hilbiir] = xcorr(data_hilbiir_env,data_hilbert_env);  % Cross-correlation of the two phase angles
[xc_DSP,lags_DSP] = xcorr(data_DSP_env,data_hilbert_env);  % Cross-correlation of the two phase angles
[~, ix1] = max(abs(xc_hilbiir)); 
[~, ix2] = max(abs(xc_DSP)); 
lag_hilbiir = lags_hilbiir(ix1)/fs;
lag_DSP = lags_DSP(ix2)/fs;
disp("Delay of hilbiir:" + num2str(lag_hilbiir) + " Delay of DSP:" + num2str(lag_DSP))
