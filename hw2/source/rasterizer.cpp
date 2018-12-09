#include <cstdio>
#include <cstdlib>
#include <math.h>
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

void viewport_transformation(double result[3][4], Camera cam);
void perspective_transformation(double result[4][4], Camera cam);
void camera_transformation(double result[4][4], Camera cam);
void translate(double M_translate[4][4], Translation t);
void scale(double M_scale[4][4], Scaling s);
void rotate(double M_rotate[4][4], Rotation r);
void model_transformation(double M_model[4][4], Model const &model);
Vec3 transform_point(double M_vp[3][4], double M_per[4][4], double M_cam[4][4], double M_model[4][4], Vec3 v);

bool is_backfaced(Vec3 v_1, Vec3 v_2, Vec3 v_3);
void draw_line(Vec3 a, Vec3 b);
void draw_triangle(Vec3 v_1, Vec3 v_2, Vec3 v_3);

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
        Model model = models[i];
        double M_model[4][4];
        model_transformation(M_model, model);

        for (int j=0; j<model.numberOfTriangles; j++) {
            int vertex_id_0 = model.triangles[j].vertexIds[0];
            int vertex_id_1 = model.triangles[j].vertexIds[1];
            int vertex_id_2 = model.triangles[j].vertexIds[2];

            Vec3 v_1 = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_0]);
            Vec3 v_2 = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_1]);
            Vec3 v_3 = transform_point(M_vp, M_per, M_cam, M_model, vertices[vertex_id_2]);

            if (backfaceCullingSetting && is_backfaced(v_1, v_2, v_3)) { // Backface Culling is enabled and the triangle is backfaced.
                if (model.type) { // Solid
                    draw_triangle(v_1, v_2, v_3);
                }
                else { // Wireframe
                    draw_line(v_1, v_2);
                    draw_line(v_2, v_3);
                    draw_line(v_3, v_1);
                }
            }
            else if (!backfaceCullingSetting) { // Backface Culling is disabled
                if (model.type) { // Solid
                    draw_triangle(v_1, v_2, v_3);
                }
                else  {  // Wireframe
                    draw_line(v_1, v_2);
                    draw_line(v_2, v_3);
                    draw_line(v_3, v_1);
                }
            }
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
        //convertPPMToPNG(cameras[i].outputFileName, 1);
    }

    return 0;
}

// Helper Function Definitions
void multiply_M34WithVec4d(double r[3], double m[3][4], double v[4]);
void assign_matrix(double lhs[4][4], double rhs[4][4]);
Color addColor(Color c1, Color c2);
Color subtractColor(Color c1, Color c2);
Color multiplyColorWithScalar(Color c, double s);

void viewport_transformation(double result[3][4], Camera cam) {
    double assing[3][4] = {
        {cam.sizeX / 2, 0, 0, (cam.sizeX - 1) / 2},
        {0, cam.sizeY / 2, 0, (cam.sizeY - 1) / 2},
        {0, 0, 0.5, 0.5}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = assing[i][j];
        }
    }
}
void perspective_transformation(double result[4][4], Camera cam) {
    double assing[4][4] = {
        {(2 * cam.n) / (cam.r - cam.l), 0, (cam.r + cam.l) / (cam.r - cam.l), 0},
        {0, (2 * cam.n) / (cam.t - cam.b), (cam.t + cam.b) / (cam.t - cam.b), 0},
        {0, 0, -(cam.f + cam.n) / (cam.f - cam.n), -(2 * cam.f * cam.n) / (cam.f - cam.n)},
        {0, 0, -1, 0}
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = assing[i][j];
        }
    }
}

void camera_transformation(double result[4][4], Camera cam) {
    double assing[4][4] = {
        {cam.u.x, cam.u.y, cam.u.z, -(cam.u.x * cam.pos.x + cam.u.y * cam.pos.y + cam.u.z * cam.pos.z)},
        {cam.v.x, cam.v.y, cam.v.z, -(cam.v.x * cam.pos.x + cam.v.y * cam.pos.y + cam.v.z * cam.pos.z)},
        {cam.w.x, cam.w.y, cam.w.z, -(cam.w.x * cam.pos.x + cam.w.y * cam.pos.y + cam.w.z * cam.pos.z)},
        {0, 0, 0, 1}
    };
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = assing[i][j];
        }
    }
}

