#include "SurfacePolygonizer.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void SurfacePolygonizer::polygonize(const char *fileName, double spacing, std::vector<float> *glVertices, std::vector<float> *glNormals, std::vector<unsigned int> *glFaces, QVector3D center, unsigned int nvertices)
{
    std::cout << "Polygonizing." << std::endl;
    ComputeBB();
    Triangles trs;
    marchingCubes(bbp1, bbp2, spacing, &trs);
//    std::ofstream ofnamefile(fileName,std::ios::out);

//    ofnamefile << "OFF\n " << vertices.size() << " " << trs.size() << " 0 " << std::endl;

//    for(int i = 0; i < vertices.size(); i++)
//    {
//            ofnamefile << std::setprecision(15) <<
//                          (vertices[i])[0] << " "
//                       << (vertices[i])[1] << " "
//                       << (vertices[i])[2] << " " << std::endl;
//    }
//    for(int i = 0; i < trs.size(); i++)
//    {
//            ofnamefile << "3 " << trs[i].i1 << " "
//                               << trs[i].i2 << " "
//                               << trs[i].i3 << " "
//                               << std::endl;
//    }
    std::cout << "Polygonizing done." << std::endl;
    std::cout << "Exporting PLY Closed Contour File." << std::endl;

    //Export PLY

    std::string outFile3 = fileName;
    outFile3 = outFile3.substr(0, outFile3.find_last_of("."));
    outFile3.append(".ply");
    std::ofstream fOut1;

    fOut1.open(outFile3.c_str());


    fOut1 << "ply" <<std::endl;
    fOut1 << "format ascii 1.0" << std::endl;
    fOut1 << "element vertex " << vertices.size() <<  std::endl;

    fOut1 << "property float x" << std::endl;
    fOut1 << "property float y" << std::endl;
    fOut1 << "property float z" << std::endl;

    fOut1 << "element face " << trs.size() << std::endl;

    fOut1 << "property list uint8 int32 vertex_indices" << std::endl;

    fOut1 << "end_header" << std::endl;



    for (int i = 0 ; i < vertices.size(); i++){
        fOut1<<
                (vertices[i])[0] << " "
             << (vertices[i])[1] << " "
             << (vertices[i])[2] << " " << std::endl;

        glVertices->push_back(vertices[i][0]);
        glVertices->push_back(vertices[i][1]);
        glVertices->push_back(vertices[i][2]);

        QVector3D p(vertices[i][0], vertices[i][1], vertices[i][2]);
        QVector3D normal(((p - center).normalized()));
        glNormals->push_back(normal.x());
        glNormals->push_back(normal.y());
        glNormals->push_back(normal.z());

    }

    for (int i = 0; i < trs.size(); i++) {
        fOut1 <<  3 << " " << trs[i].i1 << " "
                               << trs[i].i2 << " "
                               << trs[i].i3 << " "
                               << std::endl;

        glFaces->push_back(trs[i].i1 + nvertices);
        glFaces->push_back(trs[i].i2 + nvertices);
        glFaces->push_back(trs[i].i3 + nvertices);
    }


    fOut1.close();
}

void SurfacePolygonizer::ComputeBB()
{
    //this->bboxoffset = 1;
    this->bboxoffset = 1.1;
        double min[] = {HUGE_VAL,HUGE_VAL,HUGE_VAL} ,max[] = {-HUGE_VAL,-HUGE_VAL,-HUGE_VAL} ;
        for(int i =0; i < mesh->numPoints; i++)
        {

                if(mesh->points[i*3  ] < min[0]) min[0] = mesh->points[i*3  ];
                if(mesh->points[i*3+1] < min[1]) min[1] = mesh->points[i*3+1];
                if(mesh->points[i*3+2] < min[2]) min[2] = mesh->points[i*3+2];


                if(mesh->points[i*3  ] > max[0]) max[0] = mesh->points[i*3  ];
                if(mesh->points[i*3+1] > max[1]) max[1] = mesh->points[i*3+1];
                if(mesh->points[i*3+2] > max[2]) max[2] = mesh->points[i*3+2];


        }


        bbp1 =		Point3D(0.5*(min[0]+max[0])-bboxoffset*(0.5*(max[0]-min[0])),
                                   0.5*(min[1]+max[1])-bboxoffset*(0.5*(max[1]-min[1])),
                                 -200);//0.5*(min[2]+max[2])-bboxoffset*(0.5*(max[2]-min[2])));
                           // 0.5*(min[2]+max[2])-bboxoffset*(0.5*(max[2]-min[2])));
        bbp2 =		Point3D(0.5*(min[0]+max[0])+bboxoffset*(0.5*(max[0]-min[0])),
                                           0.5*(min[1]+max[1])+bboxoffset*(0.5*(max[1]-min[1])),
                                 200);//0.5*(min[2]+max[2])+bboxoffset*(0.5*(max[2]-min[2])));
               // 0.5*(min[2]+max[2])+bboxoffset*(0.5*(max[2]-min[2])));
}


