#include "QuadratureEncoder.h"

#include "mbed.h"

QuadratureEncoder::QuadratureEncoder(const PinName channelA, 
    const PinName channelB, 
    const int ticksPerRev) : _channelA(channelA, PullUp), _channelB(channelB, PullUp), _ticksPerRev(ticksPerRev) {
    
    _channelA.rise(callback(this, &QuadratureEncoder::processStateChange));
    _channelA.fall(callback(this, &QuadratureEncoder::processStateChange));

    _channelB.rise(callback(this, &QuadratureEncoder::processStateChange));
    _channelB.fall(callback(this, &QuadratureEncoder::processStateChange));

    _currentState = (_channelA.read() << 1) | _channelB.read();
    _previousState = _currentState;

    _ticks = 0;
}

void QuadratureEncoder::processStateChange() {
    _currentState = (_channelA.read() << 1) | _channelB.read();

    // Invalid when no bits change or both bits change
    if ((_currentState ^ _previousState) == INVALID_STATE_CHANGE || _currentState == _previousState) {
        return;
    }

    // Read the right hand bit of the previous state xor'ed with the left hand bit of the current state
    // If it rotates clockwise (positive), gives 0
    // If it rotates counter-clockwise(negative), gives 1
    if (STATE_RIGHT(_previousState) ^ STATE_LEFT(_currentState)) {
        _ticks -= 1;

        if (this->_onTicksChanged != nullptr) {
            this->_onTicksChanged(_ticks);
        }
    } else {
        _ticks += 1;

        if (this->_onTicksChanged != nullptr) {
            this->_onTicksChanged(_ticks);
        }
    }

    _previousState = _currentState;
}

void QuadratureEncoder::reset() {
    _ticks = 0;
}

int QuadratureEncoder::getState() {
    return _currentState;
}

int QuadratureEncoder::getTicks() {
    return _ticks;
}

double QuadratureEncoder::getRevolutions() {
    return (double) _ticks / _ticksPerRev;
}

int QuadratureEncoder::getWholeRevolutions() {
    return (int) this->getRevolutions();
}

void QuadratureEncoder::setOnTicksChanged(Callback<void(int)> callback) {
    this->_onTicksChanged = callback;
}
