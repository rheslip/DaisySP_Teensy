
// test of DaisySP synth object for the Teensy audio library
// simple poly synth 
// some of this code was cribbed from the Faust for Teensy Additivesynth example
// RH March 29 2021

#include <Audio.h>
#include <Metro.h>

//#define DEBUG   // comment out to remove debug code

#ifdef DEBUG
Metro five_sec=Metro(5000); // Set up a 5 second Metro for performance stats
#endif

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

// for polyphony - an array of all current notes. 
// Value -1 means the note is off (not sounding).
#define VOICES 16   // 
int StoredNotes[VOICES];

#include "daisysp.h"
using namespace daisysp;

// including the source files is a pain but that way you compile in only the modules you need
// DaisySP statically allocates memory and some modules e.g. reverb use a lot of ram
#include "synthesis/oscillator.cpp"
#include "control/adsr.cpp"
#include "filters/moogladder.cpp"
#include "effects/reverbsc.cpp"  // uses a LOT of ram

float samplerate=AUDIO_SAMPLE_RATE_EXACT;

// parameters we can modify via MIDI CCs
int waveform=0;
float detune=0;
float filterfreq=100;
float filtersweep=3000;
float filterresonance=0.3;
float reverblevel=0.1;
float lfofreq=0.1;
float lfofreqdepth=0;
float lfofilterdepth=0;

// create daisySP processing objects
#define OSCSPERVOICE 3   // note - the detune code is set up for 3 oscillators

Oscillator osc[VOICES * OSCSPERVOICE];
Oscillator lfo;
Adsr      env[VOICES];
MoogLadder filt[VOICES];
ReverbSc   verb;  

// this is the function called by the AudioSynthDaisySP object when it needs a block of samples
void AudioSynthDaisySP::update(void)
{
  float out,sig,outsig,envelope,filtsig,wetvl, wetvr;
  bool gate;
  audio_block_t *block;

  block = allocate(); // grab an audio block
  if (!block) {
    return;
  }

  for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {

//**** insert daisySP generators here

    outsig=0; // sum up voices
    for (int i=0; i<VOICES;++i) {

      sig=0;
      for (int j=0; j < OSCSPERVOICE; ++j) {
        sig+=osc[i*OSCSPERVOICE + j].Process(); // sum oscillators in each voice
      }
      sig=sig/OSCSPERVOICE; // scale down by number of oscillators
      if (StoredNotes[i] == -1) gate=false; // if voice is allocated then it is active
      else gate=true; 
      envelope=env[i].Process(gate);
      sig=sig*envelope;
      filt[i].SetFreq(filterfreq+envelope*filtersweep);
      //filt[i].SetFreq(200+envelope*3000*(lfo.Process()+1));
      filtsig=filt[i].Process(sig);
      outsig+=filtsig;
    }
    outsig=outsig*8/VOICES; // scale the sum 
    verb.Process(outsig, outsig, &wetvl, &wetvr); 

    out=outsig + wetvl*reverblevel;
    
// convert generated float value -1.0 to +1.0 to int16 used by Teensy Audio    
    int32_t val = out*MULT_16;
    block->data[i] = val >> 16;
  }
  transmit(block);
  release(block);
}

// teensy audio objects and patch creation

AudioOutputI2S out;
//AudioOutputUSB outUSB;  // USB audio breaks up badly
AudioControlSGTL5000 audioShield;
AudioSynthDaisySP synth;  // create the daisysp synth audio object

AudioConnection patchCord1(synth,0,out,0);
AudioConnection patchCord2(synth,0,out,1);
//AudioConnection patchCord3(synth,0,outUSB,0);
//AudioConnection patchCord4(synth,0,outUSB,1);

// frequencies for all 127 MIDI Note numbers.
//     C         C#        D         D#        E         F         F#        G         G#        A         A#        B

const float NoteNumToFreq[] = {
    8.18,     8.66,     9.18,     9.72,    10.30,    10.91,    11.56,    12.25,    12.98,    13.75,    14.57,    15.43,    
   16.35,    17.32,    18.35,    19.45,    20.60,    21.83,    23.12,    24.50,    25.96,    27.50,    29.14,    30.87,    
   32.70,    34.65,    36.71,    38.89,    41.20,    43.65,    46.25,    49.00,    51.91,    55.00,    58.27,    61.74,   
   65.41,    69.30,    73.42,    77.78,    82.41,    87.31,    92.50,    98.00,   103.82,   110.00,   116.54,   123.47, 
  130.81,   138.59,   146.83,   155.56,   164.81,   174.61,   184.99,   195.99,   207.65,   220.00,   233.08,   246.94,   
  261.63,   277.18,   293.66,   311.13,   329.63,   349.23,   369.99,   391.99,   415.31,   440.00,   466.16,   493.88,   
  523.25,   554.37,   587.33,   622.25,   659.26,   698.46,   739.99,   783.99,   830.61,   880.00,   932.32,   987.77,   
 1046.50,  1108.73,  1174.66,  1244.51,  1318.51,  1396.91,  1479.98,  1567.98,  1661.22,  1760.00,  1864.66,  1975.53,  
 2093.00,  2217.46,  2349.32,  2489.02,  2637.02,  2793.83,  2959.96,  3135.96,  3322.44,  3520.00,  3729.31,  3951.07,  
 4186.01,  4434.92,  4698.64,  4978.03,  5274.04,  5587.65,  5919.91,  6271.93,  6644.88,  7040.00,  7458.62,  7902.13,  
 8372.02,  8869.84,  9397.27,  9956.06, 10548.08, 11175.30, 11839.82, 12543.85 };


