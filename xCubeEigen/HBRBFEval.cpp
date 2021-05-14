#include "HBRBFEval.h"
RBF *HBRBFEval::getRBF()
{
    return this->rbf;
}

double HBRBFEval::evalPoint(double point[3])
{

    double phi = 0, dotBeta = 0, poly = 0, tan = 0;
    for(int i = 0; i < mesh->numPoints; i++)
    {
        double val;
        rbf->computePhi(point,&mesh->points[i*3],&val);
        phi+= coefficients[i] * val;
    }
    int offset = mesh->numPoints;
    for(int i = 0; i < mesh->numNormals; i++)
    {
        double grad[3];
        rbf->gradient(point,&mesh->normalCenters[i*3],grad);
        double tmp = 0;
        tmp = grad[0] * coefficients[offset + i*3];
        tmp += grad[1] * coefficients[offset +i*3+1];
        tmp += grad[2] * coefficients[offset + i*3+2];
        dotBeta+= tmp;
    }
    offset = mesh->numPoints + mesh->numNormals * 3;
    for(int i = 0; i < mesh->numTangents; i++)
    {
        double *t = &mesh->tangents[i*3];
        double *p = &mesh->tangentPoints[i*3];
        double grad[3];
        rbf->gradient(point,p,grad);
        tan+= VecMath::dot(t,grad)*coefficients[offset + i];
    }
    if(usePoly)
    {
        //std::cout << "Using poly" << std::endl;
        offset = numCoefficients - 4;
        double tmp;
        //BUG CORRECTED
        tmp = coefficients[offset];
        tmp+= coefficients[offset+1] * point[0];
        tmp+= coefficients[offset+2] * point[1];
        tmp+= coefficients[offset+3] * point[2];
        poly = tmp;
    }

    double ret = phi - dotBeta - tan + poly;
    if(fabs(ret)<1e-10)
        ret = 0;
    return ret;
}

HBRBFEval::~HBRBFEval()
{

}

HBRBFEval::HBRBFEval(RBF *rbf, const char *data)
{
    this->rbf = rbf;
    std::cout << "load coeffs " << std::endl;
    std::ifstream arqIn;
    arqIn.open(data, std::ifstream::binary);
    if(arqIn)
    {
        std::cout << "arq ok" << std::endl;
        bool r;
        //use poly?
        arqIn.read((char*)&r,sizeof(bool));
        std::cout <<"R: " << r << std::endl;
        usePoly = r;
        mesh = new MeshData();

        int dataDimension;
        arqIn.read((char*)&dataDimension,sizeof(int));
        std::cout << "dataDim: " << dataDimension << std::endl;

        //load points
        arqIn.read((char*)&mesh->numPoints,sizeof(int));
        std::cout << "numPoints: " << mesh->numPoints << std::endl;

        mesh->points = new double[mesh->numPoints*3];
        for(int i = 0; i < mesh->numPoints; i++)
        {
            arqIn.read((char*)&mesh->points[i*3  ],sizeof(double));


            arqIn.read((char*)&mesh->points[i*3+1],sizeof(double));
            //std::cout << mesh->points[i].y << " ";
            arqIn.read((char*)&mesh->points[i*3+2],sizeof(double));
            //std::cout << mesh->points[i].z << " ";
            //std::cout << std::endl;
        }

        //normals
        arqIn.read((char*)&mesh->numNormals,sizeof(int));

        std::cout << "numNormals: " << mesh->numNormals << std::endl;
        mesh->normals = new double[mesh->numNormals*3];
        mesh->normalCenters = new double[mesh->numNormals*3];

        for(int i = 0; i < mesh->numNormals; i++)
        {
            arqIn.read((char*)&mesh->normals[i*3  ],sizeof(double));
            arqIn.read((char*)&mesh->normals[i*3+1],sizeof(double));
            //if(dataDimension==3)
            arqIn.read((char*)&mesh->normals[i*3+2],sizeof(double));
        }
        //normal centers
        for(int i = 0; i < mesh->numNormals; i++)
        {
            arqIn.read((char*)&mesh->normalCenters[i*3  ],sizeof(double));
            arqIn.read((char*)&mesh->normalCenters[i*3+1],sizeof(double));
            //if(dataDimension==3)
            arqIn.read((char*)&mesh->normalCenters[i*3+2],sizeof(double));
        }

        //tangent
        arqIn.read((char*)&mesh->numTangents,sizeof(int));
        mesh->tangents = new double[mesh->numTangents*3];
        mesh->tangentPoints = new double[mesh->numTangents*3];
        for(int i = 0; i < mesh->numTangents; i++)
        {
            arqIn.read((char*)&mesh->tangents[i*3  ],sizeof(double));
            arqIn.read((char*)&mesh->tangents[i*3+1],sizeof(double));
            //if(dataDimension==3)
            arqIn.read((char*)&mesh->tangents[i*3+2],sizeof(double));
        }
        //tangent points
        for(int i = 0; i < mesh->numTangents; i++)
        {
            arqIn.read((char*)&mesh->tangentPoints[i*3  ],sizeof(double));
            arqIn.read((char*)&mesh->tangentPoints[i*3+1],sizeof(double));
            //if(dataDimension==3)
            arqIn.read((char*)&mesh->tangentPoints[i*3+2],sizeof(double));
        }

        //coefficients
        int nCoeffs;
        arqIn.read((char*)&nCoeffs,sizeof(int));
        coefficients = new double[nCoeffs];
        numCoefficients = nCoeffs;
        for(int i = 0; i < nCoeffs; i++)
        {
            arqIn.read((char*)&coefficients[i],sizeof(double));
        }

        if(!arqIn.eof())
        {
            //read radius used.
            arqIn.read((char*)&mesh->radius,sizeof(double));
            rbf->radius = mesh->radius;
        }


    }
    arqIn.close();

}


