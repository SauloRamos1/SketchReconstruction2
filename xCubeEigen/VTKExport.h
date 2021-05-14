#ifndef VTKEXPORT_H
#define VTKEXPORT_H
#include "MeshData.h"
#include <iostream>
#include <cmath>
#include "HBRBFEval.h"
//using namespace std;
class VTKExport
{
public:
    VTKExport(HBRBFEval *hb, int resolution);
    void toVTK(const char *fileName);
private:
    void computeBB(MeshData *m);
    double bbp1[3],bbp2[3];
    HBRBFEval *hb;
    int resolution;
};

#endif // VTKEXPORT_H
