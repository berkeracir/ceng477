#ifndef __HW1__RAY_TRACE__
#define __HW1__RAY_TRACE__

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

// Check whether ray and sphere intersects
// Inputs: Vec3f o, Vec3f d, Vec3f c, float r
//      r(t): o + t.d (o: origin, d: vector)
//      Sphere: (P-c)*(P-c) = r*r (P: point on sphere, c: coordinate of center, r: radius)
//      Solve the equation for t: (Note that . is dot product)
//          (d.d)*t*t + 2*d.(o-c)*t + (o-c).(o-c) - r*r = 0
// Output: 1 (true) if ray and sphere intersects (B*B-4*A*C >= 0), 0 (false) otherwise
//      A = d.d, B = 2*d.(o-c) and C = (o-c).(o-c) - r*r
//      t1 = (-B + sqrt(B*B-4*A*C))/(2*A)
//      t2 = (-B - sqrt(B*B-4*A*C))/(2*A)
bool is_ray_sphere_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r);

// t value for Ray and Sphere Intersection
// Inputs: Vec3f o, Vec3f d, Vec3f c, float r
//      r(t): o + t.d (o: origin, d: vector)
//      Sphere: (P-c)*(P-c) = r*r (P: point on sphere, c: coordinate of center, r: radius)
//      Solve the equation for t: (Note that . is dot product)
//          (d.d)*t*t + 2*d.(o-c)*t + (o-c).(o-c) - r*r = 0
// Output: Return the smallest positive t value, if ray and sphere don't intersect, return -1
//      A = d.d, B = 2*d.(o-c) and C = (o-c).(o-c) - r*r
//      t1 = (-B + sqrt(B*B-4*A*C))/(2*A)
//      t2 = (-B - sqrt(B*B-4*A*C))/(2*A)
float ray_sphere_intersection(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r);


#endif