void translate(double M_translate[4][4], Translation t) {
    double translation_matrix[4][4] = {
        {1, 0, 0, t.tx},
        {0, 1, 0, t.ty},
        {0, 0, 1, t.tz},
        {0, 0, 0, 1}
    };

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            M_translate[i][j] = translation_matrix[i][j];
        }
    }
}

void scale(double M_scale[4][4], Scaling s) {
    double scaling_matrix[4][4] = {
        {s.sx, 0,    0,    0},
        {0,    s.sy, 0,    0},
        {0,    0,    s.sz, 0},
        {0,    0,    0,    1}
    };

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            M_scale[i][j] = scaling_matrix[i][j];
        }
    }
}

void rotate(double M_rotate[4][4], Rotation r) {
    double theta = r.angle * M_PI / 180;
    Vec3 u; u.x = r.ux; u.y = r.uy; u.z = r.uz;
    Vec3 v, w;

    if (abs(u.x) <= abs(u.y) && abs(u.x) <= abs(u.z)) {
        v.x = 0;
        v.y = -u.z;
        v.z = u.y;
    }
    else if (abs(u.y) <= abs(u.x) && abs(u.y) <= abs(u.z)) {
        v.x = -u.z;
        v.y = 0;
        v.z = u.x;
    }
    else { // (abs(u.z) <= abs(u.x) && abs(u.z) <= abs(u.y)) 
        v.x = -u.y;
        v.y = u.x;
        v.z = 0;
    }

    u = normalizeVec3(u);
    v = normalizeVec3(v);
    w = crossProductVec3(u, v);

    double M[4][4] = {
        {u.x, u.y, u.z, 0},
        {v.x, v.y, v.z, 0},
        {w.x, w.y, w.z, 0},
        {0,   0,   0,   1}
    };

    double M_i[4][4] = {
        {u.x, v.x, w.x, 0},
        {u.y, v.y, w.y, 0},
        {u.z, v.z, w.z, 0},
        {0,   0,   0,   1}
    };

    double R[4][4] = {
        {1,          0,           0, 0},
        {0, cos(theta), -sin(theta), 0},
        {0, sin(theta),  cos(theta), 0},
        {0,           0,          0, 1}
    };

    double m[4][4];
    multiplyMatrixWithMatrix(m, M_i, R);
    multiplyMatrixWithMatrix(M_rotate, m, M);
}
void model_transformation(double M_model[4][4], Model const &model) {
    makeIdentityMatrix(M_model);

    for (int i=0; i<model.numberOfTransformations; i++) {
        double tmp[4][4];
        double result[4][4];
        assign_matrix(result, M_model);

        if (model.transformationTypes[i] == 't') {
            int translation_id = model.transformationIDs[i];
            translate(tmp, translations[translation_id]);
            multiplyMatrixWithMatrix(M_model, tmp, result);
        }
        else if (model.transformationTypes[i] == 's') {
            int scaling_id = model.transformationIDs[i];
            scale(tmp, scalings[scaling_id]);
            multiplyMatrixWithMatrix(M_model, tmp, result);
        }
        else if (model.transformationTypes[i] == 'r') {
            int rotation_id = model.transformationIDs[i];
            rotate(tmp, rotations[rotation_id]);
            multiplyMatrixWithMatrix(M_model, tmp, result);
        }
        else {
            printf("BUG! BUG! BUG! BUG! BUG! BUG! BUG! BUG!\n");
        }
    }
}

