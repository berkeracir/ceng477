#ifndef __HW1__RAY_SPHERE__
#define __HW1__RAY_SPHERE__

#include "parser.h"
#include "ray.h"

// Check whether ray and sphere intersects
// Inputs: Vec3f o, Vec3f d, Vec3f c, float r
//      r(t): o + t.d (o: origin, d: vector)
//      Sphere: (P-c)*(P-c) = r*r (P: point on sphere, c: coordinate of center, r: radius)
//      Solve the equation for t: (Note that . is dot product)
//          (d.d)*t*t + 2*d.(o-c)*t + (o-c).(o-c) - r*r = 0
//      A = d.d, B = 2*d.(o-c) and C = (o-c).(o-c) - r*r
//      t1 = (-B + sqrt(B*B-4*A*C))/(2*A)
//      t2 = (-B - sqrt(B*B-4*A*C))/(2*A)
// Output: 1 (true) if ray and sphere intersects (B*B-4*A*C >= 0), 0 (false) otherwise
bool is_ray_sphere_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r);

// t value for Ray and Sphere Intersection
// Inputs: Vec3f o, Vec3f d, Vec3f c, float r
//      r(t): o + t.d (o: origin, d: vector)
//      Sphere: (P-c)*(P-c) = r*r (P: point on sphere, c: coordinate of center, r: radius)
//      Solve the equation for t: (Note that . is dot product)
//          (d.d)*t*t + 2*d.(o-c)*t + (o-c).(o-c) - r*r = 0
//      A = d.d, B = 2*d.(o-c) and C = (o-c).(o-c) - r*r
//      t1 = (-B + sqrt(B*B-4*A*C))/(2*A)
//      t2 = (-B - sqrt(B*B-4*A*C))/(2*A)
// Output: Return the smallest positive t value, if ray and sphere don't intersect, return -1
float ray_sphere_intersection(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &c, const float &r);


#endif
