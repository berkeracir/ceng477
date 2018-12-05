#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>


Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;

/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

void viewport_transformation(double result[4][4], Camera cam);
void perspective_transformation(double result[4][4], Camera cam);
void camera_transformation(double result[4][4], Camera cam);
void translate(double M_translate[4][4], Translation t);
void scale(double M_scale[4][4], Scaling s);
void rotate(double M_rotate[4][4], Rotation r);
void model_transformation(double M_model[4][4], int model_id);
Vec3 transform_point(double M_vp[3][4], double M_per[4][4], double M_cam[4][4], double M_model[4][4], Vec3 v);

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    double M_vp[3][4];
    viewport_transformation(M_vp, cam);
    double M_per[4][4];
    perspective_transformation(M_per, cam);
    double M_cam[4][4];
    camera_transformation(M_cam, cam);

    Vec3 world_vertices[100000];

    for (int i=0; i<numberOfModels; i++) {
        double M_model[4][4];
        model_transformation(M_model, i);
        double M_l2w[3][4]; // TODO M_l2w * M_model M34 * M44

        for (int j=0; j<models[i].numberOfTriangles; j++) {
            int vertex_id_0 = models[i].triangles[j].vertexIds[0];
            int vertex_id_1 = models[i].triangles[j].vertexIds[1];
            int vertex_id_2 = models[i].triangles[j].vertexIds[2];

            // TODOs: M34 x M41 (vertex) => Vec3 transform_point(double transformMatrix[3][4], Vec3 point)
            world_vertices[vertex_id_0] = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_0]);
            world_vertices[vertex_id_1] = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_1]);
            world_vertices[vertex_id_2] = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_2]);
        }
    }
}


int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
			for (j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 1);
    }

    return 0;

}

void viewport_transformation(double result[4][4], Camera cam) {;}
void perspective_transformation(double result[4][4], Camera cam) {;}
void camera_transformation(double result[4][4], Camera cam) {;}
void translate(double M_translate[4][4], Translation t) {;}
void scale(double M_scale[4][4], Scaling s) {;}
void rotate(double M_rotate[4][4], Rotation r) {;}
void model_transformation(double M_model[4][4], int model_id) {;}
Vec3 transform_point(double M_vp[3][4], double M_per[4][4], double M_cam[4][4], double M_model[4][4], Vec3 v) {
    return Vec3 {0, 0, 0};
}