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


/*
Gets two parameters, first one transformation matrix
second one is the point that needed to be transformed
It return new point after transformation.
*/
Vec3 transfromPoint(double transformMatrix[4][4], const Vec3 &point){
    Vec3 newPoint;
    double pointMatrix[4] = {point.x, point.y, point.z, 1};
    double resultMatrix[4];
    multiplyMatrixWithVec4d(resultMatrix, transformMatrix, pointMatrix);
    newPoint.x = resultMatrix[0];
    newPoint.y = resultMatrix[1];
    newPoint.z = resultMatrix[2];
    return newPoint;
}

/*
 * Multiply matrices m1 (double[3][4]) and m2 (double[4][4]) and store the result in result matrix r (double[3][4]).
 */
void multiplyMWM(double r[3][4], double m1[3][4], double m2[4][4]) {
    int i, j, k;
    double total;

    for (i = 0; i < 3; i++)
        for (j = 0; j < 4; j++) {
            total = 0;
            for (k = 0; k < 4; k++)
                total += m1[i][k] * m2[k][j];
            r[i][j] = total;
        }
}

/*
This function gets camera and result matrix which is empty for initial
After calculating last transformation matrix, it stores the result in resultMatrix*/
void getM_l2w(Camera cam, double resultMatrix[3][4]){
    //This is camera transformation matrix
    double M_cam[4][4] = {
        {cam.u.x, cam.u.y, cam.u.z, -(cam.u.x * cam.pos.x + cam.u.y * cam.pos.y + cam.u.z * cam.pos.z)},
        {cam.v.x, cam.v.y, cam.v.z, -(cam.v.x * cam.pos.x + cam.v.y * cam.pos.y + cam.v.z * cam.pos.z)},
        {cam.w.x, cam.w.y, cam.w.z, -(cam.w.x * cam.pos.x + cam.w.y * cam.pos.y + cam.w.z * cam.pos.z)},
        {0, 0, 0, 1}
    };
    //This is perspective transformation matrix
    double M_per[4][4] = {
        {(2 * cam.n) / (cam.r - cam.l), 0, (cam.r + cam.l) / (cam.r - cam.l), 0},
        {0, (2 * cam.n) / (cam.t - cam.b), (cam.t + cam.b) / (cam.t - cam.b), 0},
        {0, 0, -(cam.f + cam.n) / (cam.f - cam.n), -(2 * cam.f * cam.n) / (cam.f - cam.n)},
        {0, 0, -1, 0}
    };
    //This is viewport transformation matrix
    double M_vp[3][4] = {
        {cam.sizeX / 2, 0, 0, (cam.sizeX -1) / 2},
        {0, cam.sizeY / 2, 0, (cam.sizeY -1) / 2},
        {0, 0, 0.5, 0.5}
    };

    double M_per_cam[4][4];
    // M_vp * M_per * M_cam = Transform matrix which can transform vertices to the viewing volume
    multiplyMatrixWithMatrix;(M_per_cam, M_per, M_cam);
    multiplyMWM(resultMatrix, M_vp, M_per_cam);
}



/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    double M_l2w[3][4];  
    getM_l2w(cam, M_l2w);

    
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
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