/* constructor
 * param func: the implicit function that will be poligonized
 **/
SurfacePolygonizer::SurfacePolygonizer(HBRBFEval *func)
//: f(func)
{
    this->f = func;
    this->mesh = f->getMesh();
    centers = new Centerlist[HSIZE];
    corners = new Cornerlist[HSIZE];
    edges = new Edgelist[2*HSIZE];

}

/* Polygonizes specifically using marching cubes
 * param x0: start point of marching cubes
 * param x1: end point of marching cubes
 * param dize: size of the cube used in marching cubes
 * param triangles: triangles generated by marching cubes
 * return: if marching cubes was successful
 **/
Point3D operator*(double c, Point3D p)
{
    return Point3D( c*p.c[0], c*p.c[1], c*p.c[2]);
}

Point3D operator-(Point3D p, Point3D q)
{
    return Point3D( p[0]-q[0], p[1]-q[1], p[2]-q[2]);
}


Point3D operator+(Point3D p, Point3D q)
{
    return Point3D( p[0]+q[0], p[1]+q[1], p[2]+q[2]);
}


bool SurfacePolygonizer::marchingCubes (Point3D x0, Point3D x1, double size, Triangles *triangles)
{
    if(!f) return false;

    int i,j,k,n,i1,j1,k1;
    int pos;
    Cube cube;
    cubeSize = size;

    // add noise to hopefully nudge precise surfaces off lattice
    srand(1);				//initialize random number generator


    start = x1 - cubeSize*Point3D(.5,.5,.5) - 0.001*cubeSize*Point3D(RAND(),RAND(),RAND());


    i1 = (int)((x1[0] - x0[0])/cubeSize);
    j1 = (int)((x1[1] - x0[1])/cubeSize);
    k1 = (int)((x1[2] - x0[2])/cubeSize);

    printf("\n Resolucao do grid = (%d,%d,%d) \n", i1,j1,k1);
    //exit(1);


    for (k = 0; k <= k1; k++)
        for (j = 0; j <= j1; j++)
            for (i = 0; i <= i1; i++)
            {
                //if((i+j+k)%500== 0)
                                 //       printf("(%d,%d,%d)\n ",i,j,k);

                cube.i = i;
                cube.j = j;
                cube.k = k;

                pos = 0;
                /* set corners of cube: */

                //#pragma omp parallel for
                for (n = 0; n < 8; n++)
                {
                    cube.corners[n] = setcorner(i+BIT(n,2), j+BIT(n,1), k+BIT(n,0));
                    if (cube.corners[n].value > 0.0) pos++;
                }

                if (pos == 0 || pos == 8) continue;       /* cube probably empty */

                dotet(&cube, SP_LBN, SP_LTN, SP_RBN, SP_LBF, triangles);
                dotet(&cube, SP_RTN, SP_LTN, SP_LBF, SP_RBN, triangles);
                dotet(&cube, SP_RTN, SP_LTN, SP_LTF, SP_LBF, triangles);
                dotet(&cube, SP_RTN, SP_RBN, SP_LBF, SP_RBF, triangles);
                dotet(&cube, SP_RTN, SP_LBF, SP_LTF, SP_RBF, triangles);
                dotet(&cube, SP_RTN, SP_LTF, SP_RTF, SP_RBF, triangles);
            }
    freeMemory();
    return true;

}

