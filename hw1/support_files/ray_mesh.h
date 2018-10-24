#ifndef __HW1__RAY_MESH__
#define __HW1__RAY_MESH__

#include "parser.h"
#include "ray.h"

// max_vector({x,y,z}, {u,v,w}) = {max(x,u), max(y,v), max(z,w)}
parser::Vec3f max_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs);

// min_vector({x,y,z}, {u,v,w}) = {min(x,u), min(y,v), min(z,w)}
parser::Vec3f min_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs);


#endif
