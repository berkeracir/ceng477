#include "shader.h"
#include "ray_mesh.h"
#include "ray_triangle.h"
#include "ray_sphere.h"
#include <math.h>

using namespace parser;

// Return positive minimum float, if both negative return 1
float positive_min(float x, float y) {
    if (x < 0 && y < 0)
        return 0;
    else if (x < 0 && y >= 0)
        return y;
    else if (x >= 0 && y < 0)
        return x;
    else // (x >= 0 && y >= 0)
        return std::min(x, y);
}

// Scalar Vec3f multiplication
// Vec3f {x, y, z} * Vec3f {u, v, w} = Vec3f {x*u, y*v, z*w}
Vec3f scalar_vec3f_multiplication(const Vec3f &lhs, const Vec3f &rhs) {
    return Vec3f {lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z};
}

Vec3f ambient_shading(const Vec3f &coefficient, const Vec3f &radiance) {
    return scalar_vec3f_multiplication(coefficient, radiance);
}

// Check whether the Ray is intersecting with the light source
// Returns true if the point is not in shadow, otherwise false
bool not_in_shadow(const Ray &ray, const Vec3f &position, const Scene &scene) {
    float t = vector_magnitude(position - ray.o); // Assumed ray.d is unit vector

    for (std::size_t sid = 0; sid < scene.spheres.size(); sid++) {
        Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id-1];
        float r = scene.spheres[sid].radius;
        float t_intersect = ray_sphere_intersection(ray.o, ray.d, c, r);

        if ((t_intersect >= 0) && (t_intersect < t))
            return false;
    }

    for (std::size_t tid = 0; tid < scene.triangles.size(); tid++) {
        Face indice = scene.triangles[tid].indices;
        
        Vec3f a = scene.vertex_data[indice.v0_id-1];
        Vec3f b = scene.vertex_data[indice.v1_id-1];
        Vec3f c = scene.vertex_data[indice.v2_id-1];

        float t_intersect = ray_triangle_intersection(ray.o, ray.d, a, b, c);

        if ((t_intersect >= 0) && (t_intersect < t))
            return false;
    }

    for (std::size_t meid = 0; meid < scene.meshes.size(); meid++) {
        if (is_ray_mesh_intersect(ray.o, ray.d, scene.max_mesh_vector[meid], scene.min_mesh_vector[meid])) {
            for (std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++) {
                Face face = scene.meshes[meid].faces[fid];

                Vec3f a = scene.vertex_data[face.v0_id-1];
                Vec3f b = scene.vertex_data[face.v1_id-1];
                Vec3f c = scene.vertex_data[face.v2_id-1];

                float t_intersect = ray_triangle_intersection(ray.o, ray.d, a, b, c);

                if ((t_intersect >= 0) && (t_intersect < t))
                    return false;
            }
        }
    }

    return true;
}

// Complete Ray Structure with given paramaters
Ray reflection_ray(const Vec3f &o, const Vec3f &d, const Scene &scene) {
    Ray rr;
    rr.o = o;
    rr.d = d;
    rr.t = INFINITY;
    //float t = INFINITY;

    for (std::size_t sid = 0; sid < scene.spheres.size(); sid++) {
        Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id-1];
        float r = scene.spheres[sid].radius;
        float t_intersect = ray_sphere_intersection(o, d, c, r);

        //if ((t_intersect >= 0) && ((rr.t < 0) || (t_intersect < rr.t))) {
        if ((t_intersect >= 0) && (t_intersect < rr.t)) {
            rr.t = t_intersect;
            rr.mid = scene.spheres[sid].material_id;
            Vec3f n = rr.o + rr.t * rr.d - c;
            rr.n = vector_normalize(n);
        }
    }

    for (std::size_t tid = 0; tid < scene.triangles.size(); tid++) {
        Face indice = scene.triangles[tid].indices;
        
        Vec3f a = scene.vertex_data[indice.v0_id-1];
        Vec3f b = scene.vertex_data[indice.v1_id-1];
        Vec3f c = scene.vertex_data[indice.v2_id-1];

        float t_intersect = ray_triangle_intersection(o, d, a, b, c);

        //if ((t_intersect >= 0) && ((rr.t < 0) || (t_intersect < rr.t))) {
        if ((t_intersect >= 0) && (t_intersect < rr.t)) {
            rr.t = t_intersect;
            rr.mid = scene.triangles[tid].material_id;
            Vec3f n = vector_cross(b-a, c-b);
            rr.n = vector_normalize(n);
        }
    }

    for (std::size_t meid = 0; meid < scene.meshes.size(); meid++) {
        if (is_ray_mesh_intersect(o, d, scene.max_mesh_vector[meid], scene.min_mesh_vector[meid])) {
            for (std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++) {
                Face face = scene.meshes[meid].faces[fid];

                Vec3f a = scene.vertex_data[face.v0_id-1];
                Vec3f b = scene.vertex_data[face.v1_id-1];
                Vec3f c = scene.vertex_data[face.v2_id-1];

                float t_intersect = ray_triangle_intersection(o, d, a, b, c);

                //if ((t_intersect >= 0) && ((rr.t < 0) || (t_intersect < rr.t))) {
                if ((t_intersect >= 0) && (t_intersect < rr.t)) {
                    rr.t = t_intersect;
                    rr.mid = scene.meshes[meid].material_id;
                    Vec3f n = vector_cross(b-a, c-b);
                    rr.n = vector_normalize(n);
                }
            }
        }
    }

    return rr;
}

