% filter_names = {'D','T','A','B','G','I','E','SPW','R','R1'};
% filter_bands={[1 4],[4 8],[8 13],[13 30],[30 80],[60 80],[80 120],[12 30],[110 250],[100 200]};
% filter_LPF = cellfun(@(x) x(1),filter_bands);

filters={};fil_idx=1;

fil=[];
fil.name = 'A';
fil.bands = [8,13];
fil.lpf = 4;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'B';
fil.bands = [13,30];
fil.lpf = 10;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

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
fil.name = 'SPW';
fil.bands = [12,30];
fil.lpf = 5;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R';
fil.bands = [110,250];
fil.lpf = 55;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

fil=[];
fil.name = 'R1';
fil.bands = [100,200];
fil.lpf = 50;
filters{fil_idx}=fil;fil_idx=fil_idx+1;

Fs = 1000;  % Sampling Frequency
N   = 1;   % Order

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

end