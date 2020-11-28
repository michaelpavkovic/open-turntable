#include <cmath>
#include <iostream>
#include <string>

#include "Button.h"
#include "mbed.h"
#include "PololuLedStrip.h"
#include "QuadratureEncoder.h"
#include "RollingAverage.h"
#include "USBMIDI.h"

#define LED_COUNT 16
#define BRIGHTNESS 30
#define CLAMP(v, l, h) min(h, max(v, l))

const int TICKS_PER_REV = 600 * 4;

USBMIDI midi;

DigitalOut cueLed(p22);
DigitalOut playLed(p21);
PololuLedStrip ledStrip(D12); //Eq. P6
QuadratureEncoder encoder(p13, p14, TICKS_PER_REV);
RollingAverage tempoSliderAverage(32);
RollingAverage volumeSliderAverage(32);

MIDIMessage receivedMessage;
void onMidiMessageReceived() {
	if (midi.read(&receivedMessage)) {
		if (receivedMessage.data[1] == 0x90 && receivedMessage.data[2] == 0x0C) {
			cueLed = receivedMessage.data[3];
		} else if (receivedMessage.data[1] == 0x90 && receivedMessage.data[2] == 0x0B) {
			playLed = receivedMessage.data[3];
		} else if (receivedMessage.data[1] == 0x90 && receivedMessage.data[2] == 0x0B) {
			
		}
	}
}

void onCueButtonPressed(Button* b) {
	std::cout << "Tempo slider:  " << (int) tempoSliderAverage.getAverage() << std::endl;
	std::cout << "Volume slider: " << (int) volumeSliderAverage.getAverage() << std::endl;
	std::cout << "Encoder ticks: " << encoder.getTicks() << std::endl << std::endl;

	midi.write(MIDIMessage::NoteOn(0x0C));
}

void onPlayButtonPress(Button* b) {
	midi.write(MIDIMessage::NoteOn(0x0B));
}

float previousTicks = 0;
int previousVolume = 0;
int previousTempo = 0;

// Taken from the PololuLedStrip Library's main, credit to David E Grayson
//Converts a color from the HSV representation to RGB.
rgb_color hsvToRgb(float h, float s, float v)
{
    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r = 0, g = 0, b = 0;
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color) {
		(std::uint8_t) (r * BRIGHTNESS), 
		(std::uint8_t) (g * BRIGHTNESS), 
		(std::uint8_t) (b * BRIGHTNESS)
	};
}

Timer timer;
Timer offsetTimer;
rgb_color colors[LED_COUNT];
int neoPixelNumber = 0;
int neoPixelOffset = 0;

void updateNeoPixelRing() {
	uint32_t time = timer.read_ms();       
	for (int i = 0; i < LED_COUNT; i++)
	{
	    uint8_t phase = (time >> 4) - (i << 2);
	    if (i == neoPixelNumber) {
	        colors[neoPixelNumber] = (rgb_color){0,0,0};    
	    } else {
	        colors[i] = hsvToRgb(phase / 256.0, 1.0, 1.0);
	    }
	}

	// Send the colors to the LED strip.
	ledStrip.write(colors, LED_COUNT);

	// This currently manually updates the neoPixelNumber on a one second timer
	if (offsetTimer.read_ms() >= 225) {
	    neoPixelOffset++;
	    neoPixelNumber %= 16;
	    offsetTimer.reset();
	}
}

int main() {
	std::cout << "[INFO] Starting..." << std::endl;

	midi.attach(onMidiMessageReceived);

	timer.start();
	offsetTimer.start();

	Button* cueButton = new Button(p17, onCueButtonPressed, "CueButton");
	Button* playButton = new Button(p18, onPlayButtonPress, "PlayButton");

	Button::addManagedButton(cueButton);
	Button::addManagedButton(playButton);

	AnalogIn tempoSlider(p19);
	AnalogIn volumeSlider(p20);

    while (true) {
		float ticks = -encoder.getTicks();
		int delta = (int) (ticks - previousTicks);

		// Only send ticks delta if it is greater than 32 to limit the number of midi messages sent
		if (abs(delta) >= 32) {
			neoPixelOffset = 0; // Reset offset to stop auto movement of NeoPixels
			
			if (delta >= 1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));
			} else if (delta <= -1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));
			}

			previousTicks = ticks;
		}

		neoPixelNumber = ((int) abs(ticks) % TICKS_PER_REV / 150 + neoPixelOffset) % 16;

		Button::watchManagedButtons();

		int currentTempo = CLAMP((int) (tempoSlider.read() * 1600.0), 0, 32);
		tempoSliderAverage.add(currentTempo);
		
		int currentVolume = CLAMP((int) (volumeSlider.read() * 1600.0), 0, 32);
		volumeSliderAverage.add(currentVolume);

		// Send tempo and volume values if they have changed
		int tempo = tempoSliderAverage.getAverage();
		int volume = volumeSliderAverage.getAverage();

		if (tempo != previousTempo) {
			midi.write(MIDIMessage::ControlChange(0x19, CLAMP(tempo * 4, 0, 127)));
			previousTempo = tempo;
		}
		if (volume != previousVolume) {
			midi.write(MIDIMessage::ControlChange(0x20, CLAMP(volume * 4, 0, 127)));
			previousVolume = volume;
		}

		updateNeoPixelRing();
    }
}
