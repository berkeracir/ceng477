#include "ray_mesh.h"

parser::Vec3f max_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {std::max(lhs.x,rhs.x), std::max(lhs.y,rhs.y), std::max(lhs.z,rhs.z)};
}

parser::Vec3f min_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {std::min(lhs.x,rhs.x), std::min(lhs.y,rhs.y), std::min(lhs.z,rhs.z)};
}