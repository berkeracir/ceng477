#include "support_files/parser.h"
#include "support_files/ppm.h"
#include "support_files/ray_trace.h"
#include <iostream>
#include <string>

typedef unsigned char RGB[3];

using namespace parser;

int main(int argc, char* argv[]) {
    Scene scene;

    scene.loadFromXml(argv[1]);

    // only for first camera
    Camera camera = scene.cameras[0];

    int nx = camera.image_width;
    int ny = camera.image_height;

    float l = camera.near_plane.x; // Left edge of image plane
    float r = camera.near_plane.y; // Right edge of image plane
    float b = camera.near_plane.z; // Bottom edge of image plane
    float t = camera.near_plane.w; // Top edge of image plane

    float d = camera.near_distance;

    //Vec3f* rays = new Vec3f [camera.image_width * camera.image_height];
    //Vec3f* image_plane_rays = new Vec3f [nx * ny];

    unsigned char *image = new unsigned char [nx * ny * 3];

    RGB color_black = {0, 0, 0};
    RGB color_white = {255, 255, 255};

    Vec3f e = camera.position; // Camera position
    std::cout << "position: " << camera.position << " - " << e << std::endl;
    Vec3f v = camera.up; // Camera Up vector (v)
    Vec3f w = -camera.gaze; // Camera Opposite Gaze vector (w)
    Vec3f u = vector_cross(v, w); // Camera Right vector (u)
    std::cout << "position: " << camera.position << " - " << e << std::endl;
    Vec3f m = e - w * d; // Vector to middle of image plane from camera
    std::cout << "position: " << camera.position << " - " << e << std::endl;
    Vec3f q = m + l*u + t*v; // Vector to top left corner of image plane from camera

    float pw = (r-l)/(float) nx; // Pixel width of each pixel
    float ph = (t-b)/(float) ny; // Pixel height of each pixel

    Vec3f c = scene.vertex_data[scene.spheres[0].center_vertex_id-1];
    float radius = scene.spheres[0].radius;

    std::cout << "position: " << camera.position << " - " << e << std::endl;
    std::cout << "up: " << camera.up << std::endl;
    std::cout << "gaze: " << camera.gaze << std::endl;
    std::cout << "near_plane: " << camera.near_plane.x << "," << camera.near_plane.y << "," << camera.near_plane.z << "," << camera.near_plane.w << std::endl;
    std::cout << "near_distance: " << camera.near_distance << std::endl;
    std::cout << "image_width: " << camera.image_width << std::endl;
    std::cout << "image_height: " << camera.image_height << std::endl;
    std::cout << "image_name: " << camera.image_name << std::endl;
    std::cout << "u: " << vector_cross(camera.gaze, camera.up) << std::endl;

    int image_index = 0;

    for (int row = 0; row < ny; row++) {
        for (int col = 0; col < nx; col++) {
            // Ray Form: r(t) = o + t.d
            // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
            Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
            float t = ray_sphere_intersection(e, d, c, radius);

            if (t > 0) {
                image[image_index++] = 255;
                image[image_index++] = 255;
                image[image_index++] = 255;
            }
            else {
                image[image_index++] = 0;
                image[image_index++] = 0;
                image[image_index++] = 0;
            }
        }
    }

    write_ppm(camera.image_name.c_str(), image, nx, ny);

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