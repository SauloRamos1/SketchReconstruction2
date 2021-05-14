#ifndef VECMATH_H
#define VECMATH_H
#include <cmath>
class VecMath
{
public:
    VecMath();
    static void vetMac(double vec[3], double mat[3][3], double res[3])
    {
        res[0] = vec[0]*mat[0][0] + vec[1]*mat[1][0] + vec[2]*mat[2][0];
        res[1] = vec[0]*mat[0][1] + vec[1]*mat[1][1] + vec[2]*mat[2][1];
        res[2] = vec[0]*mat[0][2] + vec[1]*mat[1][2] + vec[2]*mat[2][2];

    }
    static double dot(double vec1[3], double vec2[3])
    {
        return (vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2]);
    }
    static void normalize(double vec[3])
    {
        double n = calcNorm(vec);
        vec[0] = vec[0] / n;
        vec[1] = vec[1] / n;
        vec[2] = vec[2] / n;

    }
    static double calcNorm(double vec[3])
    {
        return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
    }
};

#endif // VECMATH_H
