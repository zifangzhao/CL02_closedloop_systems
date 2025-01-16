% Start recording first in intan, make sure all ADC configured correctly
% configure Device
IP  = '10.48.6.6';
i = MokuWaveformGenerator(IP,1,0);
pulse_width = 0.3;
amp1=5;
offset1 = 2.5;
amp2=5;
offset2 = 2.5;
risetime = 1e-3;
try
    
    %% Configure the instrument
    % Generate a sine wave on Channel 1
    % 0.5Vpp, 1MHz, 0V offset
    
%     i.generate_waveform(1, 'Square','amplitude', 3.3, 'frequency',1/pulse_width,'offset',0 , 'duty', 100);
    i.generate_waveform(1, 'Pulse', 'amplitude',amp1,'offset',offset1,'frequency', 0.99*1/pulse_width ,'pulse_width',pulse_width,'edge_time',risetime);
    % Generate a square wave on Channel 2
    % 1Vpp, 10kHz, 0V offset, 50% duty cycle
    i.generate_waveform(2, 'Pulse', 'amplitude',amp2,'offset',offset2,'frequency', 0.99*1/pulse_width ,'pulse_width',pulse_width,'edge_time',risetime);

    % Phase sync between the two channels
    i.sync_phase();
    
    %% Configure modulation
    % Amplitude modulate the Channel 1 Sinewave with another internally-
    % generated sinewave. 50% modulation depth at 1Hz.
%     i.disable_modulation(1);
    i.set_burst_mode(1,'Input1','NCycle','trigger_level',1.5, 'burst_cycles',1);
    
    % Burst modulation on Channel 2 using Output 1 as the trigger
    i.set_burst_mode(2,'Input2','NCycle','trigger_level',1.5, 'burst_cycles',1);
    
    i.set_output_load(1,'1MOhm');
    i.set_output_load(2,'1MOhm');
catch ME
    % End the current connection session with your Moku
    i.relinquish_ownership();
    rethrow(ME)
end

i.relinquish_ownership();