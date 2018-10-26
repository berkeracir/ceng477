#include "parser.h"
#include "tinyxml2.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

void parser::Scene::loadFromXml(const std::string& filepath) {
    tinyxml2::XMLDocument file;
    std::stringstream stream;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> background_color.x >> background_color.y >> background_color.z;

    //Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        stream >> camera.near_plane.x >> camera.near_plane.y >> camera.near_plane.z >> camera.near_plane.w;
        stream >> camera.near_distance;
        stream >> camera.image_width >> camera.image_height;
        stream >> camera.image_name;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

    //Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
    element = element->FirstChildElement("PointLight");
    PointLight point_light;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
        stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("MirrorReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phong_exponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    //Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertex_data.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.material_id;

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0_id).eof())
        {
            stream >> face.v1_id >> face.v2_id;
            mesh.faces.push_back(face);
        }
        stream.clear();

        meshes.push_back(mesh);
        mesh.faces.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    Triangle triangle;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> triangle.material_id;

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

        triangles.push_back(triangle);
        element = element->NextSiblingElement("Triangle");
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere sphere;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> sphere.material_id;

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> sphere.center_vertex_id;

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}

template <typename T>
void parser::printVector(const std::vector<T> &vec) {
    for (auto it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << " ";
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Vec3f &vec) {
    o << "(" << vec.x << ", "  << vec.y << ", "  << vec.z << ")";
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Vec3i &vec) {
    o << "(" << vec.x << ", "  << vec.y << ", "  << vec.z << ")";
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Vec4f &vec) {
    o << "(" << vec.x << ", "  << vec.y << ", "  << vec.z << ", " << vec.w << ")";
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Camera &camera) {
    o << "  Camera" << std::endl
      << "    Position: " << camera.position << std::endl
      << "    Gaze: " << camera.gaze << std::endl
      << "    Up:" << camera.up << std::endl
      << "    Near Plane: " << camera.near_plane << std::endl
      << "    Near Distance: " << camera.near_distance << std::endl
      << "    Resolution: " << camera.image_width << "x" << camera.image_height << std::endl
      << "    Image Name: " << camera.image_name << std::endl;
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::PointLight &pointLight) {
    o << "  Point Light" << std::endl
      << "    Position: " << pointLight.position << std::endl
      << "    Intensity: " << pointLight.intensity << std::endl;
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Material &material) {
    o << "  Material" << std::endl
      << "    Ambient: " << material.ambient << std::endl
      << "    Diffuse: " << material.diffuse << std::endl
      << "    Specular: " << material.specular << std::endl
      << "    Mirror: " << material.mirror << std::endl
      << "    Phong Exponent:" << material.phong_exponent << std::endl;
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Face &face) {
    o << "F(" << face.v0_id << ", " << face.v1_id << ", " << face.v2_id << ")";
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Mesh &mesh) {
    o << "  Mesh" << std::endl
      << "    Material id: " << mesh.material_id << std::endl
      << "    Faces: ";
      
    printVector(mesh.faces);
    
    o << std::endl;
    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Triangle &triangle) {
    o << "  Triangle" << std::endl
      << "    Material id: " << triangle.material_id << std::endl
      << "    Face:" << triangle.indices << std::endl;

    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Sphere &sphere) {
    o << "  Sphere" << std::endl
      << "    Material id: " << sphere.material_id << std::endl
      << "    Center Vertex id:" << sphere.center_vertex_id << std::endl
      << "    Radius: " << sphere.radius << std::endl;

    return o;
}

std::ostream& parser::operator<<(std::ostream &o,const parser::Scene &scene) {
    o << "Scene" << std::endl
      << "  Background Color: " << scene.background_color << std::endl
      << "  Shadow Ray Epsilon: " << scene.shadow_ray_epsilon << std::endl
      << "  Max Recursion Depth: " << scene.max_recursion_depth << std::endl << std::endl;

    o << "Cameras:" << std::endl;
    parser::printVector(scene.cameras);

    o << "\nAmbient Light: " << scene.ambient_light << std::endl;
    
    o << "\nPoint Lights:" << std::endl;
    parser::printVector(scene.point_lights);
    
    o << "\nMaterials:" << std::endl;
    parser::printVector(scene.materials);

    o << "\nVertex Data: ";
    parser::printVector(scene.vertex_data);
    o << std::endl;

    o << "\nMeshes:" << std::endl;
    parser::printVector(scene.meshes);

    o << "\nTriangles:" << std::endl;
    parser::printVector(scene.triangles);
    
    o << "\nSpheres:" << std::endl;
    parser::printVector(scene.spheres);

    return o;
}

parser::Vec3f parser::Vec3f::operator+(const parser::Vec3f &rhs) {
   return parser::Vec3f {this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
}

parser::Vec3f parser::Vec3f::operator-(const parser::Vec3f &rhs) {
    return parser::Vec3f {this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
}

template <typename T>
parser::Vec3f parser::Vec3f::operator*(const T &rhs) {
    return parser::Vec3f {this->x * (float) rhs, this->y * (float) rhs, this->z * (float) rhs};
}
template parser::Vec3f parser::Vec3f::operator*<int>(const int &);
template parser::Vec3f parser::Vec3f::operator*<float>(const float &);
template parser::Vec3f parser::Vec3f::operator*<double>(const double &);

template <typename T>
parser::Vec3f parser::operator*(const T &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {rhs.x * (float) lhs, rhs.y * (float) lhs, rhs.z * (float) lhs};
}
template parser::Vec3f parser::operator*<int>(const int &, const parser::Vec3f &);
template parser::Vec3f parser::operator*<float>(const float &, const parser::Vec3f &);
template parser::Vec3f parser::operator*<double>(const double &, const parser::Vec3f &);

/*parser::Vec3f parser::Vec3f::operator-() {
    return parser::Vec3f {-this->x, -this->y, -this->z};
}*/

parser::Vec3f& parser::Vec3f::operator+=(const Vec3f &rhs) {
    this->x = this->x + rhs.x;
    this->y = this->y + rhs.y;
    this->z = this->z + rhs.z;
    return *this;
}
parser::Vec3f& parser::Vec3f::operator-=(const Vec3f &rhs) {
    this->x = this->x - rhs.x;
    this->y = this->y - rhs.y;
    this->z = this->z - rhs.z;
    return *this;
}

parser::Vec3f parser::operator+(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}
parser::Vec3f parser::operator-(const parser::Vec3f &lhs, const parser::Vec3f &rhs) {
    return parser::Vec3f {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}
// Overload - operator for unary minus operation
parser::Vec3f parser::operator-(const parser::Vec3f &rhs) {
    return parser::Vec3f {-rhs.x, -rhs.y, -rhs.z};
}

template <typename T>
parser::Vec3f& parser::Vec3f::operator*=(const T &rhs) {
    this->x = this->x * (float) rhs;
    this->y = this->y * (float) rhs;
    this->z = this->z * (float) rhs;
    return *this;
}
template parser::Vec3f& parser::Vec3f::operator*=<int>(const int &);
template parser::Vec3f& parser::Vec3f::operator*=<float>(const float &);
template parser::Vec3f& parser::Vec3f::operator*=<double>(const double &);

parser::Vec3f& parser::Vec3f::operator=(const parser::Vec3f& rhs) {
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return *this;
}