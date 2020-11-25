#include <iostream>
#include <vector>

#include "Button.h"
#include "mbed.h"
#include "rtos.h"

void Button::watchManagedButtons() {
    for (auto button : Button::_managedButtons) {
        if (button->read()) {
            if (!button->_wasPressed) {
                button->_wasPressed = true;
                std::cout << "[INFO] Button pressed: " << button->getId() << std::endl;
                button->_onButtonPressed(button);
            }
        } else if (button->_wasPressed) {
            std::cout << "[INFO] Button released: " << button->getId() << std::endl;
            if (button->_onButtonReleased != nullptr) {
                button->_onButtonReleased(button);
            }

            button->_wasPressed = false;
        }
    }

    wait_us(1000);
}

void Button::setOnButtonPressed(Callback<void(Button*)> callback) {
    this->_onButtonPressed = callback;
}

void Button::setOnButtonReleased(Callback<void(Button*)> callback) {
    this->_onButtonReleased = callback;
}

void Button::addManagedButton(Button* b) {
    Button::_managedButtons.push_back(b);
}

bool Button::read() {
    return this->_mbedDigitalIn == 0;
}
