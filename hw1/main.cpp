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

int main(int argc, char* argv[]) {
    Scene scene;

    scene.loadFromXml(argv[1]);

    global_shadow_ray_epsilon = scene.shadow_ray_epsilon;

    // Iterate through cameras
    for (std::size_t cid = 0; cid < scene.cameras.size(); cid++) {    // Actual Camera ID is cid+1
        Camera camera = scene.cameras[cid];

        //std::cout << "Camera-" << cid+1 << std::endl << camera << std::endl;

        int nx = camera.image_width;
        int ny = camera.image_height;

        float l = camera.near_plane.x; // Left edge of image plane
        float r = camera.near_plane.y; // Right edge of image plane
        float b = camera.near_plane.z; // Bottom edge of image plane
        float t = camera.near_plane.w; // Top edge of image plane

        float d = camera.near_distance;

        Vec3f e = camera.position; // Camera position
        Vec3f v = vector_normalize(camera.up); // Camera Up vector (v)
        Vec3f w = vector_normalize(-camera.gaze); // Camera Opposite Gaze vector (w)
        Vec3f u = vector_normalize(vector_cross(v, w)); // Camera Right vector (u)
        Vec3f m = (-w) * d; // Vector to middle of image plane from camera
        Vec3f q = m + l*u + t*v; // Vector to top left corner of image plane from camera

        float pw = (r-l)/(float) nx; // Pixel width of each pixel
        float ph = (t-b)/(float) ny; // Pixel height of each pixel

        Ray *rays = new Ray [nx * ny];

        // Calculate Rays
        unsigned int i = 0;
        for (int row = 0; row < ny; row++) {
            for (int col = 0; col < nx; col++) {
                // Ray Form: r(t) = o + t.d
                // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                rays[i].o = e;
                rays[i].t = -1;
                rays[i].d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                i++;
            }
        }

        // Iterate through spheres
        for (std::size_t sid = 0; sid < scene.spheres.size(); sid++) {
            //std::cout << "Sphere-" << sid+1 << std::endl << scene.spheres[sid] << std::endl;
            
            Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id-1];
            float radius = scene.spheres[sid].radius;

            unsigned int index = 0;

            // Iterate through image plane (Width:nx, Height:ny)
            bool col_flag = false;
            for (int row = 0; row < ny; row++) {
                bool row_flag = false;
                for (int col = 0; col < nx; col++) {
                    // Ray Form: r(t) = o + t.d
                    // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                    // Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                    float t = ray_sphere_intersection(rays[index].o, rays[index].d, c, radius);

                   if ((t > 0) && (rays[index].t == -1 || t == positive_min(t, rays[index].t))) {
                        rays[index].t = t;
                        rays[index].mid = scene.spheres[sid].material_id;
                        Vec3f n = rays[index].o + rays[index].t*rays[index].d - c;
                        rays[index].n = vector_normalize(n);
                        index++;
                        row_flag = true;
                    }
                    else {
                        index++;

                        if (row_flag) {
                            index = (row+1) * nx;
                            break;
                        }
                    }
                }

                if (row_flag)
                    col_flag = true;
                else if (!row_flag && col_flag)
                    break;
            }
        }

        // Iterate through triangles
        for (std::size_t tid = 0; tid < scene.triangles.size(); tid++) {
            //std::cout << "Triangle-" << tid+1 << std::endl << scene.triangles[tid] << std::endl;

            Face indice = scene.triangles[tid].indices;
            
            Vec3f a = scene.vertex_data[indice.v0_id-1];
            Vec3f b = scene.vertex_data[indice.v1_id-1];
            Vec3f c = scene.vertex_data[indice.v2_id-1];

            unsigned int index = 0;

            // Iterate through image plane (Width:nx, Height:ny)
            //bool col_flag = false;
            for (int row = 0; row < ny; row++) {
                //bool row_flag = false;
                for (int col = 0; col < nx; col++) {
                    // Ray Form: r(t) = o + t.d
                    // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                    // Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                    float t = ray_triangle_intersection(rays[index].o, rays[index].d, a, b, c);

                    if ((t > 0) && (rays[index].t == -1 || t == positive_min(t, rays[index].t))) {
                        rays[index].t = t;
                        rays[index].mid = scene.triangles[tid].material_id;
                        Vec3f n = vector_cross(b-a, c-b);
                        rays[index].n = vector_normalize(n);
                        index++;
                        //row_flag = true;
                    }
                    else {
                        index++;

                        /*if (row_flag) {
                            index = (row+1) * nx;
                            break;
                        }*/
                    }
                }

                /*if (row_flag)
                    col_flag = true;

                else if (!row_flag && col_flag)
                    break;*/
            }
        }

        // Iterate through meshes
        for (std::size_t meid = 0; meid < scene.meshes.size(); meid++) {

            //Iterate through triangles in the mesh
            for (std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++) {
                Face face = scene.meshes[meid].faces[fid];

                Vec3f a = scene.vertex_data[face.v0_id-1];
                Vec3f b = scene.vertex_data[face.v1_id-1];
                Vec3f c = scene.vertex_data[face.v2_id-1];

                unsigned int index = 0;

                // Iterate through image plane (Width:nx, Height:ny)
                //bool col_flag = false;
                for (int row = 0; row < ny; row++) {
                    //bool row_flag = false;
                    for (int col = 0; col < nx; col++) {
                        // Ray Form: r(t) = o + t.d
                        // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                        // Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                        float t = ray_triangle_intersection(rays[index].o, rays[index].d, a, b, c);

                        if ((t > 0) && (rays[index].t == -1 || t == positive_min(t, rays[index].t))) {
                            rays[index].t = t;
                            rays[index].mid = scene.meshes[meid].material_id;
                            Vec3f n = vector_cross(b-a, c-b);
                            rays[index].n = vector_normalize(n);
                            index++;
                            //row_flag = true;
                        }
                        else {
                            index++;

                            /*if (row_flag) {
                                index = (row+1) * nx;
                                break;
                            }*/
                        }
                    }

                    /*if (row_flag)
                        col_flag = true;

                    else if (!row_flag && col_flag)
                        break;*/
                }
            }
        }

        // Color the Image Plane
        unsigned char *image = new unsigned char [nx * ny * 3];
        int i_img = 0;
        for (int i_ray = 0; i_ray < nx*ny; i_ray++) {
            Vec3f color = color_clamp(rays[i_ray], scene);
            image[i_img++] = (unsigned char) round(color.x);
            image[i_img++] = (unsigned char) round(color.y);
            image[i_img++] = (unsigned char) round(color.z);
        }

        // Output the Image Plane
        write_ppm(camera.image_name.c_str(), image, nx, ny);
    }

   return 0;
}