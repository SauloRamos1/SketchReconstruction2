#ifndef HBRBF_H
#define HBRBF_H

#include "vecmath.h"
#include "MeshData.h"
#include <algorithm>
#include <ctime>
#include "RBF.h"
//Eigen lib
#include <Eigen/Dense>
//using namespace Eigen;


class HBRBF
{
    public:
        HBRBF(RBF *rbf, MeshData *mesh, bool usePoly);
        //build matrix and compute coefficients
        void run(const char *coeffsOut);
        void saveCoeffs(const char *file, int matrixSize, double * coefficients);

        MeshData *getMesh()
        {
            return mesh;
        }

    private:
        RBF *rbf;
        MeshData *mesh;
        bool usePoly;
};

#endif // HBRBF_H