Vec3f diffuse_shading(const Ray &ray, const Vec3f &coefficient, const Scene &scene) {
    Vec3f result {0, 0, 0};
    Vec3f x = ray.o + ray.t * ray.d;
    
    for (size_t lid = 0; lid < scene.point_lights.size(); lid++) {
        PointLight pointLight = scene.point_lights[lid];

        Vec3f wi = pointLight.position - x;
        float r = vector_magnitude(wi);
        wi *= (1.0 / r);

        Ray shadow_ray;
        shadow_ray.o = x + wi * scene.shadow_ray_epsilon;
        shadow_ray.d = wi;

        // Check if the point is in shadow
        if (not_in_shadow(shadow_ray, pointLight.position, scene)) {
            float cos_theta = std::max((float) 0, vector_dot(wi, ray.n));

            result += (cos_theta / (r * r)) * scalar_vec3f_multiplication(coefficient, pointLight.intensity);
        }
    }

    return result;
}

Vec3f specular_shading(const Ray &ray, const Vec3f &coefficient, const float phong, const Scene &scene) {
    Vec3f result {0, 0, 0};
    Vec3f x = ray.o + ray.t * ray.d;
    Vec3f wo = vector_normalize(-ray.d);
    
    for (size_t lid = 0; lid < scene.point_lights.size(); lid++) {
        PointLight pointLight = scene.point_lights[lid];
        // Check if the point is in shadow

        Vec3f wi = (pointLight.position - x);
        float r = vector_magnitude(wi);
        wi *= (1.0 / r);

        Ray shadow_ray;
        shadow_ray.o = x + wi * scene.shadow_ray_epsilon;
        shadow_ray.d = wi;

        // Check if the point is in shadow
        if (not_in_shadow(shadow_ray, pointLight.position, scene)) {
            Vec3f h = (wi + wo) * (1.0 / vector_magnitude(wi+wo));
            float cos_alpha = std::max((float) 0, vector_dot(ray.n, h));

            result += (pow(cos_alpha, phong) / (r*r)) * scalar_vec3f_multiplication(coefficient, pointLight.intensity);
        }
    }

    return result;
}

Vec3f specular_reflection(const Ray &ray, const Vec3f &coefficient, const Scene &scene, int rec_depth) {
    if (!coefficient.x && !coefficient.y && !coefficient.z)
        return Vec3f {0, 0, 0};
    else if (rec_depth > scene.max_recursion_depth) // TODO
        return Vec3f {0, 0, 0};
    else {
        Vec3f wo = vector_normalize(-ray.d);
        Vec3f wr = -wo + 2 * ray.n * vector_dot(ray.n, wo);
        Vec3f x = ray.o + ray.t * ray.d + wr * scene.shadow_ray_epsilon;

        Ray rr = reflection_ray(x, wr, scene);

        return scalar_vec3f_multiplication(coefficient, get_color(rr, scene, rec_depth));
    }
}

Vec3f get_color(const Ray &ray, const Scene &scene, int rec_depth) {
    if ((ray.t == INFINITY) || (ray.t < 0)) {
        return Vec3f {(float) scene.background_color.x, (float) scene.background_color.y, (float) scene.background_color.z};
    }
    else {
        Material material = scene.materials[ray.mid-1];
        return ambient_shading(material.ambient, scene.ambient_light)
               + diffuse_shading(ray, material.diffuse, scene)
               + specular_shading(ray, material.specular, material.phong_exponent, scene)
               + specular_reflection(ray, material.mirror, scene, rec_depth+1);
    }
}

float clamp(float channel) {
    if (channel > 255)
        return 255;
    else
        return channel;
}

// Clamp the Vec3f into color Vec3i
Vec3f color_clamp(const Ray &ray, const Scene &scene) {
    if (ray.t < 0) {
        return Vec3f {(float) scene.background_color.x, (float) scene.background_color.y, (float) scene.background_color.z};
    }
    else {
        Vec3f color = get_color(ray, scene, 0);
        return Vec3f {clamp(color.x), clamp(color.y), clamp(color.z)};
    }
}