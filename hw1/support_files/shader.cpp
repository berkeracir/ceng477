#include "shader.h"
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

Vec3f diffuse_shading(const Ray &ray, const Vec3f &coefficient, const Scene &scene) {
    Vec3f result {0, 0, 0};
    Vec3f x = ray.o + ray.t * ray.d;
    //Vec3f wo = -ray.d*(1/vector_magnitude(ray.d));
    
    for (size_t lid = 0; lid < scene.point_lights.size(); lid++) {
        PointLight pointLight = scene.point_lights[lid];
        // Check if the point is in shadow

        Vec3f wi = pointLight.position - x;
        float r = vector_magnitude(wi);
        wi *= (1.0 / r);
        float cos_theta = std::max((float) 0, vector_dot(wi, ray.n));

        result += (cos_theta / (r * r)) * scalar_vec3f_multiplication(coefficient, pointLight.intensity);
    }

    return result;
}

Vec3f specular_shading(const Ray &ray, const Vec3f &coefficient, const float phong, const Scene &scene) {
    Vec3f result {0, 0, 0};
    Vec3f x = ray.o + ray.t * ray.d;
    Vec3f wo = -ray.d * (1.0 / vector_magnitude(ray.d));
    
    for (size_t lid = 0; lid < scene.point_lights.size(); lid++) {
        PointLight pointLight = scene.point_lights[lid];
        // Check if the point is in shadow

        Vec3f wi = (pointLight.position - x);
        float r = vector_magnitude(wi);
        wi *= (1.0 / r);
        Vec3f h = (wi + wo) * (1.0 / vector_magnitude(wi+wo));
        float cos_alpha = std::max((float) 0, vector_dot(ray.n, h));

        result += (pow(cos_alpha, phong) / (r*r)) * scalar_vec3f_multiplication(coefficient, pointLight.intensity);
    }

    return result;
}
Vec3f specular_reflection();

Vec3f get_color(const Ray &ray, const Scene &scene, int rec_depth) {
    if (ray.t < 1) {
        return Vec3f {(float) scene.background_color.x, (float) scene.background_color.y, (float) scene.background_color.z};
    }
    else {
        Material material = scene.materials[ray.mid-1];
        return ambient_shading(material.ambient, scene.ambient_light)
               + diffuse_shading(ray, material.diffuse, scene)
               + specular_shading(ray, material.specular, material.phong_exponent, scene);
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
    if (ray.t < 1) {
        return Vec3f {(float) scene.background_color.x, (float) scene.background_color.y, (float) scene.background_color.z};
    }
    else {
        Vec3f color = get_color(ray, scene, 0);
        return Vec3f {clamp(color.x), clamp(color.y), clamp(color.z)};
    }
}