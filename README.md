# Korg_DW-8000_Midi_Enhancer
Simple Arduino project to remap MIDI CC messages into SYSEX messages that Korg DW-8000 Synth expects.

5th July 2016
The Korg DW-8000 (and other early synthesisers) was created during the early days of MIDI, before things like CC messages were standardised.  As a result, it uses proprietary SYSEX commands to change parameters over MIDI.  Most modern MIDI controllers use CC messages instead, and very few are flexible enough to be able to send the appropriate SYSEX messages.

This project uses an embedded Arduino Nano (or clone) mounted inside the DW-8000, which is used to intercept the MIDI IN data just before it reaches the DW-8000 CPU. It replaces specific CC messages with SYSEX messages that the DW-8000 understands them.

The practical upshot of this is that the DW-8000 parameters can be controlled by standard CC messages, rather than obscure SYSEX messages.

MIDI THRU is unaffected by the changes, as the Nano is be spliced in just before the MIDI IN reaches the CPU, AFTER the IN has been echoed back out of the THRU port.

Certain CC messages are replaced with SYSEX parameter control messages, but all other MIDI messages are passed unchanged. This means that the SYSEX messages can still be used.

