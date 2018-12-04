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
#include <math.h>

typedef unsigned char RGB[3];

using namespace parser;

float global_shadow_ray_epsilon;

int main(int argc, char* argv[]) {
    Scene scene;

    scene.loadFromXml(argv[1]);

    global_shadow_ray_epsilon = scene.shadow_ray_epsilon;

    for (std::size_t meid = 0; meid < scene.meshes.size(); meid++) {
        Face face = scene.meshes[meid].faces[0];

        float x_max = scene.vertex_data[face.v0_id-1].x;
        float x_min = scene.vertex_data[face.v0_id-1].x;
        float y_max = scene.vertex_data[face.v0_id-1].y;
        float y_min = scene.vertex_data[face.v0_id-1].y;
        float z_max = scene.vertex_data[face.v0_id-1].z;
        float z_min = scene.vertex_data[face.v0_id-1].z;

        for (std::size_t fid = 0; fid < scene.meshes[meid].faces.size(); fid++) {
            Face face = scene.meshes[meid].faces[fid];

            Vec3f v0 = scene.vertex_data[face.v0_id-1];
            Vec3f v1 = scene.vertex_data[face.v1_id-1];
            Vec3f v2 = scene.vertex_data[face.v2_id-1];

            x_max = std::max(x_max, std::max(v0.x, std::max(v1.x, v2.x)));
            y_max = std::max(y_max, std::max(v0.y, std::max(v1.y, v2.y)));
            z_max = std::max(z_max, std::max(v0.z, std::max(v1.z, v2.z)));
            x_min = std::min(x_min, std::min(v0.x, std::min(v1.x, v2.x)));
            y_min = std::min(y_min, std::min(v0.y, std::min(v1.y, v2.y)));
            z_min = std::min(z_min, std::min(v0.z, std::min(v1.z, v2.z)));
        }

        scene.min_mesh_vector.push_back(Vec3f {x_max, y_max, z_max});
        scene.max_mesh_vector.push_back(Vec3f {x_min, y_min, z_min});
    }

    scene.sphere_count = scene.spheres.size();
    scene.triangle_count = scene.triangles.size();
    scene.mesh_count = scene.meshes.size();

    for (size_t meid = 0; meid < scene.mesh_count; meid++) {
    	scene.mesh_face_count.push_back(scene.meshes[meid].faces.size());
    }

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

	    // Optimization
	    Vec3f pwu = pw * u;
	    Vec3f half_pwu = 0.5 * pwu;
	    Vec3f ph_v = ph * (-v);
	    Vec3f half_ph_v = 0.5 * ph_v;
	    Vec3f c_vec = q + half_pwu + half_ph_v;

        // Image Plane
        unsigned char *image = new unsigned char [nx * ny * 3];
        unsigned int index = 0;

        for (int row = 0; row < ny; row++) {
            for (int col = 0; col < nx; col++) {
                // Ray Form: r(t) = o + t.d
                // where d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v)
                //Vec3f d = q + (col+0.5)*pw*u + (row+0.5)*ph*(-v);
                Vec3f d = col * pwu + row * ph_v + c_vec;
                float t = INFINITY;
                int mid;
                Vec3f n;

                // Iterate through spheres
                for (std::size_t sid = 0; sid < scene.sphere_count; sid++) {
                    Vec3f c = scene.vertex_data[scene.spheres[sid].center_vertex_id-1];
                    float radius = scene.spheres[sid].radius;

                    float t_intersect = ray_sphere_intersection(e, d, c, radius);

                    //if ((t_intersect > 0) && (t == -1 || t_intersect == positive_min(t_intersect, t))) {
                    if (t_intersect < t) {
                        t = t_intersect;
                        mid = scene.spheres[sid].material_id;
                        n = vector_normalize(e + t * d - c);
                    }
                }

                // Iterate through triangles
                for (std::size_t tid = 0; tid < scene.triangle_count; tid++) {
                    Face indice = scene.triangles[tid].indices;
                    
                    Vec3f a = scene.vertex_data[indice.v0_id-1];
                    Vec3f b = scene.vertex_data[indice.v1_id-1];
                    Vec3f c = scene.vertex_data[indice.v2_id-1];

                    float t_intersect = ray_triangle_intersection(e, d, a, b, c);

                    //if ((t_intersect > 0) && (t == -1 || t_intersect == positive_min(t_intersect, t))) {
                    if (t_intersect < t) {
                        t = t_intersect;
                        mid = scene.triangles[tid].material_id;
                        n = vector_normalize(vector_cross(b-a, c-b));
                    }
                }

                // Iterate through meshes
                for (std::size_t meid = 0; meid < scene.mesh_count; meid++) {
                    if (is_ray_mesh_intersect(e, d, scene.max_mesh_vector[meid], scene.min_mesh_vector[meid])) {
                        /*Vec3f c = (max_mesh_vector[meid] + min_mesh_vector[meid]) * (1.0/2.0);
                        float r = vector_magnitude(c - max_mesh_vector[meid]) + scene.shadow_ray_epsilon;

                        if (!is_ray_sphere_intersect(e, d, c, r))
                            continue;*/

                        //Iterate through triangles in the mesh
                        for (std::size_t fid = 0; fid < scene.mesh_face_count[meid]; fid++) {
                            Face face = scene.meshes[meid].faces[fid];

                            Vec3f a = scene.vertex_data[face.v0_id-1];
                            Vec3f b = scene.vertex_data[face.v1_id-1];
                            Vec3f c = scene.vertex_data[face.v2_id-1];

                            float t_intersect = ray_triangle_intersection(e, d, a, b, c);

                            //if ((t_intersect > 0) && (t == -1 || t_intersect == positive_min(t_intersect, t))) {
                            if (t_intersect < t) {
                                t = t_intersect;
                                mid = scene.meshes[meid].material_id;
                                n = vector_normalize(vector_cross(b-a, c-b));
                            }
                        }
                    }
                }

                if ((t == INFINITY) || (t < 0)) {
                    image[index++] = scene.background_color.x;
                    image[index++] = scene.background_color.y;
                    image[index++] = scene.background_color.z;
                }
                else {
                    Ray r;
                    r.o = e; r.t = t; r.d = d;
                    r.mid = mid; r.n = n;
                    Vec3f color = color_clamp(r, scene);
                    image[index++] = (unsigned char) round(color.x);
                    image[index++] = (unsigned char) round(color.y);
                    image[index++] = (unsigned char) round(color.z);
                }
            }
        }

        // Output the Image Plane
        write_ppm(camera.image_name.c_str(), image, nx, ny);

        delete[] image;
    }

   return 0;
}