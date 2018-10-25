#include "shader.h"

// Return positive minimum float, if both negative return 1
float positive_min(float x, float y) {
    if (x < 0 && y < 0)
        return 0;
    else if (x < 0 && y >= 0)
        return y;
    else if (x >= 0 && y < 0)
        return x;
    else // (x >= 0 && y >= 0)
        return std::min(x, y);
}