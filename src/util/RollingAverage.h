#ifndef rollingaverage_h
#define rollingaverage_h

#include <vector>

#include "mbed.h"

#define DEFAULT_ROLLING_AVG_SIZE 8

class RollingAverage {
    private:
        std::vector<float> _storage;
        int _size;

    public:
        RollingAverage(const int size);

        float getAverage();

        void add(const float value);
};

#endif