/* find: search for point with value of given sign (0: neg, 1: pos) */
Test SurfacePolygonizer::find(int sign, Point3D x)
{
    int i;
    Test test;
    double range = cubeSize;

    test.ok = 1;
    for (i = 0; i < 10000; i++)
    {
        test.pt = x + range*Point3D(RAND()-0.5,RAND()-0.5,RAND()-0.5);
        double p[3];
        p[0] = test.pt.X();
        p[1] = test.pt.Y();
        p[2] = test.pt.Z();

        test.value = f->evalPoint(p);
        if (sign == (test.value > 0.0)) return test;
        range = range*1.0005; /* slowly expand search outwards */
    }
    test.ok = 0;
    return test;
}

/* return corner with the given lattice location set (and cache) its function value */
Corner SurfacePolygonizer::setcorner (int i, int j, int k)
{
    /* for speed, do corner value caching here */
    int index = HASH(i, j, k);
    //Cornerlist *l = corners[index];
    Vector pt;
    std::list<Corner>::iterator it;
    Corner l;
    l.i = i; l.j = j; l.k = k;
    it = std::find(corners[index].begin(), corners[index].end(), l);

    if(it != corners[index].end()) return *it;
    else {
        pt = setpoint(i, j, k);
        double p[3];
        p[0] = pt.X();
        p[1] = pt.Y();
        p[2] = pt.Z();

        l.value = f->evalPoint(p);
        corners[index].push_front(l);
        return l;
    }
}

/* setpoint: set point location given lattice location */
Vector SurfacePolygonizer::setpoint(int i, int j, int k)
{
    return start - cubeSize*Vector(i-0.5,j-0.5,k-0.5);
}

/* triangulate the tetrahedron */
int SurfacePolygonizer::dotet (Cube *cube, int c1, int c2, int c3, int c4, Triangles *triangles)
{
    Corner a = cube->corners[c1],
           b = cube->corners[c2],
           c = cube->corners[c3],
           d = cube->corners[c4];

    int index = 0, apos, bpos, cpos, dpos, e1, e2, e3, e4, e5, e6;

    if (apos = (a.value > 0.0)) index += 8;
    if (bpos = (b.value > 0.0)) index += 4;
    if (cpos = (c.value > 0.0)) index += 2;
    if (dpos = (d.value > 0.0)) index += 1;
    /* index now 4-bit number equal to one of the 16 possible cases */
    if (apos != bpos) e1 = vertid(a, b);
    if (apos != cpos) e2 = vertid(a, c);
    if (apos != dpos) e3 = vertid(a, d);
    if (bpos != cpos) e4 = vertid(b, c);
    if (bpos != dpos) e5 = vertid(b, d);
    if (cpos != dpos) e6 = vertid(c, d);
    /* 14 productive tet. cases (0000 and 1111 do not yield polygons */
    switch (index) {
        case 1:  return makeTriangle(e5, e6, e3, vertices, triangles);
        case 2:  return makeTriangle(e2, e6, e4, vertices, triangles);
        case 3:  return makeTriangle(e3, e5, e4, vertices, triangles) &&
                        makeTriangle(e3, e4, e2, vertices, triangles);
        case 4:  return makeTriangle(e1, e4, e5, vertices, triangles);
        case 5:  return makeTriangle(e3, e1, e4, vertices, triangles) &&
                        makeTriangle(e3, e4, e6, vertices, triangles);
        case 6:  return makeTriangle(e1, e2, e6, vertices, triangles) &&
                        makeTriangle(e1, e6, e5, vertices, triangles);
        case 7:  return makeTriangle(e1, e2, e3, vertices, triangles);
        case 8:  return makeTriangle(e1, e3, e2, vertices, triangles);
        case 9:  return makeTriangle(e1, e5, e6, vertices, triangles) &&
                        makeTriangle(e1, e6, e2, vertices, triangles);
        case 10: return makeTriangle(e1, e3, e6, vertices, triangles) &&
                        makeTriangle(e1, e6, e4, vertices, triangles);
        case 11: return makeTriangle(e1, e5, e4, vertices, triangles);
        case 12: return makeTriangle(e3, e2, e4, vertices, triangles) &&
                        makeTriangle(e3, e4, e5, vertices, triangles);
        case 13: return makeTriangle(e6, e2, e4, vertices, triangles);
        case 14: return makeTriangle(e5, e3, e6, vertices, triangles);
    }
    return 1;
}

