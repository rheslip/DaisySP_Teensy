// test of DaisySP effect object for the Teensy audio library
// just does audio passthru

#include <Audio.h>
#include <Metro.h>

Metro five_sec=Metro(5000); // Set up a 5 second Metro
Metro trigger=Metro(250); // envelope trigger

#include "daisysp.h"

using namespace daisysp;

// including the source files is a pain but that way you compile in only the modules you need
// DaisySP statically allocates memory and some modules e.g. reverb use a lot of ram


// constants for integer to float and float to integer conversion
//#define MULT_16 2147483647
//#define DIV_16 4.6566129e-10
#define MULT_16 2147483647
#define DIV_16 3.05109e-5

float samplerate=AUDIO_SAMPLE_RATE_EXACT;

// create daisySP processing objects


AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioOutputI2S out;
//AudioOutputUSB outUSB;
AudioControlSGTL5000 audioShield;

AudioEffectDaisySP effect;  // create the daisysp effect audio object
AudioConnection patchCord1(audioInput,0,effect,0); //mono input

AudioConnection patchCord20(effect,0,out,0);  // send to left and right out
AudioConnection patchCord21(effect,0,out,1);
//AudioConnection patchCord22(synth,0,outUSB,0);
//AudioConnection patchCord23(synth,0,outUSB,1);



// this is the function called by the AudioSynthDaisySP object when it needs a block of samples
void AudioEffectDaisySP::update(void)
{
  float        in, out,sig;
  audio_block_t *block;

	// start of processing functions.
	block = receiveWritable();  // we will modify the samples in place
	if (!block) return;

  for (int s=0; s < AUDIO_BLOCK_SAMPLES; s++) {

	in=(float)block->data[s]*DIV_16; // convert -32767 to 32767 to -1.0 to +1.0
	
//**** insert daisySP generators here
	out=in;

    
// convert generated float value -1.0 to +1.0 to int16 used by Teensy Audio    
    int32_t val = out*MULT_16;
    block->data[s] = val >> 16;
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

       
      // Enable the AudioShield
  AudioMemory(10);  // only need 2 blocks for 1 daisySP object 
  audioShield.enable();
  audioShield.volume(0.3);
  
  Serial.println("finished setup");  
}
    float  freq=1000.0, amp;

void loop() {

    
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
