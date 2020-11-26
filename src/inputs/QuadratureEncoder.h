#ifndef quadratureencoder_h
#define quadratureencoder_h

#include "mbed.h"

#define STATE_LEFT(s) (s & 0b01)
#define STATE_RIGHT(s) ((s & 0b10) >> 1)
#define INVALID_STATE_CHANGE 0b11

class QuadratureEncoder {
    private:
        // Interrupts to run on rise/fall of channels A and B of the encoder
        InterruptIn _channelA;
        InterruptIn _channelB;

        // Ticks per revolution of the encoder
        int _ticksPerRev;

        // Encoder state: current, previous, and ticks tracked
        volatile int _previousState;
        volatile int _currentState;
        volatile int _ticks;

        // Callback for tick change
        Callback<void(int)> _onTicksChanged;

        void processStateChange();

    public:
        QuadratureEncoder(const PinName channelA, const PinName channelB, const int ticksPerRev);

        void reset();

        int getState();

        int getTicks();

        double getRevolutions();

        int getWholeRevolutions();

        void setOnTicksChanged(Callback<void(int)> callback);
};

#endif
