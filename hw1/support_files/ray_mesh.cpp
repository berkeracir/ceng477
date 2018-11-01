#include "ray_mesh.h"

parser::Vec3f max_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {std::max(lhs.x,rhs.x), std::max(lhs.y,rhs.y), std::max(lhs.z,rhs.z)};
}

parser::Vec3f min_vector(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {std::min(lhs.x,rhs.x), std::min(lhs.y,rhs.y), std::min(lhs.z,rhs.z)};
}

bool is_ray_mesh_intersect(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &max_vector, const parser::Vec3f &min_vector) {
    float tmin = (min_vector.x - o.x) / d.x;
    float tmax = (max_vector.x - o.x) / d.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (min_vector.y - o.y) / d.y;
    float tymax = (max_vector.y - o.y) / d.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (min_vector.z - o.z) / d.z;
    float tzmax = (max_vector.z - o.z) / d.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}