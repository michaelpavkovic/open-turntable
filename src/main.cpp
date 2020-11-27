#include <cmath>
#include <iostream>
#include <string>

#include "Button.h"
#include "mbed.h"
// #include "PololuLedStrip.h"
#include "QuadratureEncoder.h"
#include "RollingAverage.h"
#include "USBMIDI.h"

#define LED_COUNT 16
#define BRIGHTNESS 30

const int TICKS_PER_REV = 600 * 4;

USBMIDI midi;

DigitalOut cueLed(p22);
DigitalOut playLed(p21);
// PololuLedStrip ledStrip(D12); //Eq. P6
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
		}
	}
}

void onCueButtonPressed(Button* b) {
	std::cout << "Tempo slider:  " << (int) tempoSliderAverage.getAverage() << std::endl;
	std::cout << "Volume slider: " << (int) volumeSliderAverage.getAverage() << std::endl;
	std::cout << "Encoder ticks: " << encoder.getTicks() << std::endl << std::endl;

	midi.write(MIDIMessage::NoteOn(0x0C));
}

void onCueButtonReleased(Button* b) {
	// midi.write(MIDIMessage::NoteOn(0x0C));
}

void onPlayButtonPress(Button* b) {
	// playLed = !playLed;

	midi.write(MIDIMessage::NoteOn(0x0B));
}

float previousTicks = 0;
int previousVolume = 0;
int previousTempo = 0;

// Taken from the PololuLedStrip Library's main, credit to David E Grayson
//Converts a color from the HSV representation to RGB.
// rgb_color hsvToRgb(float h, float s, float v)
// {
//     int i = floor(h * 6);
//     float f = h * 6 - i;
//     float p = v * (1 - s);
//     float q = v * (1 - f * s);
//     float t = v * (1 - (1 - f) * s);
//     float r = 0, g = 0, b = 0;
//     switch(i % 6){
//         case 0: r = v; g = t; b = p; break;
//         case 1: r = q; g = v; b = p; break;
//         case 2: r = p; g = v; b = t; break;
//         case 3: r = p; g = q; b = v; break;
//         case 4: r = t; g = p; b = v; break;
//         case 5: r = v; g = p; b = q; break;
//     }
//     return (rgb_color){r * BRIGHTNESS, g * BRIGHTNESS, b * BRIGHTNESS};
// }

int main() {
	// Timer timer;
	
	// Timer timer2;
	
	// timer.start();
	// timer2.start();
	
	// int lednum = 0;
	// rgb_color colors[LED_COUNT];

	std::cout << "[INFO] Starting..." << std::endl;

	midi.attach(onMidiMessageReceived);

	Button* cueButton = new Button(p17, onCueButtonPressed, "CueButton");
	cueButton->setOnButtonReleased(onCueButtonReleased);

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
			if (delta >= 1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));
			} else if (delta <= -1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));
			}

			previousTicks = ticks;
		}

		Button::watchManagedButtons();

		tempoSliderAverage.add(min((int) (tempoSlider.read() * 1000000.0), 4));
		volumeSliderAverage.add(min((int) (volumeSlider.read() * 1000000.0), 4));

		// Send tempo and volume values if they have changed
		int tempo = tempoSliderAverage.getAverage();
		int volume = volumeSliderAverage.getAverage();

		if (tempo != previousTempo) {
			midi.write(MIDIMessage::ControlChange(0x19, tempo * 32));
			previousTempo = tempo;
		}
		if (volume != previousVolume) {
			midi.write(MIDIMessage::ControlChange(0x20, volume * 32));
			previousVolume = volume;
		}

		// uint32_t time = timer.read_ms();       
        // for(int i = 0; i < LED_COUNT; i++)
        // {
        //     uint8_t phase = (time >> 4) - (i << 2);
        //     if (i == lednum) {
        //         colors[lednum] = (rgb_color){0,0,0};    
        //     }
        //     else {
        //         colors[i] = hsvToRgb(phase / 256.0, 1.0, 1.0);
        //     }
        // }

		// // Send the colors to the LED strip.
        // ledStrip.write(colors, LED_COUNT);

		// //This currently manually updates the lednum on a one second timer
        // if (timer2.read() >= 1) {
        //     lednum++;
        //     lednum = lednum%16;
        //     timer2.reset();
        // }
    }
}