Vec3 transform_point(double M_vp[3][4], double M_per[4][4], double M_cam[4][4], double M_model[4][4], Vec3 v) {
    double vectorMatrix[4] = {v.x, v.y, v.z, 1};
    double M_per_cam[4][4];
    double M_per_cam_model[4][4];
    double M_pdivide[4];

    double result[3];
    Vec3 result_vec;

    multiplyMatrixWithMatrix(M_per_cam, M_per, M_cam);
    multiplyMatrixWithMatrix(M_per_cam_model, M_per_cam, M_model);
    multiplyMatrixWithVec4d(M_pdivide, M_per_cam_model, vectorMatrix);

    // Perspective Divide
    M_pdivide[0] /=  M_pdivide[3];
    M_pdivide[1] /= M_pdivide[3];
    M_pdivide[2] /= M_pdivide[3];
    M_pdivide[3] /= M_pdivide[3];

    multiply_M34WithVec4d(result, M_vp, M_pdivide);

    result_vec.x = result[0];
    result_vec.y = result[1];
    result_vec.z = result[2];
    result_vec.colorId = v.colorId;

    return result_vec;
}

bool is_backfaced(Vec3 v_1, Vec3 v_2, Vec3 v_3) {
    Vec3 normal = crossProductVec3(subtractVec3(v_2,v_1),subtractVec3(v_3,v_1));

    if (dotProductVec3(normal, v_1) > 0)
        return true;
    else
        return false;
}

void draw_line(Vec3 a, Vec3 b) {
    double m = atan2(b.y-a.y, b.x-a.x);

    if (m >= 0 && m <= M_PI_4) {
        int x0 = (int) round(a.x);
        int y0 = (int) round(a.y);
        int x1 = (int) round(b.x);
        int y1 = (int) round(b.y);

        int y = y0;
        int d = 2 * (y0 - y1) + (x1 - x0);

        Color c0 = colors[a.colorId];
        Color c1 = colors[b.colorId];

        Color c = c0;
        Color dc = multiplyColorWithScalar(subtractColor(c1, c0), 1.0 / (x1 - x0));

        for (int x=x0; x <= x1; x++) {
            image[x][y] = c;

            if (d < 0) {
                y += 1;
                d += 2 * ((y0 - y1) + (x1 - x0));
            }
            else {
                d += 2 * (y0 - y1);
            }

            c = addColor(c, dc);
        }
    }
    else if (m > M_PI_4 && m <= M_PI_2) {
        int x0 = (int) round(a.x);
        int y0 = (int) round(a.y);
        int x1 = (int) round(b.x);
        int y1 = (int) round(b.y);

        int x = x0;
        int d = 2 * (x1 - x0) + (y0 - y1);

        Color c0 = colors[a.colorId];
        Color c1 = colors[b.colorId];

        Color c = c0;
        Color dc = multiplyColorWithScalar(subtractColor(c1, c0), 1.0 / (y1 - y0));

        for (int y=y0; y <= y1; y++) {
            image[x][y] = c;

            if (d > 0) {
                x += 1;
                d += 2 * ((x1 - x0) + (y0 - y1));
            }
            else {
                d += 2 * (x1 - x0);
            }

            c = addColor(c, dc);
        }
    }
    else if (m > M_PI_2 && m < 3 * M_PI_4) {
        int x0 = (int) round(a.x);
        int y0 = (int) round(a.y);
        int x1 = (int) round(b.x);
        int y1 = (int) round(b.y);

        int x = x0;
        int d = 2 * (x1 - x0) - (y0 - y1);

        Color c0 = colors[a.colorId];
        Color c1 = colors[b.colorId];

        Color c = c0;
        Color dc = multiplyColorWithScalar(subtractColor(c1, c0), 1.0 / (y1 - y0));

        for (int y=y0; y <= y1; y++) {
            image[x][y] = c;

            if (d < 0) {
                x -= 1;
                d += 2 * ((x1 - x0) - (y0 - y1));
            }
            else {
                d += 2 * (x1 - x0);
            }

            c = addColor(c, dc);
        }
    }
    else if (m >= 3 * M_PI_4 && m < M_PI) {
        int x0 = (int) round(a.x);
        int y0 = (int) round(a.y);
        int x1 = (int) round(b.x);
        int y1 = (int) round(b.y);

        int y = y0;
        int d = -2 * (y0 - y1) + (x1 - x0);

        Color c0 = colors[a.colorId];
        Color c1 = colors[b.colorId];

        Color c = c0;
        Color dc = multiplyColorWithScalar(subtractColor(c1, c0), 1.0 / (x0 - x1));

        for (int x=x0; x >= x1; x--) {
            image[x][y] = c;

            if (d > 0) {
                y += 1;
                d -= 2 * ((y0 - y1) - (x1 - x0));
            }
            else {
                d -= 2 * (y0 - y1);
            }

            c = addColor(c, dc);
        }
    }
    else {
        draw_line(b, a);
    }
}

