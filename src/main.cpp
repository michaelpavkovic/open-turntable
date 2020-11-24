#include <iostream>
#include <string>

#include "Button.h"
#include "mbed.h"
#include "neopixel.h"
#include "QuadratureEncoder.h"
#include "RollingAverage.h"
#include "USBMIDI.h"

const int TICKS_PER_REV = 600 * 4;

// USBMIDI midi;

DigitalOut cueLed(p22);
DigitalOut playLed(p21);
QuadratureEncoder encoder(p13, p14, TICKS_PER_REV);
RollingAverage tempoSliderAverage(32);
RollingAverage volumeSliderAverage(32);

void onCueButtonPress(Button* b) {
	cueLed = !cueLed;

	std::cout << "Tempo slider:  " << (int) tempoSliderAverage.getAverage() << std::endl;
	std::cout << "Volume slider: " << (int) volumeSliderAverage.getAverage() << std::endl;
	std::cout << "Encoder ticks: " << encoder.getTicks() << std::endl << std::endl;
}

void onPlayButtonPress(Button* b) {
	playLed = !playLed;
}

int main() {
	std::cout << "[INFO] Starting..." << std::endl;

	Button* cueButton = new Button(p17, onCueButtonPress, "CueButton");
	Button* playButton = new Button(p18, onPlayButtonPress, "PlayButton");

	Button::addManagedButton(cueButton);
	Button::addManagedButton(playButton);

	AnalogIn tempoSlider(p19);
	AnalogIn volumeSlider(p20);

    while (true) {
		Button::watchManagedButtons();
		tempoSliderAverage.add((int) (tempoSlider.read() * 13000.0 - 10.0));
		volumeSliderAverage.add((int) (volumeSlider.read() * 13000.0 - 10.0));
    }    
}
