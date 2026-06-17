# External Control

The Windows control center includes an experimental external trigger override. The original instructions describe an int32 value in `d:\cl02_control.bin` used to mask DOUT.

## File-Control Example

```matlab
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
```

## Use Notes

- Treat this as an experimental override path.
- Confirm the file path and write permissions before relying on it.
- Verify DOUT masking with bench equipment before connecting stimulation hardware.
- Record whether external override was enabled in experiment metadata.

!!! warning "Fixed path"
    The original CL02 instructions describe the override path as `d:\cl02_control.bin`. If the local workstation does not have that path or drive, validate behavior before use.
