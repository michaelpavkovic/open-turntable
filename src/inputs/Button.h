#ifndef button_h
#define button_h

#include <string>
#include <vector>

#include "mbed.h"
#include "rtos.h"

/// Debounce time in milliseconds
#define DEBOUNCE_DURATION 10

class Button {
    private:
        DigitalIn _mbedDigitalIn;
        Callback<void(Button*)> _callback;
        string _id;
        bool _wasPressed;

        inline static std::vector<Button*> _managedButtons = std::vector<Button*>();

    public:
        Button(PinName pin, Callback<void(Button*)> callback, string id) : _mbedDigitalIn(pin, PullUp), _callback(callback), _id(id), _wasPressed(false) {};

        DigitalIn getMbedDigitalIn() {
            return _mbedDigitalIn;
        }

        Callback<void(Button*)> getCallback() {
            return _callback;
        }

        string getId() {
            return _id;
        }

        static std::vector<Button*> getManagedButtons() {
            return Button::_managedButtons;
        }

        static void watchManagedButtons();

        static void addManagedButton(Button* b);

        bool read();
};

#endif