void draw_triangle(Vec3 v_1, Vec3 v_2, Vec3 v_3) {
    int min_x = (int) round(std::min(v_1.x, std::min(v_2.x, v_3.x)));
    int min_y = (int) round(std::min(v_1.y, std::min(v_2.y, v_3.y)));
    int max_x = (int) round(std::max(v_1.x, std::max(v_2.x, v_3.x)));
    int max_y = (int) round(std::max(v_1.y, std::max(v_2.y, v_3.y)));

    double alpha;
    double beta;
    double gamma;
    
    double denom_alpha = v_1.x * (v_2.y - v_3.y) + v_1.y * (v_3.x - v_2.x) + v_2.x * v_3.y - v_2.y * v_3.x;
    double denom_beta  = v_2.x * (v_3.y - v_1.y) + v_2.y * (v_1.x - v_3.x) + v_3.x * v_1.y - v_3.y * v_1.x;
    double denom_gamma = v_3.x * (v_1.y - v_2.y) + v_3.y * (v_2.x - v_1.x) + v_1.x * v_2.y - v_1.y * v_2.x;

    Color c1, c2, c3, c;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            alpha = x * (v_2.y - v_3.y) + y * (v_3.x - v_2.x) + v_2.x * v_3.y - v_2.y * v_3.x;
            beta  = x * (v_3.y - v_1.y) + y * (v_1.x - v_3.x) + v_3.x * v_1.y - v_3.y * v_1.x;
            gamma = x * (v_1.y - v_2.y) + y * (v_2.x - v_1.x) + v_1.x * v_2.y - v_1.y * v_2.x;

            alpha /= denom_alpha;
            beta  /= denom_beta;
            gamma /= denom_gamma;

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                c1 = colors[v_1.colorId];
                c2 = colors[v_2.colorId];
                c3 = colors[v_3.colorId];

                c.b = alpha * c1.b + beta * c2.b + gamma * c3.b;
                c.r = alpha * c1.r + beta * c2.r + gamma * c3.r;
                c.g = alpha * c1.g + beta * c2.g + gamma * c3.g;

                image[x][y] = c;
            }
        }
    }
}

// Helper Functions
// ================
// Multiply matrices -> m[3][4] * v[4][1] = r[3][1]
void multiply_M34WithVec4d(double r[3], double m[3][4], double v[4]) {
    int i, j;
    double total;

    for (i = 0; i < 3; i++) {
        total = 0;
        for (j = 0; j < 4; j++) {
            total += m[i][j] * v[j];
        }

        r[i] = total;
    }
}

void assign_matrix(double lhs[4][4], double rhs[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            lhs[i][j] = rhs[i][j];
        }
    }
}

Color addColor(Color c1, Color c2) {
    Color result = {c1.r + c2.r, c1.g + c2.g, c1.b + c2.b};
    return result;
}

Color subtractColor(Color c1, Color c2) {
    Color result = {c1.r - c2.r, c1.g - c2.g, c1.b - c2.b};
    return result;
}

Color multiplyColorWithScalar(Color c, double s) {
    Color result = {s * c.r, s * c.g, s * c.b};
    return result;
}