void setup() { 
  Serial.begin(38400);

#ifdef DEBUG
  while (!Serial) {
    // wait for Arduino Serial Monitor to be ready
  }
  Serial.println("starting setup");  
#endif

  for (int i=0; i< VOICES;++i) {  
      StoredNotes[i]=-1;  // initialize the note allocation array
      for (int j=0; j< OSCSPERVOICE; ++j) {
        osc[i*OSCSPERVOICE +j].Init(samplerate);       // initialize the voice objects
        osc[i*OSCSPERVOICE +j].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);  // changing waveforms on the fly seems to cause a crash    
      }
      env[i].Init(samplerate);
      env[i].SetTime(ADENV_SEG_DECAY, 1.0f);
      filt[i].Init(samplerate);
  }

  lfo.Init(samplerate); // Init LFO oscillator
  lfo.SetFreq(0.1);

//  env.SetCurve(-15.0f); // only for AR env
    
// initialize the reverb object and set its initial parameters
  verb.Init(samplerate);
  verb.SetFeedback(0.87);
  verb.SetLpFreq(10000.0f);
    
  // Enable the AudioShield
  AudioMemory(10);  // only uses 2 blocks
  Serial.println("enabling audio shield");    
  audioShield.enable();
  audioShield.volume(0.8);


  // Handles for the USB MIDI callbacks
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleAfterTouchPoly(myAfterTouch);
#ifdef DEBUG  
  Serial.println("finished setup");  
#endif
}


// Only looking for incoming MIDI events in the loop()
// myNoteOn(), myNoteOff() and myControlChange() will be processed on incoming MIDI messages.

void loop() {
  usbMIDI.read();
  
#ifdef DEBUG
  // DEBUG - Microcontroller Load Check
    if (five_sec.check() == 1)
    {
      Serial.print("Proc = ");
      Serial.print(AudioProcessorUsage());
      Serial.print(" (");    
      Serial.print(AudioProcessorUsageMax());
      Serial.print("),  Mem = ");
      Serial.print(AudioMemoryUsage());
      Serial.print(" (");    
      Serial.print(AudioMemoryUsageMax());
      Serial.println(")");
    }
#endif 
}

// Callback for incoming NoteOn messages
// Handling the voice allocation here.

void myNoteOn(byte channel, byte note, byte velocity) {
  int i=0;
  while( i < VOICES){
    if (StoredNotes[i] == -1) {  // if voice is idle
      StoredNotes[i] = int(note); // allocate this voice
      osc[i*OSCSPERVOICE].SetFreq(NoteNumToFreq[note]);  
      osc[i*OSCSPERVOICE+1].SetFreq(NoteNumToFreq[note]+detune);   // quick and dirty detune
      osc[i*OSCSPERVOICE+2].SetFreq(NoteNumToFreq[note]-detune);

//      env[i].Trigger(); // ADSR triggering happens in the sample loop
      break;
    }
    ++i;
  }  
}

// Callback for incoming NoteOff messages
// Releasing voices to be re-allocated here.

void myNoteOff(byte channel, byte note, byte velocity) {
  for (int i=0; i < VOICES; ++i){
    int k = int(note); 
    if (StoredNotes[i] == k) { // if this voice matches the note we are silencing
      StoredNotes[i] = -1; // deallocate the voice
    }
  }
}


// Callback for incoming CC messages
// I'm using an external MIDI controller (Arturia Beatstep) to set voice parameters
// you can also do this with pots and AnalogRead()

void myControlChange(byte channel, byte control, byte value) {
  float val = float(value) / 127; // convert to 0-1

  for (int i=0; i < VOICES; ++i){
    switch (control) {
      case 101:
        waveform=value/40; // chaning waveforms on the fly doesn't seem to work
        break;
      case 102:
        detune=val*5;  // oscillator detune
        break;
      case 103:
    
        break;
      case 105:
        env[i].SetTime(ADSR_SEG_ATTACK,val);
        break;
      case 106:
        env[i].SetTime(ADSR_SEG_DECAY,val);
        break;    
      case 107:
        env[i].SetSustainLevel(val);
        break;  
      case 108:
        env[i].SetTime(ADSR_SEG_RELEASE,val);
        break;
      case 113:
        filterfreq=50+val*2000; // filter cutoff
        break;
      case 114:
        filtersweep=val*10000; // filter sweep - controlled by envelope
        break;
      case 115:
        filt[i].SetRes(val); // filter resonance
        break;
      case 116:
        reverblevel=val; // reverb
        break;
      default:
        break;
    }
  }

}

// Callback for incoming Aftertouch messages

void myAfterTouch(byte channel, byte note, byte value) {
    float val = float(value) / 127; // convert to 0-1
    
}
  




