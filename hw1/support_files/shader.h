#ifndef __HW1__SHADER__
#define __HW1__SHADER__

#include "parser.h"
#include "ray.h"

// Return positive minimum float, if both negative return 1
float positive_min(float x, float y);

// Calculate color of the ray
// Implement Ambient, Diffuse, Specular Shading and Specular Reflection
parser::Vec3f get_color(const Ray &ray, const parser::Scene &scene, int rec_depth);

// Clamp the Vec3f into color Vec3i
parser::Vec3i color_clamp(const Ray &ray, const parser::Scene &scene);


#endif
