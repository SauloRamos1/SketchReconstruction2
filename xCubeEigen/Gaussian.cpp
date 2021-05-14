#include "Gaussian.h"

Gaussian::Gaussian()
{
    smooth = 2;
}

void Gaussian::computePhi(double *center, double *vertex, double *ret)
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);



    double ht = (smooth*smooth) * (t*t);

    *ret = exp(-ht);


}
void Gaussian::gradient(double *center, double *vertex, double grad[3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    double ht = (smooth*smooth) * (t*t);
    double c = -2*smooth*smooth*exp(-ht);

     grad[0] = p[0] * c;
     grad[1] = p[1] * c;
     grad[2] = p[2] * c;

}

void Gaussian::hessian(double *center, double *vertex, double hess[3][3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

     double ht = (smooth*smooth) * (t*t);
    double frac = -2*smooth*smooth*exp(-ht);

    double frac2 = 4*smooth*smooth*smooth*smooth*exp(-ht);

    hess[0][0] =  frac + frac2*p[0]*p[0];
    hess[0][1] = frac2*p[0]*p[1];
    hess[0][2] = frac2*p[0]*p[2];
    hess[1][0] = hess[0][1];
    hess[1][1] = frac + frac2*p[1]*p[1];
    hess[1][2] = frac2*p[1]*p[2];
    hess[2][0] = hess[0][2];
    hess[2][1] = hess[1][2];
    hess[2][2] = frac + frac2*p[2]*p[2];
}

