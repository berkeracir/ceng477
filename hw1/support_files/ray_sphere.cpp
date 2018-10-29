#include "ray_sphere.h"
#include <math.h>

bool is_ray_sphere_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r) {
    float A = vector_dot(d, d);
    float B = 2*vector_dot(d, o-c);
    float C = vector_dot(o-c, o-c) - r*r;
    float delta = B*B-4*A*C;

    if (delta >= 0) {
        float t1 = (-B + sqrt(delta))/(2*A);
        float t2 = (-B - sqrt(delta))/(2*A);

        float result = std::min(std::max((float) 0, t1), std::max((float)0, t2));

        if (result >= 0)
            return true;
        else
            return false;
    }
    else
        return false;
}

float ray_sphere_intersection(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r) {
    float A = vector_dot(d, d);
    float B = 2*vector_dot(d, o-c);
    float C = vector_dot(o-c, o-c) - r*r;
    float delta = B*B-4*A*C;

    if (delta >= 0) {
        float t1 = (-B + sqrt(delta))/(2*A);
        float t2 = (-B - sqrt(delta))/(2*A);

        float result = std::min(std::max((float) 0, t1), std::max((float)0, t2));

        if (result > 0)
            return result;
        else
            return -1;
    }
    else
        return -1;
}
