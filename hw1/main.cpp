#include "support_files/parser.h"
#include "support_files/ppm.h"
#include "support_files/ray.h"
#include "support_files/ray_sphere.h"
#include "support_files/ray_triangle.h"
#include "support_files/ray_mesh.h"
#include <sstream>
#include <iostream>
#include <string>

typedef unsigned char RGB[3];

using namespace parser;

int main(int argc, char* argv[]) {
    Scene scene;

    scene.loadFromXml(argv[1]);

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

        unsigned char *image = new unsigned char [nx * ny * 3];
        // Set background color to Image Plane
        for (std::size_t i = 0; i < nx*ny*3;) {
            image[i++] = scene.background_color.x;
            image[i++] = scene.background_color.y;
            image[i++] = scene.background_color.z;
        }

        Vec3f e = camera.position; // Camera position
        Vec3f v = camera.up; // Camera Up vector (v)
        Vec3f w = -camera.gaze; // Camera Opposite Gaze vector (w)
        Vec3f u = vector_cross(v, w); // Camera Right vector (u)
        Vec3f m = (-w) * d; // Vector to middle of image plane from camera
        Vec3f q = m + l*u + t*v; // Vector to top left corner of image plane from camera

        float pw = (r-l)/(float) nx; // Pixel width of each pixel
        float ph = (t-b)/(float) ny; // Pixel height of each pixel

        // Iterate through meshes
        for (std::size_t meid = 0; meid < scene.meshes.size(); meid++) {

            //Iterate through triangles in the mesh
            for (std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++) {
                Vec3f a = scene.vertex_data[scene.meshes[meid].faces[fid].v0_id-1];
                Vec3f b = scene.vertex_data[scene.meshes[meid].faces[fid].v1_id-1];
                Vec3f c = scene.vertex_data[scene.meshes[meid].faces[fid].v2_id-1];

                unsigned int image_index = 0;

                // Iterate through image plane (Width:nx, Height:ny)
                bool col_flag = false;
                for (int row = 0; row < ny; row++) {
                    bool row_flag = false;
                    for (int col = 0; col < nx; col++) {
                        // Ray Form: r(t) = o + t.d
                        // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                        Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                        float t = ray_triangle_intersection(e, d, a, b, c);

                        if (t > 0) {
                            image[image_index++] = 100 + fid*5;
                            image[image_index++] = 100 + fid*5;
                            image[image_index++] = 100 + fid*5;
                        }
                        else {
                            image_index += 3;

                            if (row_flag) {
                                image_index = (row+1) * nx * 3;
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
        }

        // Iterate through spheres
        for (std::size_t sid = 0; sid < scene.spheres.size(); sid++) {
            //std::cout << "Sphere-" << sid+1 << std::endl << scene.spheres[sid] << std::endl;
            
            Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id-1];
            float radius = scene.spheres[sid].radius;

            unsigned int image_index = 0;

            // Iterate through image plane (Width:nx, Height:ny)
            bool col_flag = false;
            for (int row = 0; row < ny; row++) {
                bool row_flag = false;
                for (int col = 0; col < nx; col++) {
                    // Ray Form: r(t) = o + t.d
                    // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                    Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                    float t = ray_sphere_intersection(e, d, c, radius);

                    if (t > 0) {
                        image[image_index++] = 155;
                        image[image_index++] = 155;
                        image[image_index++] = 155;
                        row_flag = true;
                    }
                    else {
                        image_index += 3;

                        if (row_flag) {
                            image_index = (row+1) * nx * 3;
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
            
            Vec3f a = scene.vertex_data[scene.triangles[tid].indices.v0_id-1];
            Vec3f b = scene.vertex_data[scene.triangles[tid].indices.v1_id-1];
            Vec3f c = scene.vertex_data[scene.triangles[tid].indices.v2_id-1];

            unsigned int image_index = 0;

            // Iterate through image plane (Width:nx, Height:ny)
            bool col_flag = false;
            for (int row = 0; row < ny; row++) {
                bool row_flag = false;
                for (int col = 0; col < nx; col++) {
                    // Ray Form: r(t) = o + t.d
                    // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                    Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                    float t = ray_triangle_intersection(e, d, a, b, c);

                    if (t > 0) {
                        image[image_index++] = 215;
                        image[image_index++] = 215;
                        image[image_index++] = 215;
                    }
                    else {
                        image_index += 3;

                        if (row_flag) {
                            image_index = (row+1) * nx * 3;
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

        // Output the image_plane
        write_ppm(camera.image_name.c_str(), image, nx, ny);
    }

    /*
    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }

    write_ppm(argv[2], image, width, height);
    */

   return 0;
}