
#ifndef SURFACEPOLYGONIZER_H
#define SURFACEPOLYGONIZER_H

#include "MC.h"
#include "point3d.h"
#include <vector>
#include <set>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "../HBRBFEval.h"
#include "../MeshData.h"

#define DIM 3


 struct Triangle
{
	int i1, i2, i3;
	Point3D v[3];	
};

typedef std::vector<Point3D> Vertices;
typedef std::vector<Triangle> Triangles;

typedef  Point3D Vector;


class SurfacePolygonizer
{
    public:
  
        SurfacePolygonizer(HBRBFEval *func);
	
	bool marchingCubes (Point3D x0, Point3D x1, double size, Triangles *triangles);
        Vertices vertices;             	/* surface vertices */

        void polygonize(const char *fileName, double spacing);

  private:
  
        HBRBFEval *f;				/* surface we are poligonizing */
  	double cubeSize;				/* cube size for marching cubes */
  	
//  	Vector start;                  	/* start point on surface */
        Point3D start;                  	/* start point on surface */
  	
  
  	
        Centerlist *centers;          	/* cube center hash table */
        Cornerlist *corners;          	/* corner value hash table */
        Edgelist *edges;              	/* edge and vertex id hash table */
    
        Test find(int sign, Point3D x);
        Corner setcorner(int i, int j, int k);
//        Vector setpoint(int i, int j, int k);
        Point3D  setpoint(int i, int j, int k);
        int dotet (Cube *cube, int c1, int c2, int c3, int c4, Triangles *triangles);
        int vertid (Corner &c1, Corner &c2);
        void setedge (int i1, int j1, int k1, int i2, int j2, int k2, int vid);
        int getedge (int i1, int j1, int k1, int i2, int j2, int k2);
        Point3D converge (Point3D p1, Point3D p2, double v);
        void freeMemory();
        int makeTriangle(int i1, int i2, int i3, Vertices vertices, Triangles *triangles);


        void ComputeBB();
        double bboxoffset;
        Point3D bbp1, bbp2;
	
        MeshData *mesh;
	
};

#endif
