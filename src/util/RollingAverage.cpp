#include "RollingAverage.h"

RollingAverage::RollingAverage(const int size) : _size(size) {

}

float RollingAverage::getAverage() {
    float average = 0.0;
    for (auto v : _storage) {
        average += v;
    }

    average /= _storage.size();
    return average;
}

void RollingAverage::add(const float value) {
    if (_storage.size() == _size) {
        _storage.erase(_storage.begin());
    }

    _storage.push_back(value);
}
