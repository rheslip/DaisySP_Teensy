# DaisySP_Teensy

DaisySP DSP Library for the Teensy 4
 
This is a port of Electrosmith's DaisySP signal processing library to the PJRC Teensy 4.x https://github.com/electro-smith/DaisySP. Its a Teensy Audio library object and allows you to use the features of both. Teensy Audio is fast and memory efficient but has a limited selection of DSP functions. DaisySP is a CPU and memory hog but it has a lot of very sophisticated audio processing functions. The Teensy 4.x is VERY fast and you can do a lot of signal processing with it.
 
DaisySP is a DSP library consisting mostly of code collected from other projects - Csound, Soundpipe, and Mutable Instruments eurorack modules. DaisySP is quite similar to Soundpipe but much better written and documented. I ported Soundpipe first and then realized DaisySP is much better.

DaisySP uses floating point for all DSP operations and as such will run slowly on the Teensy 3.x - this has not been tested. On a Teensy 4.x each DaisySP function consumes roughly 1% of the CPU so you could create a polyphonic synth with 10 oscillators, 10 envelope generators and 10 filters and still have lots of CPU left. The sine oscillator uses up more CPU since its implemented as a trig function. DaisySP has antialiased polyblep oscillators which are quite CPU efficient.

This implementation creates DaisySP as a Teensy Audio object. Currently it supports one instance of a DaisySP object - more may be possible but I'm not good enough with C++ to figure it out. A DaisySP object is a Teensy Audio synth - it has no inputs and it outputs a single stream of audio samples with can be processed by other Teensy audio objects.
The simplest setup is a DaisySP object to the Teensy Audio Sheild object which is set up like this:

AudioSynthDaisySP synth;  // create the daisysp synth audio object

AudioOutputI2S out;   // audio shield object
AudioControlSGTL5000 audioShield;  // control channel

AudioConnection patchCord1(synth,0,out,0);  // patch mono synth to right and left channels
AudioConnection patchCord2(synth,0,out,1);

In DaisySP everything is processed one sample at a time and each function allocates its memory statically. Simple, but it uses a lot of memory for things like reverbs and delays.

Teensy Audio processes 128 samples at a time and uses a dynamic pool of sample buffers which are passed between audio objects. This is generally more CPU efficient but can make coding audio objects quite complicated because of the dynamic sample buffer management. Every audio object has a callback function which processes blocks of samples approximately every 2.3 ms @ 44.1khz sample rate.

The DaisySP audio object must have a callback function called AudioSynthDaisySP::update which processes 128 samples and passes them on to the next audio object. You must have this function in your sketch - this is where you call DaisySP library functions. Look at the example sketch to see how this works.

Installing the library:

Copy the contents of the DaisySP folder to your arduino/library folder

Copy the file Teensy/Audio/synth_daisysp.h (the DaisySP audio object) to your Teensy audio library - usually this will be <installation directory>/hardware/teensy/avr/libraries/audio. Teensy/Audio/Audio.h has been modified to add synth_daisysp.h. You may want to do this manually since I probably won't be tracking future changes to the Teensy Audio library.


I decided to structure the library so you have to manually include the DaisySP *.cpp files you are using in your sketch vs compiling the whole library into the sketch. This is a bit of a pain but including the whole library uses almost 500k of program memory and close to 500k of RAM which leaves very little RAM for the rest of your code. There is currently no provision for using the PSRAM on the Teensy 4.1.

I have not tested the library extensively but so far everything works as expected. It should be fairly simple to add a DaisySP audio effect object (ie one that has inputs and outputs) and one that generates stereo audio out (some of the DaisySP functions generate stereo audio). I have not had a need for this yet so its not in the library.

Tested with Arduino 1.85 and Teensyduino 1.53.



