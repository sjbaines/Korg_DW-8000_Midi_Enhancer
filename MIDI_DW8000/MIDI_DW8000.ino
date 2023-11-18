// Korg DSS-1 MIDI Enhancer
// By Steve Baines 2016; moded by Vladistone, 2023
// See: https://hackaday.io/

#include <MIDI.h>

// Create MIDI interface on main hardware serial port
// (Tx/Rx pins on Arduino board)
MIDI_CREATE_DEFAULT_INSTANCE();

// -----------------------------------------------------------------------------
// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html

const int ledPinOut = 13;
const int dssChannel = 0;	// 0 here is 1 on DSS-1, so necessary to check&set MIDI Mode parameters of below:
// MIDI F1 Channel Select TRANS: 01
// MIDI F2 Program Change Mode: MODE1 or MODE2; MODE3
// MIDI F2 Modulation Mode: ON
// MIDI F2 After-touch Mode: ON
// MIDI F3 OMNY Mode: ON
// otherwise someone or all SysEx messages will ignored.

void sendDSS1Param_8(byte channel, byte paramOffset, byte paramValue7Bit)
{
  const int sysexLen = 8;
  static byte sysexData[sysexLen] = {
	0xf0, // 0 Sysex start
	0x42, // 1 Manufacturer ID: 42, Korg
	0x30, // 2 Channel 1
	0x0b, // 3 Device ID: 0b, DSS-1
	0x41, // 4 Message: 41, Parameter change
	0x00, // 5 Parameter Offset number (which parameter we want to change)
	0x00, // 6 Parameter value
	0xf7  // 7 Sysex end
  };

  // paramValue7Bit is assumed to run from 0 to 127 in all cases,
  paramValue7Bit &= 0x7f;


#if(0)
	case 0:  // Osc 1 Level (101)
	case 1:  // Osc 2 Level (101)
	case 2:  // A.Bend Intesity (128)
	case 3:  // Noise Level (64)
	case 4:  // VCF Mode/Slope (2)
	case 5:  // VCF EG Polarity (2)
	case 6:  // VCF Cutoff (128)
	case 7:  // VCF EG Intensity (64)
	case 8:  // VCF Resonance (64)
	case 9:  // VCF Kbd Track (64)
	case 10: // VCF MG Frequency (64)
	case 11: // VCF MG Delay (64)
	case 12: // VCF MG Intensity (64)
	case 13: // VCF EG Attack (64)
	case 14: // VCF EG Decay (64)
	case 15: // VCF EG Breakpoint (64)
	case 16: // VCF EG Slope (64)
	case 17: // VCF EG Sustain (64)
	case 18: // VCF EG Release (64)
	case 19: // VCA Kbd Decay (128)
	case 20: // VCA Level (64)
	case 21: // VCA EG Attack (64)
	case 22: // VCA EG Decay (64)
	case 23: // VCA EG Breakpoint (64)
	case 24: // VCA EG Slope (64)
	case 25: // VCA EG Sustain (64)
	case 26: // VCA EG Release (64)
	case 27: // Veloc ABend Intens (64)
	case 28: // Veloc VCF Cutoff (16)
	case 29: // Veloc VCF EG Attack (64)
	case 30: // Veloc VCF EG Decay (64)
	case 31: // Veloc VCF EG Slope (64)
	case 32: // VCA EG Intensity (64)
	case 33: // Veloc VCA EG Attack (64)
	case 34: // Veloc VCA EG Decay (64)
	case 35: // Veloc VCA EG Slope (64)
	case 36: // ATch Osc MG Intens (16)
	case 37: // ATch VCF Level (16)
	case 38: // ATch VCF Mode (2)
	case 39: // ATch VCA Level (16)
	case 40: // JStck PBend Range (13)
	case 41: // JStck VCF Mode (2)
	case 42: // EQ Bass (13)
	case 43: // EQ Treble (13)
	case 44: // DDL MG-A Freq (64)
	case 45: // DDL MG-B Freq (64)
	case 46: // DDL-1 Time (a) (128)
	case 46: // DDL-1 Time (b) (128)
	case 46: // DDL-1 Time (c) (128)
	case 46: // DDL-1 Time (d) (117)
	case 47: // DDL-1 Feedback (16)
	case 48: // DDL-1 Effect Level (16)
	case 49: // DDL-1 MG-A Intens (64)
	case 50: // DDL-1 MG-B Intens (64)
	case 51: // DDL-2 Input Select (2)
	case 52: // DDL-2 Time (a) (128)
	case 52: // DDL-2 Time (b) (128)
	case 52: // DDL-2 Time (c) (128)
	case 52: // DDL-2 Time (d) (117)
	case 53: // DDL-2 Feedback (16)
	case 54: // DDL-2 Effect Level (16)
	case 55: // DDL-2 MG-A Intens (64)
	case 56: // DDL-2 MG-B Intens (64)
	case 57: // DDL-2 Mod Invert (2)
	case 58: // Osc 1 Multisound (16)
	case 59: // Osc 2 Multisound (16)
	case 61: // Sync Mode (2)
	case 62: // Bit D A Resolution (5)
	case 63: // Osc 1 Octave (3)
	case 64: // Osc 2 Octave (3)
	case 65: // Osc 2 Detune (64)
	case 66: // Osc 2 Interval (12)
	case 67: // Osc MG Select (4)
	case 68: // Osc MG Frequency (32)
	case 69: // Osc MG Intensity (64)
	case 70: // Osc MG Delay (16)
	case 71: // A.Bend Select (4)
	case 72: // A.Bend Polarity Mode (2)
	case 73: // A.Bend Time (32)
	case 74: // Unison Detune (8)
	case 75: // Veloc Osc X-Switch (32)
	case 76: // Key Assign mode (3)
	case 77: // Unison Voices (4)
	case : // Max OSC Band Range (13)
#endif

  // so here we rescale to fit to appropriate bit width for each parameter
  byte paramValueScaled = 0;
  switch (paramOffset)
  {
	case 4:  // VCF Mode/Slope (2)
	case 5:  // VCF EG Polarity (2)
	case 38: // ATch VCF Mode (2)
	case 41: // JStck VCF Mode (2)
	case 51: // DDL-2 Input Select (2)
	case 57: // DDL-2 Mod Invert (2)
	case 61: // Sync Mode (2)
	case 72: // A.Bend Polarity Mode (2)
	  paramValueScaled = paramValue7Bit >> 6;
	  break;

	case 67: // Osc MG Select (4)
	case 71: // A.Bend Select (4)
	case 77: // Unison Voices (4)
	  paramValueScaled = paramValue7Bit >> 5;
	  break;

	case 74: // Unison Detune (8)
	  paramValueScaled = paramValue7Bit >> 4;
	  break;

	case 28: // Veloc VCF Cutoff (16)
	case 36: // ATch Osc MG Intens (16)
	case 37: // ATch VCF Level (16)
	case 39: // ATch VCA Level (16)
	case 47: // DDL-1 Feedback (16)
	case 48: // DDL-1 Effect Level (16)
	case 53: // DDL-2 Feedback (16)
	case 54: // DDL-2 Effect Level (16)
	case 58: // Osc 1 Multisound (16)
	case 59: // Osc 2 Multisound (16)
	case 70: // Osc MG Delay (16)
	  paramValueScaled = paramValue7Bit >> 3;
	  break;

	case 68: // Osc MG Frequency (32)
	case 73: // A.Bend Time (32)
	case 75: // Veloc Osc X-Switch (32)
	  paramValueScaled = paramValue7Bit >> 2;
	  break;

	case 3:  // Noise Level (64)
	case 7:  // VCF EG Intensity (64)
	case 8:  // VCF Resonance (64)
	case 9:  // VCF Kbd Track (64)
	case 10: // VCF MG Frequency (64)
	case 11: // VCF MG Delay (64)
	case 12: // VCF MG Intensity (64)
	case 13: // VCF EG Attack (64)
	case 14: // VCF EG Decay (64)
	case 15: // VCF EG Breakpoint (64)
	case 16: // VCF EG Slope (64)
	case 17: // VCF EG Sustain (64)
	case 18: // VCF EG Release (64)
	case 20: // VCA Level (64)
	case 21: // VCA EG Attack (64)
	case 22: // VCA EG Decay (64)
	case 23: // VCA EG Breakpoint (64)
	case 24: // VCA EG Slope (64)
	case 25: // VCA EG Sustain (64)
	case 26: // VCA EG Release (64)
	case 27: // Veloc ABend Intens (64)
	case 29: // Veloc VCF EG Attack (64)
	case 30: // Veloc VCF EG Decay (64)
	case 31: // Veloc VCF EG Slope (64)
	case 32: // VCA EG Intensity (64)
	case 33: // Veloc VCA EG Attack (64)
	case 34: // Veloc VCA EG Decay (64)
	case 35: // Veloc VCA EG Slope (64)
	case 44: // DDL MG-A Freq (64)
	case 45: // DDL MG-B Freq (64)
	case 49: // DDL-1 MG-A Intens (64)
	case 50: // DDL-1 MG-B Intens (64)
	case 55: // DDL-2 MG-A Intens (64)
	case 56: // DDL-2 MG-B Intens (64)
	case 65: // Osc 2 Detune (64)
	case 69: // Osc MG Intensity (64)
	  paramValueScaled = paramValue7Bit >> 1;
	  break;

	case 2:  // A.Bend Intesity (128)
	case 6:  // VCF Cutoff (128)
	case 19: // VCA Kbd Decay (128)
	  paramValueScaled = paramValue7Bit;
	  break;

	case 63: // Osc 1 Octave (3)  // for this parameter and below we are use operation `/` divide
	case 64: // Osc 2 Octave (3)
	case 76: // Key Assign mode (3)
	  paramValueScaled = paramValue7Bit/43;
	  break;

	case 62: // Bit D A Resolution (5)
	  paramValueScaled = paramValue7Bit/26;
	  break;

	case 66: // Osc 2 Interval (12)
	  paramValueScaled = paramValue7Bit*24/256;
	  break;

	case 40: // JStck PBend Range (13)
	case 42: // EQ Bass (13)
	case 43: // EQ Treble (13)
	  paramValueScaled = paramValue7Bit*26/256;
	  break;

	case 0:  // Osc 1 Level (101)
	case 1:  // Osc 2 Level (101)
	  paramValueScaled = paramValue7Bit*203/256;
	  break;

	// case : // Max OSC Band Range (13) the SysEx unclear, indefinite!
	// case 46/47: // DDL-1 Time (a) (128)
	// case 46/47: // DDL-1 Time (b) (128)
	// case 46/47: // DDL-1 Time (c) (128)
	// case 46/47: // DDL-1 Time (d) (117)
	// case 53/54: // DDL-2 Time (a) (128)
	// case 53/54: // DDL-2 Time (b) (128)
	// case 53/54: // DDL-2 Time (c) (128)
	// case 53/54: // DDL-2 Time (d) (117)
	//  paramValueScaled = paramValue7Bit - is understand how to convert value 501 of rahge?;
	//  break;

	default:
	return;	// Unknown parameter - ignore
  }

  sysexData[2] = 0x30 | (channel & 0x0f);	// Set channel number
  sysexData[5] = paramOffset;
  sysexData[6] = paramValueScaled;

  MIDI.sendSysEx(sysexLen, sysexData, true);
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
	// Do whatever you want when a note is pressed.
	digitalWrite(ledPinOut, HIGH);	// turn the LED on
	MIDI.sendNoteOn(pitch, velocity, channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
	// Do something when the note is released.
	// Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
	digitalWrite(ledPinOut, LOW);	// turn the LED off
	MIDI.sendNoteOff(pitch, velocity, channel);
}

void handleControlChange(byte channel, byte number, byte value)
{
  if (channel != dssChannel)
  { // If CC not sent to DSS-1 channel, then just pass them on
	MIDI.sendControlChange(number, value, channel);
	return;
  }

  // Map from received CC numbers to corresponding DSS-1 Parameter offset numbers
  switch(number)
  {
	// Mappings to common CCs: (See http://nickfever.com/music/midi-cc-list)
	//   CC#		Param. Offset
	//   ||				 ||
	//	SSL Nucleus Fader group L
	case 0: sendDSS1Param_8(channel, 0, value); break;  // OSC1 Level
	case 1: sendDSS1Param_8(channel, 1, value); break;  // OSC2 Level
	case 2: sendDSS1Param_8(channel, 3, value); break;  // Noise Level
	case 3: sendDSS1Param_8(channel, 65, value); break; // OSC1 Octave
	case 4: sendDSS1Param_8(channel, 66, value); break; // OSC2 Octave
	case 5: sendDSS1Param_8(channel, 2, value); break;  // A.Bend Intesity/ Portameto Mix
	case 6: sendDSS1Param_8(channel, 79, value); break; // Unison Voices
	case 7: sendDSS1Param_8(channel, 20, value); break; // VCA Level
	//	SSL Nucleus Fader group R
	case 8: sendDSS1Param_8(channel, 42, value); break;  // EQ Bass
	case 9: sendDSS1Param_8(channel, 43, value); break;  // EQ Treble
	case 10: sendDSS1Param_8(channel, 21, value); break; // VCA EG Attack
	case 11: sendDSS1Param_8(channel, 22, value); break; // VCA EG Decay
	case 12: sendDSS1Param_8(channel, 23, value); break; // VCA EG Breakpoint
	case 13: sendDSS1Param_8(channel, 24, value); break; // VCA EG Slope
	case 14: sendDSS1Param_8(channel, 25, value); break; // VCA EG Sustain
	case 15: sendDSS1Param_8(channel, 26, value); break; // VCA EG Release
	//	SSL Nucleus V-Coder group L
	case 16: sendDSS1Param_8(channel, 60, value); break; // OSC1 Multisound
	case 17: sendDSS1Param_8(channel, 61, value); break; // OSC2 Multisound
	case 18: sendDSS1Param_8(channel, 64, value); break; // Bit D A Resolution
	case 19: sendDSS1Param_8(channel, 68, value); break; // OSC2 Interval
	case 20: sendDSS1Param_8(channel, 67, value); break; // OSC2 Detune
	case 21: sendDSS1Param_8(channel, 75, value); break; // A.Bend Time/ Portamento time
	case 22: sendDSS1Param_8(channel, 76, value); break; // Unison Detune
	case 23: sendDSS1Param_8(channel, 77, value); break; // Velocity OSC X-Switch
	//	SSL Nucleus V-Coder group R
	case 24: sendDSS1Param_8(channel, 6, value); break;  // VCF Cutoff
	case 25: sendDSS1Param_8(channel, 8, value); break;  // VCF Resonance
	case 26: sendDSS1Param_8(channel, 13, value); break; // VCF EG Attack
	case 27: sendDSS1Param_8(channel, 14, value); break; // VCF EG Decay
	case 28: sendDSS1Param_8(channel, 15, value); break; // VCF EG Breakpoint
	case 29: sendDSS1Param_8(channel, 16, value); break; // VCF EG Slope
	case 30: sendDSS1Param_8(channel, 17, value); break; // VCF EG Sustain
	case 31: sendDSS1Param_8(channel, 18, value); break; // VCF EG Release
	//	SSL Nucleus Select SW group L
	case 64: sendDSS1Param_8(channel, 38, value); break; // ATch VCF Mode
	case 65: sendDSS1Param_8(channel, 41, value); break; // JStck VCF Mode
	case 66: sendDSS1Param_8(channel, 63, value); break; // Sync Mode
	case 67: sendDSS1Param_8(channel, 52, value); break; // DDL-2 Input Select Dir/parallel/DLL-1
	case 68: sendDSS1Param_8(channel, 59, value); break; // DDL-2 Mod Invert
	case 69: sendDSS1Param_8(channel, 74, value); break; // A.Bend Polarity Mode Down/Up
	case 70: sendDSS1Param_8(channel, 4, value); break;  // VCF Mode/Slope: on/off
	case 71: sendDSS1Param_8(channel, 5, value); break;  // VCF EG Polarity: on/off
	//	SSL Nucleus Select SW group R
	//case 72: sendDSS1Param_8(channel, , value); break; //
	//case 73: sendDSS1Param_8(channel, , value); break; //
	//case 74: sendDSS1Param_8(channel, , value); break; //
	//case 75: sendDSS1Param_8(channel, , value); break; //
	//case 76: sendDSS1Param_8(channel, , value); break; //
	//case 77: sendDSS1Param_8(channel, , value); break; //
	//case 78: sendDSS1Param_8(channel, , value); break; //
	//case 79: sendDSS1Param_8(channel, , value); break; //
	//	SSL Nucleus V-Coder Select group L
	//case 80: sendDSS1Param_8(channel, , value); break; //
	//case 81: sendDSS1Param_8(channel, , value); break; //
	//case 82: sendDSS1Param_8(channel, , value); break; //
	//case 83: sendDSS1Param_8(channel, , value); break; //
	//case 84: sendDSS1Param_8(channel, , value); break; //
	//case 85: sendDSS1Param_8(channel, , value); break; //
	//case 86: sendDSS1Param_8(channel, , value); break; //
	//case 87: sendDSS1Param_8(channel, , value); break; //
	//	SSL Nucleus V-Coder Select group R
	//case 88: sendDSS1Param_8(channel, , value); break; //
	//case 89: sendDSS1Param_8(channel, , value); break; //
	//case 90: sendDSS1Param_8(channel, , value); break; //
	//case 91: sendDSS1Param_8(channel, , value); break; //
	//case 92: sendDSS1Param_8(channel, , value); break; //
	//case 93: sendDSS1Param_8(channel, , value); break; //
	//case 94: sendDSS1Param_8(channel, , value); break; //
	//case 95: sendDSS1Param_8(channel, , value); break; //

#if(0)
	CC   Offset #  Param name
	0    case 0:   // Osc 1 Level
	1    case 1:   // Osc 2 Level
	2    case 3:   // Noise Level
	3    case 65:  // Osc 1 Octave
	4    case 66:  // Osc 2 Octave
	5    case 2:   // A.Bend Intesity/ Portameto mix
	6    case 79:  // Unison Voices
	7    case 20:  // VCA Level
	8    case 42:  // EQ Bass
	9    case 43:  // EQ Treble
	10    case 21: // VCA EG Attack
	11    case 22: // VCA EG Decay
	12    case 23: // VCA EG Breakpoint
	13    case 24: // VCA EG Slope
	14    case 25: // VCA EG Sustain
	15    case 26: // VCA EG Release
	16    case 60: // Osc 1 Multisound
	17    case 61: // Osc 2 Multisound
	18    case 64: // Bit D A Resolution
	19    case 68: // Osc 2 Interval
	20    case 67: // Osc 2 Detune
	21    case 75: // A.Bend Time/ Portamento time
	22    case 76: // Unison Detune
	23    case 77: // Veloc Osc X-Switch
	24    case 6:  // VCF Cutoff
	25    case 8:  // VCF Resonance
	26    case 13: // VCF EG Attack
	27    case 14: // VCF EG Decay
	28    case 15: // VCF EG Breakpoint
	29    case 16: // VCF EG Slope
	30    case 17: // VCF EG Sustain
	31    case 18: // VCF EG Release
	65    case 52: // DDL-2 Input Select
	66    case 59: // DDL-2 Mod Invert
	67    case 38: // ATch VCF Mode
	68    case 41: // JStck VCF Mode
	68    case 63: // Sync Mode
	69    case 74: // A.Bend Polarity Mode
	70    case 4:  // VCF Mode/Slope
	71    case 5:  // VCF EG Polarity
#endif

	default:
	// If not explicitly remapped, pass on
	MIDI.sendControlChange(number, value, channel);
	break;
  }
}

void handleProgramChange(byte channel, byte number)
{
  MIDI.sendProgramChange(number, channel);
}

void handleAfterTouchChannel(byte channel, byte pressure)
{
  MIDI.sendAfterTouch(pressure, channel);
}

void handlePitchBend(byte channel, int bend)
{
  MIDI.sendPitchBend(bend, channel);
}

void handleSystemExclusive(byte* arrayData, unsigned arrayLen)
{
  MIDI.sendSysEx(arrayLen, arrayData, false);
}

// -----------------------------------------------------------------------------

void setup()
{
	// initialize digital pin with LED as an output.
	pinMode(ledPinOut, OUTPUT);
	digitalWrite(ledPinOut, LOW);	// turn the LED off by making the voltage LOW

	// Connect the handleNoteOn function to the library,
	// so it is called upon reception of a NoteOn.
	MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

	// Do the same for NoteOffs
	MIDI.setHandleNoteOff(handleNoteOff);

	MIDI.setHandleControlChange(handleControlChange);

	MIDI.setHandleProgramChange(handleProgramChange);
	MIDI.setHandleAfterTouchChannel(handleAfterTouchChannel);
	MIDI.setHandlePitchBend(handlePitchBend);
	MIDI.setHandleSystemExclusive(handleSystemExclusive);

	// Initiate MIDI communications, listen to all channels
	MIDI.begin(MIDI_CHANNEL_OMNI);
	MIDI.turnThruOff(); // Prevent passthrough
}

void loop()
{
	// Call MIDI.read the fastest you can for real-time performance.
	MIDI.read();

	// There is no need to check if there are messages incoming
	// if they are bound to a Callback function.
	// The attached method will be called automatically
	// when the corresponding message has been received.
}
