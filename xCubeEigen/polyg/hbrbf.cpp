#include "hbrbf.h"

using namespace std;

bool HBRBF::loadCoefficients(std::ifstream & file)
{

	//tin.read();

	//ifstream file;
        //file.open(fileName,ios::in|ios::binary);
        if(file)
        {

            file.read((char*)&numCoefs,sizeof(int));
            //cout << "num coefs: " << numCoefs << endl;

            coefs = new double[numCoefs];

            double tmp = 0;
            int count = 0;
           // while(!file.eof())
            for(count = 0; count < numCoefs;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                coefs[count] = tmp;
               // count++;
            }

            //agora os vertex

            file.read((char*)&numVertex,sizeof(int));
            vertexX = new double[numVertex];
            vertexY = new double[numVertex];
            vertexZ = new double[numVertex];
            for(count = 0; count < numVertex;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                vertexX[count] = tmp;
               // count++;
            }

            for(count = 0; count < numVertex;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                vertexY[count] = tmp;
               // count++;
            }
            for(count = 0; count < numVertex;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                vertexZ[count] = tmp;
               // count++;
            }


            //le os centros


            file.read((char*)&numCenters,sizeof(int));
            centersX = new double[numCenters];
            centersY = new double[numCenters];
            centersZ = new double[numCenters];
            for(count = 0; count < numCenters;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                centersX[count] = tmp;
               // count++;
            }

            for(count = 0; count < numCenters;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                centersY[count] = tmp;
               // count++;
            }
            for(count = 0; count < numCenters;count++)
            {
                file.read((char*)&tmp,sizeof(double));
                centersZ[count] = tmp;
               // count++;
            }
            /*
	    	cout << "lido" << endl;
            for(int i = 0; i < numVertex; i++)
            {
                cout << "vert: " << vertexX[i] << " " << vertexY[i] << " " << vertexZ[i] << endl;
            }

            for(int i = 0; i < numCenters; i++)
            {
                cout << "cent: " << centersX[i] << " " << centersY[i] << " " << centersZ[i] << endl;
            }
            for(int i = 0; i < numCoefs; i++)
            {
                cout << "coef: " << coefs[i] <<endl;
            }
            cout << "-------------------------" << endl;*/
            file.close();
	}
}


