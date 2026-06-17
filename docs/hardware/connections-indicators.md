# Connections and Indicators

The original CL02 hardware note defines four external signal connections and three state indicators.

## Signal Connections

| Label | Role | Range or behavior |
| --- | --- | --- |
| IN0 | Analog input 0 | 0-3.3 V; can be configured with +1.5 V offset. |
| IN1 | Analog input 1 | 0-3.3 V; can be configured with +1.5 V offset. |
| DOUT | Digital trigger output | 0-3.3 V trigger output. |
| AOUT | Analog output | 0-3.3 V analog output. |

## State Indicators

| Indicator | Meaning |
| --- | --- |
| Ready | Closed-loop processor state. Slow blink means waiting; fast blink means estimating baseline; on means ready. |
| Detect | Trigger condition is armed. The final DOUT trigger still depends on stimulator settings. |
| Trigger | DOUT output state. |

!!! warning "Voltage compatibility"
    Check that every connected input accepts CL02 output voltage levels before enabling trigger output. DOUT is documented as 0-3.3 V.

## Bench Check

1. Connect only the bench input signal and a measurement instrument first.
2. Confirm IN0 or IN1 preview in the control center.
3. Confirm Ready state transitions.
4. Confirm DOUT timing and polarity on bench equipment before connecting a stimulator.
