#include <iostream>




#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "RBF.h"
#include "Eval.h"

//#include "HBRBFEval.h"

double bbp1[3],bbp2[3];
void computeBB(MeshData *m)
{
    double bboxoffset = 1.9;
	double min[] = {HUGE_VAL,HUGE_VAL,HUGE_VAL} ,max[] = {-HUGE_VAL,-HUGE_VAL,-HUGE_VAL} ;
	for(int i =0; i < m->numPoints; i++)
	{
                if(m->points[i*3  ] < min[0]) min[0] = m->points[i*3  ];
                if(m->points[i*3+1] < min[1]) min[1] = m->points[i*3+1];
                if(m->points[i*3+2] < min[2]) min[2] = m->points[i*3+2];


                if(m->points[i*3  ] > max[0]) max[0] = m->points[i*3  ];
                if(m->points[i*3+1] > max[1]) max[1] = m->points[i*3+1];
                if(m->points[i*3+2] > max[2]) max[2] = m->points[i*3+2];


	}


        bbp1[0] = 0.5*(min[0]+max[0])-bboxoffset*(0.5*(max[0]-min[0]));
        bbp1[1] = 0.5*(min[1]+max[1])-bboxoffset*(0.5*(max[1]-min[1]));
        bbp1[2]  = 0.5*(min[2]+max[2])-bboxoffset*(0.5*(max[2]-min[2]));

        bbp2[0] = 0.5*(min[0]+max[0])+bboxoffset*(0.5*(max[0]-min[0]));
        bbp2[1] = 0.5*(min[1]+max[1])+bboxoffset*(0.5*(max[1]-min[1]));
        bbp2[2] = 0.5*(min[2]+max[2])+bboxoffset*(0.5*(max[2]-min[2]));
}

int main (int argc, char * const argv[]) {

	if(argc != 4)
	{
		std::cerr << "Usage ./hrbf_polygonalizer <input.coefficients> <output.vtk> <resolution>\n";
		exit(1);
	}
        RBF r;
        r.radius = 1;

	//std::ifstream ifnamefile(argv[1],std::ios::in | std::ios::binary);
	unsigned int resolution = atoi(argv[3]);

	std::cerr << "Loading coefficients " << std::endl;
        //Eval hb(&r,argv[1]);
         Eval hb(&r,argv[1]);
	computeBB(hb.getMeshData());

	std::ofstream ofnamefile(argv[2],std::ios::out);


	ofnamefile << "# vtk DataFile Version 3.0\nNon-Uniform rect grid \nASCII\n";
	ofnamefile << "DATASET RECTILINEAR_GRID" << std::endl;
	ofnamefile << "DIMENSIONS " << resolution << " " << resolution << " " << resolution  <<std::endl;
	ofnamefile << "X_COORDINATES " << resolution <<  " float\n";
	for(unsigned int i = 0; i < resolution ; i++)
            ofnamefile << bbp1[0] +  (double)i/(double)resolution*(bbp2[0]-bbp1[0]) << " ";
 	ofnamefile << "\nY_COORDINATES " <<resolution <<  " float\n";
	for(unsigned int i = 0; i < resolution ; i++)
            ofnamefile << bbp1[1] +  (double)i/(double)resolution*(bbp2[1]-bbp1[1]) << " ";
	ofnamefile << "\nZ_COORDINATES " << resolution <<  " float\n";
    for(unsigned int i = 0; i < resolution ; i++)
        ofnamefile << bbp1[2] +  (double)i/(double)resolution*(bbp2[2]-bbp1[2]) << " ";
	ofnamefile << "\nPOINT_DATA " << resolution*resolution*resolution<< std::endl;
    ofnamefile << "SCALARS Escalares float 1\nLOOKUP_TABLE default\n" << std::endl;
    for(unsigned int i = 0; i < resolution ; i++)
		for(unsigned int j = 0; j < resolution ; j++)
			for(unsigned int k = 0; k < resolution ; k++)
			{

                            double p[3];
                            p[0] = bbp1[0] +  (double)k/(double)resolution*(bbp2[0]-bbp1[0]);
                            p[1] = bbp1[1] +  (double)j/(double)resolution*(bbp2[1]-bbp1[1]);
                            p[2] = bbp1[2] +  (double)i/(double)resolution*(bbp2[2]-bbp1[2]);

                                ofnamefile << hb.evalPoint(p) << "  ";
				ofnamefile.flush();

			//	std::cerr << i*resolution*resolution + j*30 + k << "  ";
			}
	ofnamefile.close();
	std::cerr << "Impressao para arquivo .vtk concluida\n " << std::endl;
    return 0;
}
