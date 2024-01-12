function [Hd,param] = CL02_filterDesigner(Fs,Fc1,Fc2,N,filename)
%CL02_FILTERDESIGNER Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.7 and Signal Processing Toolbox 8.3.
% Generated on: 09-Apr-2021 15:50:34

% Butterworth Lowpass filter designed using FDESIGN.LOWPASS.

% All frequency values are in Hz.
if nargin<4
    filename=uiputfile('*.filter');
end
% All frequency values are in Hz.
% Fs = 1000;  % Sampling Frequency

% N   = 2;    % Order
% Fc1 = 60;   % First Cutoff Frequency
% Fc2 = 110;  % Second Cutoff Frequency

% Construct an FDESIGN object and call its BUTTER method.
h  = fdesign.bandpass('N,F3dB1,F3dB2', N, Fc1, Fc2, Fs);
Hd = design(h, 'butter');
fvtool(Hd);
grpdelay(Hd);
%% filter header infomation generation
NSEC=length(Hd.ScaleValues)+size(Hd.sosMatrix,1);
NL=ones(NSEC,1);
NL(2:2:end)=3;
NUM=zeros(NSEC,3);
DEN=zeros(NSEC,3);
NUM(1:2:end,1)=Hd.ScaleValues;
NUM(2:2:end,:)=Hd.sosMatrix(:,1:3);
DEN(1:2:end,1)=1;
DEN(2:2:end,:)=Hd.sosMatrix(:,4:6);

type=02; %01 for DF-I , 02 for SOS filter

param.filename=filename;
param.type=type;
param.NSEC=NSEC;
param.NL=NL;
param.NUM=NUM;
param.DEN=DEN;


fh=fopen(filename,'wb+');
fwrite(fh,type,'uint8');
fwrite(fh,NSEC,'uint16');
fwrite(fh,NL,'int');
fwrite(fh,NUM','single');
fwrite(fh,DEN','single');
fclose(fh);

fh=fopen(filename,'rb');

param.Byte=fread(fh,inf,'uint8');
fclose(fh);

% [EOF]
