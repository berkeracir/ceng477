#ifndef __HW1__SHADER__
#define __HW1__SHADER__

#include "parser.h"

// Return positive minimum float, if both negative return 1
float positive_min(float x, float y);

// Calculate color of the ray
parser::Vec3f get_color();


#endif
