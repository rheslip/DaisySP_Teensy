// test of DaisySP synth object for the Teensy audio library

#include <Audio.h>
#include <Metro.h>

Metro five_sec=Metro(5000); // Set up a 5 second Metro
Metro trigger=Metro(250); // envelope trigger

#include "daisysp.h"

using namespace daisysp;

// including the source files is a pain but that way you compile in only the modules you need
// DaisySP statically allocates memory and some modules e.g. reverb use a lot of ram
#include "synthesis/oscillator.cpp"
#include "control/adenv.cpp"
#include "filters/moogladder.cpp"
#include "effects/reverbsc.cpp"  // uses a LOT of ram

// constants for integer to float and float to integer conversion
#define MULT_16 2147483647
#define DIV_16 4.6566129e-10

float samplerate=AUDIO_SAMPLE_RATE_EXACT;

// create daisySP processing objects

#define NUM_OSCS 4
Oscillator osc[NUM_OSCS];
Oscillator lfo;
AdEnv      env;
MoogLadder        filt;
ReverbSc   verb;  



AudioOutputI2S out;
AudioOutputUSB outUSB;
AudioControlSGTL5000 audioShield;

AudioSynthDaisySP synth;  // create the daisysp synth audio object

AudioConnection patchCord20(synth,0,out,0);
AudioConnection patchCord21(synth,0,out,1);
AudioConnection patchCord22(synth,0,outUSB,0);
AudioConnection patchCord23(synth,0,outUSB,1);



// this is the function called by the AudioSynthDaisySP object when it needs a block of samples
void AudioSynthDaisySP::update(void)
{
  float        out,sig, envelope, filtsig, sendsig, wetvl, wetvr;
  audio_block_t *block;

  block = allocate(); // grab an audio block
  if (!block) {
    return;
  }

  for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {

//**** insert daisySP generators here

    sig=0; // sum up oscillators
    for (int i=0; i<NUM_OSCS;++i) {
      sig+=osc[i].Process();
    }
    sig=sig/NUM_OSCS; // scale the sum 
    envelope=env.Process();
    sig=sig* envelope;
    filt.SetFreq(200+ envelope*3000*(lfo.Process()+1));
    filtsig =filt.Process(sig);
    //filtsig = filt.Low();
    sendsig = filtsig*20;
//    sendsig=sendsig*5; // crank the level a bit
    verb.Process(sendsig, sendsig, &wetvl, &wetvr); 

    out=sendsig + wetvl*0.1;
    
// convert generated float value -1.0 to +1.0 to int16 used by Teensy Audio    
    int32_t val = out*MULT_16;
    block->data[i] = val >> 16;
  }
  transmit(block);
  release(block);
}

void setup() {
  // Init Serial 
  Serial.begin(38400);
// wait for Arduino Serial Monitor to be ready
//  while (!Serial);

  Serial.println("starting setup");  

    for (int i=0; i<NUM_OSCS;++i) {
      osc[i].Init(samplerate); // Init oscillator
      osc[i].SetFreq(80+i);
      osc[i].SetWaveform(Oscillator::WAVE_SAW); // saw
    }

    lfo.Init(samplerate); // Init oscillator
    lfo.SetFreq(0.1);
    env.Init(samplerate);
    env.SetCurve(-15.0f);
    env.SetTime(ADENV_SEG_ATTACK, 0.020f);
    env.SetTime(ADENV_SEG_DECAY, 0.6f);
    env.SetMax(1.f);
    env.SetMin(0.f);
    filt.Init(samplerate);
    filt.SetRes(0.6f);
//    filt.SetDrive(0.8f);
    filt.SetFreq(2400.0f);
    verb.Init(samplerate);
    verb.SetFeedback(0.87);
    verb.SetLpFreq(10000.0f);
        
      // Enable the AudioShield
  AudioMemory(10);  // only need 2 blocks for 1 daisySP object 
  audioShield.enable();
  audioShield.volume(0.3);
  
  Serial.println("finished setup");  
}
    float  freq=1000.0, amp;

void loop() {
    float offset=-3.0/NUM_OSCS;
    for (int i=0; i<NUM_OSCS;++i) {
      //osc[i].Init(44100); // Init oscillator
      offset=offset+6/NUM_OSCS;
      osc[i].SetFreq(100+offset);
      //osc[i].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW); // saw      
      // osc[i].SetWaveform[Oscillator::WAVE_RAMP]; // enum doesn't work consistently - compiler bug?

    }

     if (trigger.check() == 1)  env.Trigger();
    
// audio stats
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
}