/* return index for vertex on edge:
 * Remark: c1->value and c2->value are presumed of different sign
 * return saved index if any; else compute vertex and save */
int SurfacePolygonizer::vertid (Corner &c1, Corner &c2)
{

    Point3D a, b;
    int vid = getedge(c1.i, c1.j, c1.k, c2.i, c2.j, c2.k);
    if (vid != -1) return vid;                	// previously computed
    a = setpoint (c1.i, c1.j, c1.k);
    b = setpoint (c2.i, c2.j, c2.k);
    Point3D v(converge(a, b, c1.value)); 	// posn
    vertices.push_back(v);
    vid = vertices.size() - 1;
    setedge(c1.i, c1.j, c1.k, c2.i, c2.j, c2.k, vid);
    return vid;
}

/* set vertex id for edge */
void SurfacePolygonizer::setedge(int i1, int j1, int k1, int i2, int j2, int k2, int vid)
{
    unsigned int index;
    MC_Edge edge;

    if (i1>i2 || (i1==i2 && (j1>j2 || (j1==j2 && k1>k2))))
    {
        int t=i1; i1=i2; i2=t; t=j1; j1=j2; j2=t; t=k1; k1=k2; k2=t;
    }
    index = HASH(i1, j1, k1) + HASH(i2, j2, k2);
    edge.i1 = i1; edge.j1 = j1; edge.k1 = k1;
    edge.i2 = i2; edge.j2 = j2; edge.k2 = k2;
    edge.vid = vid;
    edges[index].push_front(edge);
}

/* return vertex id for edge; return -1 if not set */
int SurfacePolygonizer::getedge(int i1, int j1, int k1, int i2, int j2, int k2)
{
    if (i1>i2 || (i1==i2 && (j1>j2 || (j1==j2 && k1>k2))))
    {
        int t=i1; i1=i2; i2=t; t=j1; j1=j2; j2=t; t=k1; k1=k2; k2=t;
    }
    int index = HASH(i1, j1, k1) + HASH(i2, j2, k2);

    std::list<MC_Edge>::iterator it;
    MC_Edge q;
    q.i1 = i1; q.i2 = i2; q.j1 = j1; q.j2 = j2; q.k1 = k1; q.k2 = k2;
    it = std::find(edges[index].begin(), edges[index].end(), q);
    if(it == edges[index].end()) return -1;
    else return (*it).vid;
}

/* from two points of differing sign, converge to surface */
Vector SurfacePolygonizer::converge (Vector p1, Vector p2, double v)
{
    int i = 0;
    Point3D pos, neg, p;

    if (v < 0)
    {
        pos = p2;
        neg = p1;
    }
    else
    {
        pos = p1;
        neg = p2;
    }

    for (;;)
    {
        p = 0.5*(pos+neg);
        double p1[3];
        p1[0] = p.X();
        p1[1] = p.Y();
        p1[2] = p.Z();
        double fv = f->evalPoint(p1);
        if (i++ == RES || fabs(fv) < 1e-9)
        {
            return p;
        }
        if (fv > 0.0) pos = p;
        else neg = p;
    }
}

/* free all allocated memory */
void SurfacePolygonizer::freeMemory()
{
    int i;

    for (i = 0; i < HSIZE; i++)
    {
        corners[i].clear();
        centers[i].clear();
    }
      for (i = 0; i < 2*HSIZE; i++) edges[i].clear();

   // vertices.clear();
}

/* make a triangle and add it in the return triangle vector */
int SurfacePolygonizer::makeTriangle(int i1, int i2, int i3, Vertices vertices, Triangles *triangles)
{
    Triangle t;
    t.i1 = i1;
    t.i2 = i2;
    t.i3 = i3;
    t.v[0] = vertices[i1];
    t.v[1] = vertices[i2];
    t.v[2] = vertices[i3];
    triangles->push_back(t);

    return 1;
}
