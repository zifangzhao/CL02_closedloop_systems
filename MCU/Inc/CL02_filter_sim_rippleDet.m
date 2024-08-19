function output = CL02_filter_sim_rippleDet(data,fs)



function Hd = loadFilterFromC(filename)
fh = fopen(filename,'r+');txt  = textscan(fh,'%s');fclose(fh);
%%
decoded = cell(length(txt{1}),1);
for idx=123:length(txt{1}) %line 123 started datastructure
    try
        decoded{idx} = str2num(txt{1}{idx});
    end
end
decoded_nums = decoded(cellfun(@(x) ~isempty(x),decoded));
ord = decoded_nums{1};
b= [decoded_nums{3:3+ord*3-1}];
a= [decoded_nums{3+ord*3+1:end}];
NL_D = decoded_nums{2};
DL_D = decoded_nums{3+ord*3};
b= reshape(b,[],ord)';
a= reshape(a,[],ord)';

% Initialize an empty SOS matrix
sos = [];

% Loop through each section
for i = 1:ord
    % Extract the coefficients for the current section
%     num = b(i, 1:NL_D(i));
%     den = a(i, 1:DL_D(i));
    num = b(i, :);
    den = a(i, :);
    % Concatenate to the SOS matrix
    sos = [sos; num, den];
end

% Create a scale vector with all ones
g = ones(ord, 1);

% Create the digital filter object
Hd = dfilt.df2sos(sos, g);
