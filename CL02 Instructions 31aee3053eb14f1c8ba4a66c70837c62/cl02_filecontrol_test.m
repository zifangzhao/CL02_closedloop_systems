file='d:\cl02_control.bin';
value=1;
while(1)
    try
        fh=fopen(file,'w');
        fwrite(fh,value,'int32');
        fclose(fh);
        value=~value;
        disp(['Updated:' num2str(value)]);
        pause(5)
        
    catch
    end
end
   
   