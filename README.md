# DaisySP_Teensy

DaisySP DSP Library for the Teensy 4.x 

Alpha release 0.2 June 14 2021
 
This is a port of Electrosmith's DaisySP signal processing library

https://github.com/electro-smith/DaisySP

to the PJRC Teensy 4.x. This library allows you to use the features of the Teensy Audio library and DaisySP together. Teensy Audio is fast and memory efficient but has a limited selection of DSP functions. DaisySP is a bit of a CPU and memory hog but it has some sophisticated audio processing functions. The Teensy 4.x is VERY fast and you can do a lot of signal processing with it.
 
DaisySP consists mostly of code collected from other projects - Csound, Soundpipe, and Mutable Instruments eurorack modules. DaisySP is quite similar to Soundpipe but much better written and documented. I ported Soundpipe first and then realized DaisySP is much better.

DaisySP uses floating point for all DSP operations and as such will run slowly on the Teensy 3.x - this has not been tested. On a Teensy 4.x most DaisySP functions (oscillators, envelope generators etc) consume roughly 1% of the CPU so you could create a polyphonic synth with 10 oscillators, 10 envelope generators and 10 filters and still have lots of CPU left. The physical modelling functions use quite a lot of CPU. The sine oscillator uses up more CPU since its implemented as a trig function. DaisySP has antialiased polyblep oscillators which are quite CPU efficient - much better than the simplistic and noisy waveform generators in the Teensy Audio library.

This implementation adds a DaisySP Teensy Audio synth object and a DaisySP Teensy Audio effect object to the Teensy Audio library. The DaisySP synth object has no inputs and one output ie its a generator of audio samples. The DaisySP effect object has one input and one output. The implementation currently supports only one instance of a DaisySP object - more may be possible but I'm not good enough with C++ to figure it out. 
The simplest setup is a DaisySP synth object to the Teensy Audio Shield object which is set up like this:

AudioSynthDaisySP Mysynth;  // create the daisysp synth audio object

AudioOutputI2S out;   // audio shield object

AudioControlSGTL5000 audioShield;  // control channel

AudioConnection patchCord1(Mysynth,0,out,0);  // patch mono synth to right and left channels

AudioConnection patchCord2(Mysynth,0,out,1);


A DaisySP effect object would be set up like this:

AudioEffectDaisySP Myeffect;  // create the daisysp effect audio object

AudioInputI2S       audioInput;         // audio shield: mic or line-in

AudioOutputI2S out;   // audio shield object

AudioControlSGTL5000 audioShield;  // control channel

AudioConnection patchCord1(audioInput,0,Myeffect,0);  // mono input

AudioConnection patchCord2(Myeffect,0,out,0);  // patch mono synth to right and left channels

AudioConnection patchCord3(Myeffect,0,out,1);


Teensy Audio processes 128 16 bit integer samples at a time and uses a pool of sample buffers which are passed between audio objects. This is memory and CPU efficient but can make coding audio objects quite complicated because of the sample buffer management. Every audio object has a callback function which processes blocks of samples approximately every 2.3 ms @ 44.1khz sample rate.

In contrast, DaisySP processes one sample at a time using floating point and each function allocates its memory statically. Simple, but uses a lot of memory for things like reverbs and delays and its pretty CPU intensive. 

To use DaisySP with Teensy Audio we process 128 samples at a time, convert the floating point results to integer and pass them to the next Teensy audio object in the patch. The DaisySP audio object has a callback function called AudioSynthDaisySP::update (or AudioEffectDaisySP::update) which does this. You must have this function in your sketch and this is where you call DaisySP library functions. Look at the example sketches to see how this works.

Installing the library:

Copy the contents of the DaisySP folder to your arduino/library folder

Copy the file Teensy/Audio/synth_daisysp.h and Teensy/Audio/effect_daisysp.h (the DaisySP audio objects) to your Teensy audio library - usually this will be your_Arduino_installation_directory/hardware/teensy/avr/libraries/audio. 

Teensy/Audio/Audio.h has the lines #include synth_daisysp.h and #include effect_daisysp.h added so you can replace your Teensy audio library Audio.h with this file. Its probably better edit your existing Audio.h file - I can't guarantee I will be tracking future changes to the Teensy Audio library.


I decided to structure the library so you have to manually include the DaisySP *.cpp files you are using in your sketch vs compiling the whole library into the sketch. This is a bit of a pain but including the whole library uses almost 500k of program memory and close to 500k of RAM which leaves very little RAM for the rest of your code. There is currently no provision for using the optional PSRAM on the Teensy 4.1.

I have not tested the library extensively. It should be fairly simple to add a DaisySP audio effect object that has two outputs for the DaisySP functions that generate stereo audio. I have not had a need for this yet so its not in the library.

There are compile problems with the resonator objects. I think this an issue with derived classes but I have not been able to fix it as yet.

Tested with Arduino 1.85 and Teensyduino 1.53.

Rich Heslip rheslip@hotmail.com




