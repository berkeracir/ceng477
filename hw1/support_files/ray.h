#ifndef __HW1__RAY__
#define __HW1__RAY__

#include "parser.h"
#include <vector>

// Vector (parser::Vec3f) magnitude
// |(x,y,z)| = sqrt(x*x+y*y+z*z)
float vector_magnitude(const parser::Vec3f &vec);

// Vector (parser::Vec3f) addition
// (x,y,z) + (u,v,w) = (x+y, y+v, z+w)
parser::Vec3f vector_add(const parser::Vec3f &rhs, const parser::Vec3f &lhs);

// Vector (parser::Vec3f) subtracktion
// (x,y,z) - (u,v,w) = (x-y, y-v, z-w)
parser::Vec3f vector_sub(const parser::Vec3f &rhs, const parser::Vec3f &lhs);

// Vector (parser::Vec3f) multiplication by scalar (int, float)
// a * (x,y,z) = (a*x, a*y, a*z)
// (x,y,z) * a = (a*x, a*y, a*z)
template <typename T>
parser::Vec3f vector_scalar(const T &lhs, const parser::Vec3f &rhs);
template <typename T>
parser::Vec3f vector_scalar(const parser::Vec3f &lhs, const T &rhs);

// Vector (parser::Vec3f) dot product
// (x,y,z) . (u,v,w) = x*u + y*v + z*w
float vector_dot(const parser::Vec3f &lhs, const parser::Vec3f &rhs);

// Vector (parser::Vec3f) cross product
// (x,y,z) x (u,v,w) = (y*w-z*v, z*u-x*w, x*v-y*u)
parser::Vec3f vector_cross(const parser::Vec3f &lhs, const parser::Vec3f &rhs);


#endif
