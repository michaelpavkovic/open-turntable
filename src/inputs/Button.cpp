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
                button->_callback(button);
            }
        } else if (button->_wasPressed) {
            std::cout << "[INFO] Button released: " << button->getId() << std::endl;
            button->_wasPressed = false;
        }
    }

    wait_us(1000);
}

void Button::addManagedButton(Button* b) {
    Button::_managedButtons.push_back(b);
}

bool Button::read() {
    return this->_mbedDigitalIn == 0;
}
