function CE32_filterGen_SOS(filename,var_name,sos,G)
fh = fopen(filename,'w+');
N = size(sos,1)+length(G);
writeline(fh,'#include "tmwtypes.h"');
writeline(fh,"#define MWSPT_NSEC " + num2str(N));

b = zeros(N,3);
b(1:2:end,1) = G;
b(2:2:end,:) = sos(:,1:3);
a = zeros(N,3);
a(1:2:end,1) = 1;
a(2:2:end,:) = sos(:,4:6);
NL_mat = ones(1,N);
NL_mat(2:2:end)=3;
NL_str = vector2txt(NL_mat);

writeline(fh,"const int NL_" + var_name + " [MWSPT_NSEC][3] = " + [NL_str{:}] + ";");
writeline(fh,"const real32_T NUM_" + var_name + " [MWSPT_NSEC][3] = {")
cellfun(@(x) writeline(fh,x),matrix2txt(b));
writeline(fh,'};');
writeline(fh,"const int DL_" + var_name + " [MWSPT_NSEC][3] = " + [NL_str{:}] + ";");
writeline(fh,"const real32_T DEN_" + var_name + " [MWSPT_NSEC][3] = {")
cellfun(@(x) writeline(fh,x),matrix2txt(a));
writeline(fh,'};');
writeline(fh,'');
fclose(fh);

function writeline(fh,txt)
fprintf(fh,'%s\n',txt);

function strs = vector2txt(mat)
strs = cell(3*size(mat,1),1);
for idx=1:size(mat,1)
    temp = arrayfun(@(x) [num2str(x),','],mat(idx,:),'uni',0);
    temp = [temp{:}];
    strs{1+3*(idx-1)} = '{ ';
    strs{2+3*(idx-1)} = temp(1:end-1);
    strs{3+3*(idx-1)} = ' }';
end

function strs = matrix2txt(mat)
strs = cell(3*size(mat,1),1);
for idx=1:size(mat,1)
    temp = arrayfun(@(x) [num2str(x),', '],mat(idx,:),'uni',0);
    temp = [temp{:}];
    strs{1+3*(idx-1)} = '{ ';
    strs{2+3*(idx-1)} = temp(1:end-1);
    strs{3+3*(idx-1)} = ' },';
end
strs{end}(end)=[]; %remove trailing  ','