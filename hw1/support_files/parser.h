#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <iostream>

extern float global_shadow_ray_epsilon;

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;

        // Overload + and - operators for binary addition and subtraction operations
        Vec3f operator+(const Vec3f &rhs);
        Vec3f operator-(const Vec3f &rhs);

        // Overload * operator for scalar multiplication
        template <typename T>
        Vec3f operator*(const T &rhs);

        // Overload - operator for unary minus operation
        //Vec3f operator-();

        // Overload += and -= operator
        Vec3f& operator+=(const Vec3f &rhs);
        Vec3f& operator-=(const Vec3f &rhs);

        // Overload * operator
        template <typename T>
        Vec3f& operator*=(const T &rhs);

        // Overload = operator
        Vec3f& operator=(const Vec3f& rhs);
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

	size_t sphere_count;
        size_t triangle_count;
        size_t mesh_count;
        std::vector<size_t> mesh_face_count;

        std::vector<Vec3f> min_mesh_vector;
        std::vector<Vec3f> max_mesh_vector;

        //Functions
        void loadFromXml(const std::string& filepath);
    };

    // Generic print function for vector<typename T>
    // typename T must be printable with cout << operator
    template <typename T>
    void printVector(const std::vector<T> &vec);

    // Overload << operator for printing basic structures
    std::ostream& operator<<(std::ostream &o,const parser::Vec3f &vec);
    std::ostream& operator<<(std::ostream &o,const parser::Vec3i &vec);
    std::ostream& operator<<(std::ostream &o,const parser::Vec4f &vec);
    
    std::ostream& operator<<(std::ostream &o,const parser::Camera &camera);
    std::ostream& operator<<(std::ostream &o,const parser::PointLight &pointLight);
    std::ostream& operator<<(std::ostream &o,const parser::Material &material);
    std::ostream& operator<<(std::ostream &o,const parser::Face &face);
    std::ostream& operator<<(std::ostream &o,const parser::Mesh &mesh);
    std::ostream& operator<<(std::ostream &o,const parser::Triangle &triangle);
    std::ostream& operator<<(std::ostream &o,const parser::Sphere &sphere);
    std::ostream& operator<<(std::ostream &o,const parser::Scene &scene);

    // Overload + and - operators for binary addition and subtraction operations
    Vec3f operator+(const Vec3f &lhs, const Vec3f &rhs);
    Vec3f operator-(const Vec3f &lhs, const Vec3f &rhs);
    
    // Overload - operator for unary minus operation
    Vec3f operator-(const Vec3f &rhs);

    // Overload * operator for scalar multiplication
    template <typename T>
    Vec3f operator*(const T &lhs, const Vec3f &rhs);
}

#endif
