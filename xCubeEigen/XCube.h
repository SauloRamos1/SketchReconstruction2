#ifndef XCUBE_H
#define XCUBE_H
#include <cmath>
#include <iostream>
#include "RBF.h"
//using namespace std;
class XCube: public RBF
{
public:
    XCube();
    //double radius;


    //center[n], vertex[n], ret[1]
    //here n = 3
    void computePhi(double *center, double *vertex, double *ret);

    void gradient(double *center, double *vertex, double grad[3]);

    void hessian(double *center, double *vetex, double hess[3][3]);



};

#endif // XCUBE_H
