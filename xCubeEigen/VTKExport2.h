#ifndef VTKEXPORT_H
#define VTKEXPORT_H
#include "MeshData.h"
#include <iostream>
#include <cmath>
#include "HBRBFEval.h"
#include <vector>

//using namespace std;
class VTKExport2
{
public:
    VTKExport2(HBRBFEval *hb, int resolution);
    void toVTK(const char *fileName);
private:
    void computeBB(MeshData *m);
    double bbp1[3],bbp2[3];
    HBRBFEval *hb;
    int resolution;
    KDTree<double,3> *pointKdtree;
    void computeNeighbours(double *points,
                           int numPoints, KDTree<double,3> *kdTree,
                           std::vector< std::vector<unsigned int> >& neighborPoints,
                           bool greaterThan );

    double *pOut, *pIn;//points deslocated outer and inn.

    RBF *rbf;
    double radius;
    MeshData *mesh;
    double bboxoffset;
};

#endif // VTKEXPORT_H
