# Troubleshooting and Engineering Challenges

This document describes the main technical challenges encountered during the development of the Arduino-controlled buck converter, the diagnostic methods used, and the solutions implemented.

## 1. Output Voltage Overshoot During Startup

**Problem**

During initial testing, the converter exhibited significant voltage overshoot, particularly under High-load conditions.

The PI controller initially used a nonzero base duty cycle, causing the MOSFET to begin switching before the controller could adequately regulate the output.

**Solution**

The base duty cycle was reduced to zero:

This allowed the PI controller to increase the duty cycle progressively in response to the output voltage error.

The change significantly improved startup behavior and reduced the initial overshoot.

**Key takeaway:** Controller initialization has a significant impact on the transient response of a switching converter.

---

## 2. PI Controller Tuning and Load Variations

**Problem**

The initial PI controller parameters produced different responses depending on the connected load.

Some configurations resulted in slow voltage stabilization, while more aggressive gains increased voltage oscillations and overshoot.

**Investigation**

Different proportional and integral gains were experimentally tested while monitoring output voltage and PWM duty cycle through the Arduino Serial Monitor.

**Solution**

The controller gains were progressively adjusted to improve stability.

The controller also incorporates conditional integration anti-windup to prevent excessive integral accumulation when the duty cycle reaches its limits.

**Key takeaway:** Digital PI controller parameters must be selected considering both the converter dynamics and the operating load.

---

## 3. ADC Measurement Instability During Switching

This was the most significant hardware debugging challenge encountered during the project.

### Problem

During closed-loop operation, the Arduino occasionally detected output voltages exceeding the software overvoltage threshold of 5.6 V.

However, the digital multimeter generally indicated an output voltage near 5 V.

In addition, the Serial Monitor showed unexpected voltage spikes while the PWM duty cycle remained relatively stable.

An example of the observed behavior:

```text
Vout: 4.85 V | Duty: 31.5 %
Vout: 4.84 V | Duty: 31.5 %
Vout: 4.97 V | Duty: 31.4 %
Vout: 5.51 V | Duty: 31.1 %
Vout: 4.80 V | Duty: 31.5 %
```

### Diagnostic Procedure

A separate Arduino diagnostic program was developed to isolate the ADC from the converter's output voltage.

The ADC input was connected to a resistor divider powered directly from the Arduino 5 V pin.

This provided a nominally constant ADC input while allowing the MOSFET switching circuit to operate independently.

The diagnostic firmware monitored:

- Average ADC reading
- Minimum and maximum ADC values
- Number of readings deviating significantly from the baseline
- Total number of ADC samples

The test was performed under three conditions.

### Test A — PWM Disabled

With the power stage inactive, the ADC measurements were stable.

```text
PWM: OFF | AVG: 408.9
MIN: 408 | MAX: 409
SPIKES: 0
```

This indicated that the ADC could measure a stable voltage accurately under quiet electrical conditions.

### Test B — PWM Enabled with Active Power Stage

The MOSFET was switched at 20 kHz using a fixed 20% duty cycle.

Significant ADC disturbances appeared despite the ADC measuring a voltage derived from the Arduino's own supply.

```text
PWM: ON | AVG: 413.0
MIN: 403 | MAX: 462
SPIKES: 1325
SAMPLES: 8295
```

Approximately 16% of the samples exceeded the diagnostic deviation threshold.

Following initial wiring and filtering changes, measurements still exhibited substantial disturbances:

```text
PWM: ON | AVG: 415.4
MIN: 401 | MAX: 484
SPIKES: 1660
SAMPLES: 8295
```

This confirmed that the measurement instability was strongly associated with the active switching power stage rather than the PI algorithm alone.

### Root Cause Investigation

The grounding arrangement was identified as a major contributor to the measurement interference.

The converter was implemented on breadboards, where jumper wires and electrical contacts introduce parasitic resistance and inductance.

Rapid current changes during MOSFET switching can create transient voltage differences along these connections.

Possible interference mechanisms included:

- Ground bounce
- Shared ground impedance

### Solution — Grounding Improvement

The grounding arrangement was modified to provide a more appropriate reference for analog voltage measurements.

Particular attention was given to separating sensitive ADC measurement return paths from high-current switching paths.

Following this modification, the diagnostic results improved significantly.

```text
PWM: ON | AVG: 408.2
MIN: 407 | MAX: 409
SPIKES: 0
SAMPLES: 8303
```

### Before and After Comparison

| Parameter | Before grounding correction | After grounding correction |
|---|---:|---:|
| Average ADC | 415.4 | 408.2 |
| Minimum ADC | 401 | 407 |
| Maximum ADC | 484 | 409 |
| Detected spikes per second | ~1660 | 0 |
| Switching frequency | 20 kHz | 20 kHz |

The grounding modification eliminated the observed ADC spikes under the diagnostic test conditions.

This was a key step in resolving the measurement instability encountered during development.

**Key takeaway:** Grounding and physical layout can have a major impact on measurement accuracy in switching power converters. Reliable voltage feedback requires more than a correctly implemented control algorithm.

---

## 4. ADC Voltage Calibration

**Problem**

A difference was observed between the output voltage measured by the digital multimeter and the voltage calculated by the Arduino.

For example, the multimeter indicated approximately 4.8 V while the firmware reported around 5.0 V.

**Investigation**

The Arduino uses AVcc as the ADC reference, so accurate voltage calculations require an appropriate reference value.

The voltage divider ratio must also be considered.

**Solution**

The ADC reference calibration constant was adjusted based on voltage measurements.

**Key takeaway:** ADC measurement calibration is essential when implementing closed-loop voltage regulation, because systematic measurement errors directly affect the regulated output voltage.

---

## 5. Final Results and Lessons Learned

The project demonstrated several important aspects of practical power electronics development:

1. **Simulation and hardware behavior can differ.** Real components introduce parasitic effects and measurement challenges that may not be visible in simplified simulations.

2. **Controller tuning is only one part of voltage regulation.** Inaccurate feedback measurements can cause unexpected controller behavior even when the control algorithm is mathematically correct.

3. **Grounding significantly affects ADC accuracy.** Correcting the grounding arrangement eliminated the switching-related measurement disturbances observed in the diagnostic experiment.

4. **Systematic troubleshooting is more effective than random parameter adjustments.** By isolating the ADC from the output voltage and testing it under different switching conditions, the source of the interference was narrowed down.

### Measurement Limitations

The experiments were conducted using an Arduino Uno, digital multimeter, and diagnostic measurements.

High-frequency output voltage transients were not independently characterized with an oscilloscope due to the lack of one.

## Conclusion

The development process highlighted the importance of integrating circuit design, embedded programming, control theory, and experimental debugging.

The most valuable engineering lesson was that improving the electrical measurement environment can be more effective than repeatedly adjusting controller parameters.

The project provided practical experience in diagnosing and resolving hardware-software interaction issues in a digitally controlled switching power converter.
