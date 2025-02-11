# Electronic Sand Glass

## Overview
This project is an interactive Electronic Sand Glass, developed by Diogo Chaves e Joana Pereira within the scope of the curricular unit Embedded Computing Architectures.

Using a Raspberry Pi Pico, design an electronic sand glass that acts as a timer. Finally, present a small report with the final schematic, the projected finite state machines, and a description of the corresponding software implementation.

At the press of a button, five LEDs from an RGB LED strip should be on. At predetermined intervals, each would switch off until they all were off. When the predefined time is up, all will fast-blink with the red colour. The timer should be able to be paused, and the remaining time should be able to be increased while counting down. 

The buttons Sgo, Sup, and Sdown should be connected to GPIO pins GP2, GP3, and GP4. When pressed, all buttons should connect their pin to GND. The LED strip must be powered by the 5V line, and GP6 is used for communication. Use the serial port to report each button reading, all LED states, and the current state for each State Machine.
