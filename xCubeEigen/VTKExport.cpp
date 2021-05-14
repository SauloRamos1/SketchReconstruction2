#include "VTKExport.h"

VTKExport::VTKExport(HBRBFEval *hb, int resolution)
{
    this->hb = hb;
    this->resolution = resolution;
    computeBB(hb->getMesh());
}


void VTKExport::toVTK(const char *fileName)
{
    int numEvals = 0;

    std::cerr << "Loading coefficients ..." << std::endl;

    computeBB(hb->getMesh());
    std::cerr << "Exporting to VTK ... please wait." << std::endl;
    std::ofstream ofnamefile(fileName,std::ios::out);


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
                        //double r = hb->evalPoint()
                            ofnamefile << hb->evalPoint(p) << "  ";
                            ofnamefile.flush();
                            numEvals++;
                    //	std::cerr << i*resolution*resolution + j*30 + k << "  ";
                    }
    ofnamefile.close();
    std::cerr << "Exported to VTK file.\n " << std::endl;
    std::cerr<<"num evals: " << numEvals << std::endl;

}

void VTKExport::computeBB(MeshData *m)
{
    double bboxoffset = 1.7;
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
        bbp1[2]  = -100 ;// 0.5*(min[2]+max[2])-bboxoffset*(0.5*(max[2]-min[2]));

        bbp2[0] = 0.5*(min[0]+max[0])+bboxoffset*(0.5*(max[0]-min[0]));
        bbp2[1] = 0.5*(min[1]+max[1])+bboxoffset*(0.5*(max[1]-min[1]));
        bbp2[2] = 100;// 0.5*(min[2]+max[2])+bboxoffset*(0.5*(max[2]-min[2]));
}
