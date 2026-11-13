# Arduino-PI-Controlled-Buck-Converter-9V-to-5V-DC-DC
Design, LTspice simulation, and hardware implementation of a 9V-to-5V DC-DC buck converter using ArduinoUno-based digital PI control at 20 kHz. 

## Project Overview

This project focuses on the development of a step-down DC-DC converter designed to regulate a nominal 9V battery DC input to a 5 V output.

The project covers the complete engineering process, from LTspice simulation and component selection to hardware prototyping, embedded programming, and experimental troubleshooting.

## Technical Specifications

| Parameter             | Description           |
| --------------------- | --------------------- |
| Input voltage         | 9 V nominal           |
| Target output voltage | 5 V                   |
| Switching frequency   | 20 kHz                |
| Microcontroller       | Arduino Uno           |
| Control method        | Digital PI controller |
| PWM generation        | Timer1, pin D9        |
| Voltage feedback      | ADC input A0          |
| Simulation software   | LTspice               |

## Hardware Implementation

The converter uses a P-channel MOSFET (IRF9Z34N) controlled through a 2N2222 transistor driver.

The power stage includes a Schottky flyback diode, inductor, output capacitor, and a resistive load.

Output voltage feedback is provided through a 15 kΩ / 10 kΩ voltage divider connected to the Arduino ADC.

## Embedded Control

The Arduino firmware implements:

- 20 kHz hardware PWM using Timer1
- Digital PI voltage regulation
- Duty-cycle limiting
- Overvoltage detection and shutdown
- Serial monitoring of voltage and duty cycle

## Simulation and Experimental Validation

The converter was first modeled in LTspice to investigate switching behavior, output voltage response, and inductor current.

The design was subsequently implemented and tested using physical components.

Experimental results and comparisons with the simulation are documented in the repository.

## Engineering Challenges

One of the main challenges was ADC measurement instability during MOSFET switching.

Systematic diagnostic testing identified a strong relationship between power-stage switching and measurement disturbances.

Improving the grounding arrangement eliminated the observed ADC spikes in the diagnostic test, demonstrating the importance of grounding and layout in switching power converters.

## Key Learning Outcomes

- DC-DC converter design and simulation
- MOSFET switching and gate driving
- Microcontroller PWM configuration
- Digital closed-loop control
- ADC-based voltage measurement
- Hardware debugging and grounding techniques

# Project Status

Hardware implementation and firmware development completed.

## Tools

LTspice · Arduino IDE · C++ · Digital Multimeter

This project was made during the fourth year of my Electrical and Computer Engineering studies at the Aristotle University of Thessaloniki.
