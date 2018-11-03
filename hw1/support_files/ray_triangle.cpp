#include "ray_triangle.h"

typedef parser::Vec3f MATRIX[3]; // 3x3 Matrix [v1|v2|v3], each vector represents a column

float matrix3_determinant(MATRIX M) { // Returns determinant of 3x3 matrix (MATRIX)
    return M[0].x*(M[1].y*M[2].z-M[2].y*M[1].z) - 
           M[1].x*(M[0].y*M[2].z-M[2].y*M[0].z) + 
           M[2].x*(M[0].y*M[1].z-M[1].y*M[0].z);
}

// t value for Ray and Triangle Intersection (use Barycentric Coordinates)
// Inputs: Vec3f o, Vec3f d, Vec3f a, Vec3f b, Vec3f c
//      r(t): o + t.d (o: origin, d: vector)
//      Triangle is contains 3 vertices a, b and c
//      Solve the equation for t: (Note that . is dot product)
//          p(A, B, C) = A*a + B*b + C*c
//          0<A<1, 0<B<1, 0<C<1 => A = 1-B-C where B+C<=1, B>=0, C>=0
//          p(r(t)) = o + t*d = A*a + B*b + C*c = a + B*(b-a) + C*(c-a)
//                    ox + t*dx = ax + B*(bx-ax) + C*(cx-ax)
//                    oy + t*dy = ay + B*(by-ay) + C*(cy-ay)
//                    oz + t*dz = az + B*(bz-az) + C*(cz-az)
//          Obtain t, B and C values with Cramer's rule.
//              | ax-bx  ax-cx  dx | | B |   | ax-ox |
//              | ay-by  ay-cy  dy |x| C | = | ay-oy |
//              | az-bz  az-cz  dz | | t |   | az-oz |
//      The ray will intersect the triangle if:
//          tmin <= t <= tmax, B+C<=1, B>=0, C>=0
// Output: Return the t value, if it satisfies the conditions, otherwise return -1
float ray_triangle_intersection(const parser::Vec3f &o, const parser::Vec3f &d, const parser::Vec3f &a, const parser::Vec3f &b, const parser::Vec3f &c) {
    MATRIX matrix_A = {a-b, a-c, d};
    float det_matrix_A = matrix3_determinant(matrix_A);

    

    MATRIX matrix_B = {a-o, a-c, d};
    float B = matrix3_determinant(matrix_B)/det_matrix_A;
    if (B < 0 - global_shadow_ray_epsilon)
        return INFINITY;

    MATRIX matrix_C = {a-b, a-o, d};
    float C = matrix3_determinant(matrix_C)/det_matrix_A;
    if ((C < 0 - global_shadow_ray_epsilon) || (B+C > 1 + global_shadow_ray_epsilon))
        return INFINITY;

    MATRIX matrix_T = {a-b, a-c, a-o};
    float t = matrix3_determinant(matrix_T)/det_matrix_A;
    if (t <= 0)
        return INFINITY;
    
    return t;
}
