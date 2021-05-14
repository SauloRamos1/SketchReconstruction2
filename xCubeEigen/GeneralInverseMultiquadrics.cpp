#include "GeneralInverseMultiquadrics.h"

GeneralInverseMultiquadrics::GeneralInverseMultiquadrics()
{
    smooth = 2;
}

void GeneralInverseMultiquadrics::computePhi(double *center, double *vertex, double *ret)
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);



    *ret  =1/sqrt(t*t+smooth*smooth);


}
void GeneralInverseMultiquadrics::gradient(double *center, double *vertex, double grad[3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t2 = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);



    double t2mc2 = t2*t2 + smooth*smooth;

    //double c = -1/sqrt(t2mc2*t2mc2*t2mc2);

    //double c = -1/(pow(t2mc2,3/2));
    double c = -1/(sqrt(t2mc2*t2mc2*t2mc2));
    grad[0] = p[0] * c;
    grad[1] = p[1] * c;
    grad[2] = p[2] * c;

}

void GeneralInverseMultiquadrics::hessian(double *center, double *vertex, double hess[3][3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double t2 = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

    double t2mc2 = t2*t2 + smooth*smooth;

    //double frac =  -1/sqrt(t2mc2*t2mc2*t2mc2);
    //double frac = -1/(pow(t2mc2,3/2));
    double frac = -1/(sqrt(t2mc2*t2mc2*t2mc2));

    //double frac2 = -1/sqrt(t2mc2*t2mc2*t2mc2*t2mc2*t2mc2);
   // double frac2 = 3/(pow(t2mc2,5/2));

    double frac2 = 3/(sqrt(t2mc2*t2mc2*t2mc2*t2mc2*t2mc2));

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
