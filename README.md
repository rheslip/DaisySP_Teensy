# DaisySP_Teensy

DaisySP DSP Library for the Teensy 4

Alpha release 0.1 March 28 2021
 
This is a port of Electrosmith's DaisySP signal processing library to the PJRC Teensy 4.x https://github.com/electro-smith/DaisySP. This library allows you to use the features of the Teensy Audio library and DaisySP together. Teensy Audio is fast and memory efficient but has a limited selection of DSP functions. DaisySP is a bit of a CPU and memory hog but it has some sophisticated audio processing functions. The Teensy 4.x is VERY fast and you can do a lot of signal processing with it.
 
DaisySP consists mostly of code collected from other projects - Csound, Soundpipe, and Mutable Instruments eurorack modules. DaisySP is quite similar to Soundpipe but much better written and documented. I ported Soundpipe first and then realized DaisySP is much better.

DaisySP uses floating point for all DSP operations and as such will run slowly on the Teensy 3.x - this has not been tested. On a Teensy 4.x each DaisySP function call consumes roughly 1% of the CPU so you could create a polyphonic synth with 10 oscillators, 10 envelope generators and 10 filters and still have lots of CPU left. The sine oscillator uses up more CPU since its implemented as a trig function. DaisySP has antialiased polyblep oscillators which are quite CPU efficient - much better than the simplistic and noisy waveform generators in the Teensy Audio library.

This implementation adds a DaisySP Teensy Audio synth object to the Teensy Audio library. An Audio Library synth object has no inputs and it outputs a single stream of audio samples. The library currently supports only one instance of a DaisySP object - more may be possible but I'm not good enough with C++ to figure it out. 
The simplest setup is a DaisySP object to the Teensy Audio Shield object which is set up like this:

AudioSynthDaisySP synth;  // create the daisysp synth audio object

AudioOutputI2S out;   // audio shield object

AudioControlSGTL5000 audioShield;  // control channel

AudioConnection patchCord1(synth,0,out,0);  // patch mono synth to right and left channels

AudioConnection patchCord2(synth,0,out,1);

Teensy Audio processes 128 16 bit integer samples at a time and uses a dynamic pool of sample buffers which are passed between audio objects. This is memory and CPU efficient but can make coding audio objects quite complicated because of the dynamic sample buffer management. Every audio object has a callback function which processes blocks of samples approximately every 2.3 ms @ 44.1khz sample rate.

In contrast, DaisySP processes one sample at a time using floating point and each function allocates its memory statically. Simple, but uses a lot of memory for things like reverbs and delays and its pretty CPU intensive. 

To use DaisySP with Teensy Audio we process 128 samples at a time, convert the floating point results to integer and pass them to the next Teensy audio object in the patch. The DaisySP audio object has a callback function called AudioSynthDaisySP::update which does this. You must have this function in your sketch and this is where you call DaisySP library functions. Look at the example sketch to see how this works.

Installing the library:

Copy the contents of the DaisySP folder to your arduino/library folder

Copy the file Teensy/Audio/synth_daisysp.h (the DaisySP audio object) to your Teensy audio library - usually this will be your_Arduino_installation_directory/hardware/teensy/avr/libraries/audio. 

Teensy/Audio/Audio.h has been modified to include synth_daisysp.h. You may want to do this manually since I can't guarantee I will be tracking changes to the Teensy Audio library.


I decided to structure the library so you have to manually include the DaisySP *.cpp files you are using in your sketch vs compiling the whole library into the sketch. This is a bit of a pain but including the whole library uses almost 500k of program memory and close to 500k of RAM which leaves very little RAM for the rest of your code. There is currently no provision for using the optional PSRAM on the Teensy 4.1.

I have not tested the library extensively but so far everything works as expected. It should be fairly simple to add a DaisySP audio effect object (ie one that has inputs and outputs) and one that generates stereo audio out for the DaisySP functions generate stereo audio. I have not had a need for this yet so its not in the library.

Tested with Arduino 1.85 and Teensyduino 1.53.

Rich Heslip rheslip@hotmail.com




