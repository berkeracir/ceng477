#include "ray_trace.h"

#include <math.h>

float vector_magnitude(const parser::Vec3f &vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

parser::Vec3f vector_add(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

parser::Vec3f vector_sub(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

template <typename T>
parser::Vec3f vector_scalar(const T &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {(float) lhs * rhs.x, (float) lhs * rhs.y, (float) lhs * rhs.z};
}
template <typename T>
parser::Vec3f vector_scalar(const parser::Vec3f &lhs, const T &rhs) {
    return parser::Vec3f {lhs.x * (float) rhs, lhs.y * (float) rhs, lhs.z * (float) rhs};
}

template parser::Vec3f vector_scalar<int>(const int &, const parser::Vec3f &);
template parser::Vec3f vector_scalar<float>(const float &, const parser::Vec3f &);
template parser::Vec3f vector_scalar<double>(const double &, const parser::Vec3f &);

template parser::Vec3f vector_scalar<int>(const parser::Vec3f &, const int &);
template parser::Vec3f vector_scalar<float>(const parser::Vec3f &, const float &);
template parser::Vec3f vector_scalar<double>(const parser::Vec3f &, const double &);

float vector_dot(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

parser::Vec3f vector_cross(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {lhs.y*rhs.z-lhs.z*rhs.y, lhs.z*rhs.x-lhs.x*rhs.z, lhs.x*rhs.y-lhs.y*rhs.x};
}

bool is_ray_sphere_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r) {
    float A = vector_dot(d, d);
    float B = 2*vector_dot(d, o-c);
    float C = vector_dot(o-c, o-c) - r*r;

    if (B*B-4*A*C >= 0) {
        return 1;
    }
    else
        return 0;
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