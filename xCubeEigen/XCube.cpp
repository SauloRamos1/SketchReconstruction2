#include "XCube.h"

XCube::XCube()
{

}

void XCube::computePhi(double *center, double *vertex, double *ret)
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double n = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    *ret = n * n *n;
}
void XCube::gradient(double *center, double *vertex, double grad[3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double n = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

    grad[0] = 3 * p[0] * n;
    grad[1] = 3 * p[1] * n;
    grad[2] = 3 * p[2] * n;

}

void XCube::hessian(double *center, double *vertex, double hess[3][3])
{
    double p[3];
    p[0] = vertex[0] - center[0];
    p[1] = vertex[1] - center[1];
    p[2] = vertex[2] - center[2];

    double n = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            hess[i][j] = 0;
        }
    }
    if(n!=0)
    {
        //cout << "hei" << endl;
        double tn = 3.0/n;
        hess[0][0]=tn*(n*n + p[0]*p[0]);    hess[0][1]=tn*(p[0]*p[1]);          hess[0][2]=tn*(p[0]*p[2]);
        hess[1][0]=hess[0][1];              hess[1][1]= tn*(n*n+ p[1]*p[1]);    hess[1][2]=tn*(p[1]*p[2]);
        hess[2][0]=hess[0][2];              hess[2][1]=hess[1][2];              hess[2][2]=tn*(n*n + p[2]*p[2]);
    }
}
