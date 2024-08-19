function [output,Hd1,Hd2] = CL02_filter_sim(data,filBPF,filLPF)
Hd1 = loadFilterFromC(filBPF);
Hd2 = loadFilterFromC(filLPF);
output = filter(Hd1,data);
output = filter(Hd2,abs(output));


function Hd = loadFilterFromC(filename)
fh = fopen(filename,'r+');txt  = textscan(fh,'%s');fclose(fh);
%%
txts = cat(2,txt{:});
decoded = cell(length(txt),1);
line_start = cellfun(@(x) ~isempty(strfind(x,'MWSPT_NSEC')),txt{1});
line_start = find(line_start,1,'first');
for idx=line_start:length(txts) %line 123 started datastructure
    try
        decoded{idx} = str2num(txts{idx});
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

