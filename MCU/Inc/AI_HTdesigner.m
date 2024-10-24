% Define the target phase shift (90 degrees)
target_phase_shift = -90;  % Desired 90-degree phase shift

% RL parameters
alpha = 0.1;  % Learning rate
gamma = 0.95;  % Discount factor
epsilon = 0.1;  % Exploration probability
max_iter = 100000;  % Maximum iterations

fs=1000;
freq_range = [4 8];
% Initialize filter coefficients for a 6th-order IIR filter
% b = [1, zeros(1, 6)];  % 6th-order numerator coefficients
% a = [1, zeros(1, 6)];  % 6th-order denominator coefficients
[b,a]=hilbiir(1/fs,5/fs,[],4);
[H, W] = freqz(b, a, freqs);
b = b./mean(abs(H)); %scale gain to 1;
% Initialize Q-table for multiple coefficients
num_coefficients = 6;
Q_table = zeros(num_coefficients, 3);  % Three actions: increase, decrease, no change

% Frequency points for evaluating the phase response
freqs = linspace(0, pi, fs);  % Frequency range

% Reinforcement Learning Loop for Higher-Order IIR Filter
epsilon_decay = 0.995;  % Decay rate for exploration
for iter = 1:max_iter
    % Exploration-exploitation decision
    if rand < epsilon
        % Explore: Randomly select actions for each coefficient (increase, decrease, no change)
        action = randi([1, 3], 1, num_coefficients);
    else
        % Exploit: Choose the best actions from the Q-table
        [~, action] = max(Q_table, [], 2);
    end
    
    % Take actions: Modify each filter coefficient slightly based on the selected action
    parm_update_rate = 0.001;
    for i = 2:length(b)
        if action(i-1) == 1
            b(i) = b(i) + parm_update_rate;  % Increase numerator coefficient
        elseif action(i-1) == 2
            b(i) = b(i) - parm_update_rate;  % Decrease numerator coefficient
        end
    end
    for i = 2:length(a)
        if action(i-1) == 1
            a(i) = a(i) + parm_update_rate;  % Increase denominator coefficient
        elseif action(i-1) == 2
            a(i) = a(i) - parm_update_rate;  % Decrease denominator coefficient
        end
    end
    
    % Calculate the filter's phase response
    [H, W] = freqz(b, a, freqs);
    idx_sel = round(freq_range(1)/fs*2*length(freqs)):round(freq_range(2)/fs*2*length(freqs));
%         phase_response = (angle(H)) * 180 / pi;  % Convert to degrees
    real_part = real(H);
    imag_part = imag(H);
    phase_response = atan2(imag_part, real_part) * 180 / pi;  % Convert to degrees
    phase_response = unwrap(phase_response);
%     
    % Calculate the reward: How close is the phase response to 90 degrees?
    phase_error = abs(phase_response(idx_sel) - target_phase_shift);
%     phase_error(phase_error>45)=45;
    
    mag_error = mean(log10(abs(H(idx_sel))))/100;
    
    reward = -mean(phase_error)-mag_error;  % Negative mean error as reward
    % Update Q-table for each coefficient
    for i = 1:num_coefficients
        Q_table(i, action(i)) = Q_table(i, action(i)) + alpha * (reward + gamma * max(Q_table(i, :)) - Q_table(i, action(i)));
    end
    
    % Decay epsilon to reduce exploration over time
%         epsilon = epsilon * epsilon_decay;
    
    % Plot phase response every 50 iterations to visualize progress
    if mod(iter, 200) == 1
        subplot(121)
        plot(W/pi, 20*log10(abs(H)), 'b');
        xlabel('Normalized Frequency (\times\pi rad/sample)');
        ylabel('Magnitude (dB)');
        title('Magnitude Response');
        subplot(122)
        plot(W/pi, phase_response, 'b');
        hold on;
        plot(W/pi, target_phase_shift * ones(size(W)), 'r--');
        title([sprintf('Iteration: %d', iter) ' Error:' num2str(mean(phase_error))]);
        xlabel('Normalized Frequency');
        ylabel('Phase (degrees)');
        legend('Current Phase Response', 'Target 90 degrees');
        hold off;
        drawnow;
    end
end

% Final optimized filter coefficients
disp('Optimized Numerator Coefficients (b):');
disp(b);
disp('Optimized Denominator Coefficients (a):');
disp(a);
