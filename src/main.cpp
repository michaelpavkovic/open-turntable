#include <cmath>
#include <iostream>
#include <string>

#include "Button.h"
#include "mbed.h"
#include "QuadratureEncoder.h"
#include "RollingAverage.h"
#include "USBMIDI.h"

const int TICKS_PER_REV = 600 * 4;

USBMIDI midi;

DigitalOut cueLed(p22);
DigitalOut playLed(p21);
QuadratureEncoder encoder(p13, p14, TICKS_PER_REV);
RollingAverage tempoSliderAverage(32);
RollingAverage volumeSliderAverage(32);

void onMidiMessageReceived() {
	MIDIMessage receivedMessage;
	if (midi.read(&receivedMessage)) {
		// Do something with the message
	}
}

void onCueButtonPressed(Button* b) {
	cueLed = !cueLed;

	std::cout << "Tempo slider:  " << (int) tempoSliderAverage.getAverage() << std::endl;
	std::cout << "Volume slider: " << (int) volumeSliderAverage.getAverage() << std::endl;
	std::cout << "Encoder ticks: " << encoder.getTicks() << std::endl << std::endl;

	midi.write(MIDIMessage::NoteOn(0x0C));
}

void onCueButtonReleased(Button* b) {
	// midi.write(MIDIMessage::NoteOn(0x0C));
}

void onPlayButtonPress(Button* b) {
	playLed = !playLed;

	midi.write(MIDIMessage::NoteOn(0x0B));
}

float previousTicks = 0;

int main() {
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
		if (abs(delta) >= 10) {
			if (delta >= 1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));

				std::cout << delta << std::endl;
			} else if (delta <= -1) {
				midi.write(MIDIMessage::ControlChange(0x21, delta + 64));
			}

			previousTicks = ticks;
		}

		Button::watchManagedButtons();
		tempoSliderAverage.add((int) (tempoSlider.read() * 3000.0));
		volumeSliderAverage.add((int) (volumeSlider.read() * 3000.0));
    }    
}
