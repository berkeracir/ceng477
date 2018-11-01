#ifndef __HW1__RAY_MESH__
#define __HW1__RAY_MESH__

#include "parser.h"
#include "ray.h"

// max_vector({x,y,z}, {u,v,w}) = {max(x,u), max(y,v), max(z,w)}
parser::Vec3f max_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs);

// min_vector({x,y,z}, {u,v,w}) = {min(x,u), min(y,v), min(z,w)}
parser::Vec3f min_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs);

// We have max_vector and min_vector of mesh
// That means we have 8 vertices (max x,y,z and min x,y,z vertices), also 16 triangles
// Check those triangles
// Return true if one of the triangles intersects with the ray
bool is_ray_mesh_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &max_vector, const parser::Vec3f &min_vector);


#endif
