#ifndef HBRBFEVAL_H
#define HBRBFEVAL_H
//#include "XCube.h"
#include "RBF.h"
#include "MeshData.h"
#include "vecmath.h"
#include <fstream>
#include <algorithm>
//using namespace std;

class HBRBFEval
{
public:
    HBRBFEval(RBF *rbf,const char *data);
    ~HBRBFEval();
    double evalPoint(double point[3]);
    bool evalGradient(double xp[3],double ret[3]);
    bool evalTangent(double tangentPoint[3], double tan[3], double *ret);
    RBF *getRBF();
    MeshData *getMesh()
    {
        return mesh;
    }

  private:
    void loadCoeff(char *data);
    MeshData *mesh;
    double *coefficients;
    int numCoefficients;
    RBF *rbf;
    bool usePoly;
};

#endif // HBRBFEVAL_H
