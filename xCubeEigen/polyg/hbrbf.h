
#ifndef PBRT_HBRBF_SHAPE_H
#define PBRT_HBRBF_SHAPE_H

#include <iostream>
#include <fstream>


#include <string>

#include "point3d.h"
#include <cmath>

const unsigned int DIMN = 3;
typedef double real;

class HBRBF  {


  public:


	real operator()(Point3D p)
	{
		return this->eval(p);
	}


	real eval( Point3D p)
	{

		double sum = 0;
		double xx, xy, xz;


		double dotx = 0;
		double phix = 0;
	    for(int i = 0; i < numVertex; i++)
        {
		    //x-xi
		    xx = p[0]-vertexX[i];
		    xy = p[1]-vertexY[i];
	 	    xz = p[2]-vertexZ[i];

		    //normax
		    double nX = sqrt(xx*xx + xy*xy + xz*xz);
		    //phi(x-xi)
		    phix+= nX*nX*nX*coefs[i];
		}

	     for(int i = 0; i < numCenters; i++)
		{
			xx = p[0]-centersX[i];
		    	xy = p[1]-centersY[i];
	 	    	xz = p[2]-centersZ[i];
			double nX = sqrt(xx*xx + xy*xy + xz*xz);
		    	double grad[3];
		    	grad[0] =  3 * xx * nX;
		    	grad[1] =  3 * xy * nX;
		    	grad[2] =  3 * xz * nX;

		    	dotx+= grad[0]*coefs[numVertex+ i*3] + grad[1]*coefs[numVertex+ i*3+1] + grad[2]*coefs[numVertex+ i*3+2];

	    }
		double px = coefs[numCoefs-4] + coefs[numCoefs-3]*p[0] + coefs[numCoefs-2]*p[1] + coefs[numCoefs-1]*p[2];
        return phix - dotx +px;
	}


	void ComputeBB() ;
	bool loadCoefficients(std::ifstream & tin)  ;

	Point3D bbp1, bbp2;
	 double bboxoffset;


	double *vertexX;
        double *vertexY;
        double *vertexZ;

        double *centersX;
        double *centersY;
        double *centersZ;
        double *coefs;
        int numCoefs;
        int numVertex;
        int numCenters;

};




#endif // PBRT_HBRBF_SHAPE_H
