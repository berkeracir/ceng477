#include "support_files/parser.h"
#include "support_files/ppm.h"
#include "support_files/ray.h"
#include "support_files/ray_sphere.h"
#include "support_files/ray_triangle.h"
#include "support_files/ray_mesh.h"
#include "support_files/shader.h"
#include <iostream>
#include <string>
#include <cmath>

typedef unsigned char RGB[3];

using namespace parser;

float global_shadow_ray_epsilon;

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    Scene scene;

    scene.loadFromXml(argv[1]);

    for(std::size_t cid = 0; cid < scene.cameras.size(); cid++){
        Camera camera = scene.cameras[cid];



        int nx = camera.image_width;
        int ny = camera.image_height;

        float l = camera.near_plane.x; // Left edge of image plane
        float r = camera.near_plane.y; // Right edge of image plane
        float b = camera.near_plane.z; // Bottom edge of image plane
        float t = camera.near_plane.w; // Top edge of image plane

        float d = camera.near_distance;
        float pw = (r-l)/(float) nx; // Pixel width of each pixel
        float ph = (t-b)/(float) ny; // Pixel height of each pixel

        Vec3f e = camera.position; // Camera position
        Vec3f v = vector_normalize(camera.up); // Camera Up vector (v)
        Vec3f w = vector_normalize(-camera.gaze); // Camera Opposite Gaze vector (w)
        Vec3f u = vector_normalize(vector_cross(v, w)); // Camera Right vector (u)
        Vec3f m = (-w) * d; // Vector to middle of image plane from camera
        Vec3f q = m + l*u + t*v; // Vector to top left corner of image plane from camera
        unsigned char* image = new unsigned char [nx * ny * 3];
        int i_img = 0;
        for(int row = 0; row < ny; row++){
            for(int col = 0; col < nx; col++){
                Ray ray;
                ray.o = e;
                ray.t = -1;
                ray.d = q + (col + 0.5) * pw * u + (row + 0.5) * ph * (-v);
                //Iterate spheres
                for(std::size_t sid = 0; sid < scene.spheres.size(); sid++){
                    Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id - 1];
                    float radius = scene.spheres[sid].radius;

                    unsigned int index = 0;

                    //Iterate image plane 
                    float t = ray_sphere_intersection(ray.o, ray.d, c, radius);

                    if((t > 0) && (ray.t == -1 || t == positive_min(t, ray.t))){ //TODOOOOO TODOOOOO TODOOOO
                        ray.t = t;
                        ray.mid = scene.spheres[sid].material_id;
                        ray.n = vector_normalize(ray.o + ray.t * ray.d - c);
                    }

                }

                //Iterate triangle
                for(std::size_t tid = 0; tid < scene.triangles.size(); tid++){
                    Face indice = scene.triangles[tid].indices;

                    Vec3f a = scene.vertex_data[indice.v0_id-1];
                    Vec3f b = scene.vertex_data[indice.v1_id-1];
                    Vec3f c = scene.vertex_data[indice.v2_id-1];

                    t = ray_triangle_intersection(ray.o, ray.d, a, b, c);

                    if((t > 0) && (ray.t == 1 || t == positive_min(t, ray.t))){
                        ray.t = t;
                        ray.mid = scene.triangles[tid].material_id;
                        Vec3f n = vector_normalize(vector_cross(b - a, c - b));


                    }



                }

                //Iterate meshes
                for(std::size_t meid = 0; meid < scene.meshes.size(); meid++){

                    for(std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++){
                        Face face = scene.meshes[meid].faces[fid];

                        Vec3f a = scene.vertex_data[face.v0_id-1];
                        Vec3f b = scene.vertex_data[face.v1_id-1];
                        Vec3f c = scene.vertex_data[face.v2_id-1];
                        t = ray_triangle_intersection(ray.o, ray.d, a, b, c);

                        if((t > 0) && (ray.t == -1 || t == positive_min(t, ray.t))){
                            ray.t = t;
                            ray.mid = scene.meshes[meid].material_id;
                            ray.n = vector_normalize(vector_cross(b -a, c - b));
                        }
                        

                    }
                }


                Vec3f color = {0, 0, 0};
                image[i_img++] = (unsigned char) round(color.x);
                image[i_img++] = (unsigned char) round(color.y);
                image[i_img++] = (unsigned char) round(color.z);

                







            }
        }
        write_ppm(camera.image_name.c_str(), image, nx, ny);
        delete[] image;
    }


}
