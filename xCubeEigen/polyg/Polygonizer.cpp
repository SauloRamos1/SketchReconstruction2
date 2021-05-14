/*	#include <iostream>

#include "surfacepolygonizer.h"
#include "hbrbf.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>

int main (int argc, char * const argv[]) {

	if(argc != 4)
	{
		std::cerr << "Usage ./hrbfpolygonalization <input.coefficients> <output.off> <size-cube>\n";
		exit(1);
	}
	HBRBF hb;
	std::ifstream ifnamefile(argv[1],std::ios::in | std::ios::binary);

	std::cerr << "Loading coefficients " << std::endl;
	hb.loadCoefficients(ifnamefile);
	hb.ComputeBB();
	std::cerr << "BB computed " << std::endl;
	std::cerr << hb.bbp1[0] << " " <<hb.bbp1[1] << " " << hb.bbp1[2] << std::endl;
	std::cerr << hb.bbp2[0] << " " <<hb.bbp2[1] << " " << hb.bbp2[2] << std::endl;
	SurfacePolygonizer sp(&hb);
	std::cerr << "Surface Polygonizer created " << std::endl;
	Triangles trs;
	std::cerr << "Starting MC... " << std::endl;

	sp.marchingCubes(hb.bbp1, hb.bbp2, atof(argv[3]), &trs);
	std::cerr << "MC done!" << std::endl;


	std::ofstream ofnamefile(argv[2],std::ios::out);

	ofnamefile << "OFF\n " << sp.vertices.size() << " " << trs.size() << " 0 " << std::endl;

	for(int i = 0; i < sp.vertices.size(); i++)
		ofnamefile << std::setprecision(15) << (sp.vertices[i])[0] << " " << (sp.vertices[i])[1] << " " << (sp.vertices[i])[2] << " " << std::endl;
	for(int i = 0; i < trs.size(); i++)
		ofnamefile << "3 " << trs[i].i1 << " " << trs[i].i2 << " " << trs[i].i3 << " " << std::endl;


        std::cerr << "Verificando centroide do triangulo \n";
        for(int i = 0; i < trs.size(); i++)
        {
            int i1 = trs[i].i1 , i2 = trs[i].i1, i3 = trs[i].i3;
            double x = ( (sp.vertices[i1])[0] + (sp.vertices[i2])[0] + (sp.vertices[i3])[0] )/3.0;
            double y = ( (sp.vertices[i1])[1] + (sp.vertices[i2])[1] + (sp.vertices[i3])[1] )/3.0;
            double z = ( (sp.vertices[i1])[2] + (sp.vertices[i2])[2] + (sp.vertices[i3])[2] )/3.0;
            double ev = hb.eval(Point3D(x,y,z));
            if(ev > 1e-7)
                std::cerr << ev << std::endl;
        }

    return 0;
}
*/