bool HBRBFEval::evalGradient(double center[3],double ret[3])
{
    ret[0] = ret[1] = ret[2] = 0.0;
    //double center[3];
    //center[o(x[0],x[1],x[2]);
    //grad cj-vi
    for (unsigned int i = 0; i < mesh->numPoints; i++)
    {
        double g[] = {0,0,0};
        //RBF::gradient(center,this->Vertices[i],g);
        rbf->gradient(center,&mesh->points[i*3],g);

        ret[0] += g[0]*this->coefficients[i];
        ret[1] += g[1]*this->coefficients[i];
        ret[2] += g[2]*this->coefficients[i];
    }

    int offset = mesh->numPoints;

    for (unsigned int i = 0; i < mesh->numNormals; i++)
    {
        double hev[] = {0,0,0};
        double phessian[] = {coefficients[3*i   + offset],coefficients[3*i+1 + offset],coefficients[3*i +2  + offset]};

        rbf->eval_hessian(center,&mesh->normalCenters[i*3],phessian,hev);


        ret[0] -= hev[0];
        ret[1] -= hev[1];
        ret[2] -= hev[2];
    }
    //TANGENTS
    offset =  mesh->numPoints + 3 * mesh->numNormals;
    for(int i = 0; i < mesh->numTangents; i++)
    {
        double hev[]={0,0,0};
        double hess[3][3];
        //Hess(c-pi)
        rbf->hessian(center,&mesh->tangentPoints[i*3],hess);

        double th[3];
        double *ti = &mesh->tangents[i*3];
        //ti*hess
        th[0] =  ti[0] * hess[0][0] + ti[1]*hess[1][0] + ti[2]*hess[2][0];
        th[1] =  ti[0] * hess[0][1] + ti[1]*hess[1][1] + ti[2]*hess[2][1];
        th[2] =  ti[0] * hess[0][2] + ti[1]*hess[1][2] + ti[2]*hess[2][2];
        //gamma_i * ti
        ret[0] -= th[0]*coefficients[offset+i];
        ret[1] -= th[1]*coefficients[offset+i];
        ret[2] -= th[2]*coefficients[offset+i];
    }
    if(usePoly)
    {
        offset  = this->numCoefficients -  3;

        ret[0] += this->coefficients[offset];
        ret[1] += this->coefficients[offset+1];
        ret[2] += this->coefficients[offset+2];
    }
    if(fabs(ret[0])<1e-10)
        ret[0] = 0;

    if(fabs(ret[1])<1e-10)
        ret[1] = 0;

    if(fabs(ret[2])<1e-10)
        ret[2] = 0;
    return true;

}

bool HBRBFEval::evalTangent(double tangentPoint[3], double tan[3], double *ret)
{
    int offset;
    double phi = 0, grad = 0, dot = 0, poly = 0;

    for(int i = 0; i < mesh->numPoints; i++)
    {
        double grad[3];
        rbf->gradient(tangentPoint,&mesh->points[i*3],grad);

        double d = tan[0]*grad[0]+ tan[1]*grad[1] + tan[2]*grad[2];
        phi+= d*coefficients[i];
    }

    offset = mesh->numPoints;
    for(int i = 0; i < mesh->numNormals; i++)
    {
        double h[3][3];
        rbf->hessian(tangentPoint, &mesh->normalCenters[i*3],h);

        double d[3];
        d[0] = tan[0]*h[0][0] + tan[1]*h[1][0] + tan[2]*h[2][0];
        d[1] = tan[0]*h[0][1] + tan[1]*h[1][1] + tan[2]*h[2][1];
        d[2] = tan[0]*h[0][2] + tan[1]*h[1][2] + tan[2]*h[2][2];

        double *beta = &coefficients[offset + i*3];

        grad+= beta[0]*d[0] + beta[1]*d[1] + beta[2]*d[2];
    }

    offset = mesh->numPoints + mesh->numNormals * 3;
    for(int i = 0; i < mesh->numTangents; i++)
    {
        double h[3][3];
        rbf->hessian(tangentPoint,&mesh->tangentPoints[i*3], h);
        double th[3];
        double *t = &mesh->tangents[i*3];
        /*
        th[0] = t[0]*h[0][0] + t[1]*h[1][0] + t[2]*h[2][0];
        th[1] = t[0]*h[0][1] + t[1]*h[1][1] + t[2]*h[2][1];
        th[2] = t[0]*h[0][2] + t[1]*h[1][1] + t[2]*h[2][2];
*/
        VecMath::vetMac(t,h,th);

        double d = VecMath::dot(tan,th); //tan[0]*th[0] + tan[1]*th[1] + tan[2]*th[2];
        dot+= coefficients[offset+i]*d;

    }

    offset = numCoefficients - 3;
    if(usePoly)
    {

        poly += this->coefficients[offset]      * tan[0];

        poly += this->coefficients[offset+1]    *tan[1];
        poly += this->coefficients[offset+2]    *tan[2];

    }
    *ret = phi - grad - dot + poly;
    if(fabs(ret[0])<1e-10)
        *ret = 0;
    return true;
}
