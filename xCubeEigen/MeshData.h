#ifndef MESHDATA_H
#define MESHDATA_H
#include <QString>
#include <fstream>
#include <iostream>
#include "vecmath.h"
//using namespace std;
class MeshData
{
public:
    MeshData(const QString &file);
    MeshData();
    int numPoints;
    int numNormals;
    int numTangents;
    double *points;
    double *normals;
    double *normalCenters;
    double *tangents;
    double *tangentPoints;
    double radius;
};

#endif // MESHDATA_H
