
%% generate filters
% filter_names = {'D','T','A','B','G','I','E','SPW','R','R1'};
% filter_bands={[1 4],[4 8],[8 13],[13 30],[30 80],[60 80],[80 120],[12 30],[110 250],[100 200]};
% filter_LPF = cellfun(@(x) x(1),filter_bands);

fs=20000;
Fs = 1000;  % Sampling Frequency
f_pre = @(x) resample(x,Fs,fs);
win = 30;
filters={};fil_idx=1;
fil=[];
fil.name = 'D';
fil.bands = [1.5,5.5];
fil.lpf = 6;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;


fil=[];
fil.name = 'T';
fil.bands = [4,8];
fil.lpf = 4;
fil.best_dim = 2;
ts=3805;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'T';
fil.bands = [-inf,12];
fil.lpf = 4;
fil.best_dim = 2;
ts=3805;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'T';
fil.bands = [4,inf];
fil.lpf = 4;
fil.best_dim = 2;
ts=3805;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'A';
fil.bands = [8,13];
fil.lpf = 4;
fil.best_dim = 2;
ts=3805;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'SPW';
fil.bands = [12,30];
fil.lpf = 5;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'B';
fil.bands = [13,30];
fil.lpf = 10;
fil.best_dim = 2;
ts=3805;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil.name = 'G';
fil.bands = [30,80];
fil.lpf = 15;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'I';
fil.bands = [60,80];
fil.lpf = 30;
fil.best_dim = 2;
ts = 5;
fil.testdata = f_pre(readmulti_frank('D:\Code\Natalie_IED\hpc_ctx_project\hpc09_day11_220513_095958_5min.dat',...
    1,1,ts*fs,(ts+win)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'E';
fil.bands = [80,120];
fil.lpf = 40;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R1';
fil.bands = [100,200];
fil.lpf = 50;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R';
fil.bands = [110,250];
fil.lpf = 55;
fil.best_dim = 2;
ts = 3651;
fil.testdata = f_pre(readmulti_frank('W:\data\FieldRat\2024\F5\day10\1B1_day10\2_20000101_000220.099\amplifier.dat',...
    64,31,ts*fs,(win+ts)*fs));
filters{fil_idx}=fil;fil_idx=fil_idx+1;

%%

N   = 1;   % Order

Hds = cell(length(fil),3);
for idx=1:length(filters)
    disp("Generating filters:"+filters{idx}.name)
    freqs = filters{idx}.bands;
    name = filters{idx}.name;
    Fc1 = freqs(1);   % First Cutoff Frequency
    Fc2 = freqs(2);  % Second Cutoff Frequency
    Fl = filters{idx}.lpf;
    % Construct an FDESIGN object and call its BUTTER method.
    if(isinf(Fc1))
        [b,a]=butter(N,[Fc2]/Fs*2,'low');
    else
        if(isinf(Fc2))
            [b,a]=butter(N,[Fc1]/Fs*2,'high');
        else
            [b,a]=butter(N,[Fc1,Fc2]/Fs*2,'bandpass');
        end
    end
    [sos,g]=tf2sos(b,a);
    Hd = dfilt.df2sos(sos,g);

    %     h  = fdesign.bandpass('N,F3dB1,F3dB2', N*2, Fc1, Fc2, Fs);
    %     Hd = design(h, 'butter');
    set(Hd,'arithmetic','single');
    % set(Hd,'OptimizeScaleValues',)
    filename = "fdacoefs_BPF_"+name+"_" + num2str(Fc1) + '_' +num2str(Fc2) + "Hz@" + num2str(Fs) + "_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,name,Hd.sosMatrix,Hd.ScaleValues);
    Hds{idx,1}=Hd;
    % Construct HT filter
    test_data = filters{idx}.testdata;
    %     test_data = rand(round(fs*100/Fc1),1)*32768;
    test_data = filter(Hd,test_data);
    [Hd_best,Hd_all,accuracy]=CE32_generateHilbertTransformer(test_data,Fs,25,6,filters{idx}.best_dim,1);
    filename = "fdacoefs_HTF_"+name+"_" + num2str(Fc1) + '_' +num2str(Fc2) + "Hz@" + num2str(Fs) + "_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,["HTF_"+name],Hd_best.sosMatrix,Hd_best.ScaleValues);
%     Hd_sim= loadFilterFromC(filename);
%     fvtool(Hd_best);
%     fvtool(Hd_sim);
%     [env_spw,phase_spw,BPF_spw,LPF_spw]= CL02_filter_HT_sim(data_rip,fileBPF_spw,fileLPF_spw);
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
    filename = "fdacoefs_LPF_"+name+"_" + num2str(Fl) + "Hz@" + num2str(Fs) + "_ord" + num2str(N) + '_SOS.h';
    CE32_filterGen_SOS(filename,["LPF_"+name],Hd.sosMatrix,Hd.ScaleValues);
    Hds{idx,3}=Hd;
end

%% filter test
M=4;
N=1;
for idx=1:length(filters)
    figure(idx)
    data = filters{idx}.testdata;
    data_fil = filter(Hds{idx,1},data);
    data_hilbiir = filter(Hds{idx,2},data_fil);
    data_hilbiir_env =  sqrt(data_fil.^2+data_hilbiir.^2); %calculate envelop by getting norm of analytic signal (original signal + 90 phase shift signal*j)
    data_hilbiir_angle  = angle(data_fil + data_hilbiir*1i);
    data_hilbert = hilbert(data_fil);
    data_hilbert_env = abs(data_hilbert);
    t=(0:(length(data)-1))/Fs;
    subplot(M,N,1);
    plot(t,data); %plot raw data
    title(['Raw ' filters{idx}.name])
    subplot(M,N,2);
    plot(t,data_fil); %plot filtered signal
    title('band-pass filtering')
    %rectification
    data_DSP_env = abs(data_fil);
    % smoothing by low pass filter
    data_DSP_env = filter(Hds{idx,3},data_DSP_env);
    subplot(M,N,3);
    plot(t,data_hilbert_env);hold on;
    plot(t,data_hilbiir_env);
    plot(t,data_DSP_env); %plot extracted envelop
    hold off
    legend({'HT','hilbiir','DSP'})
    txt_env_acc = "Envelop accuracy:hilbiir:"+ num2str(corr(data_hilbiir_env,abs(data_hilbert))) +...
        " DSP:" +num2str(corr(data_DSP_env,abs(data_hilbert)));
    title(txt_env_acc)
    
    subplot(M,N,4);
    plot(t,angle(data_hilbert)); %plot extracted envelop
    hold on;
    plot(t,data_hilbiir_angle);
    hold off
    legend({'HT','hilbiir'})
    %     txt_phase_acc = "Phase accuracy"+ num2str(corr(data_hilbiir_angle,angle(data_hilbert)));
    txt_phase_acc = "Weighted Phase accuracy"+ num2str(corr(data_hilbiir_angle.*abs(data_hilbert),angle(data_hilbert).*abs(data_hilbert)));
    title(txt_phase_acc);
    disp(txt_phase_acc);
    [xc_hilbiir,lags_hilbiir] = xcorr(data_hilbiir_env,data_hilbert_env);  % Cross-correlation of the two phase angles
    [xc_DSP,lags_DSP] = xcorr(data_DSP_env,data_hilbert_env);  % Cross-correlation of the two phase angles
    [~, ix1] = max(abs(xc_hilbiir));
    [~, ix2] = max(abs(xc_DSP));
    lag_hilbiir = lags_hilbiir(ix1)/Fs*1000;
    lag_DSP = lags_DSP(ix2)/Fs*1000;
    txt= "Delay of hilbiir:" + num2str(lag_hilbiir) + "ms Delay of DSP:" + num2str(lag_DSP) + "ms";
    disp(txt)
    subplot(M,N,1);
    legend(txt);
    drawnow;
    
    %     subplot(M,2*N,M*2*N-1);
    %     fvtool(Hds{idx,1});
    %     subplot(M,2*N,M*2*N);
    %     fvtool(Hds{idx,2});
    
end