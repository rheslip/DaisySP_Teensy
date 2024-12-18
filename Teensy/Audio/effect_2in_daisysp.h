/* Audio Library for Teensy 3.X
 * Copyright (c) 2016, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 // RH June 28 2023 - an effect that uses the DaisySP DSP library to process audio
 // its an effect - it has two inputs. 
 // Generates one or more outputs which is determined by your AudioEffect_2in_DaisySP:update() code
 // normally AudioEffect_2in_DaisySP:update() will be in your sketch - its the equivalent of Daisylib's Audiocallback function

#ifndef effect_2in_daisysp_h_
#define effect_2in_daisysp_h_
#include "Arduino.h"
#include "AudioStream.h"

class AudioEffect_2in_DaisySP: public AudioStream
{
public:
	AudioEffect_2in_DaisySP(void) : AudioStream(2, inputQueueArray) {}
	virtual void update(void);
private:
	audio_block_t *inputQueueArray[2];
};

#endif


