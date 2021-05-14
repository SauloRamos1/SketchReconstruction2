#include "HBRBF.h"

HBRBF::HBRBF(RBF *rbf, MeshData *mesh, bool usePoly)
{
    this->rbf = rbf;
    this->mesh = mesh;
    this->usePoly = usePoly;
}


void HBRBF::run(const char *coeffsOut)
{

    int start,endBuildMatrix, endSolve;

    start = clock();

    int matrixSize = mesh->numPoints + mesh->numNormals * 3 + mesh->numTangents;
    if(usePoly)
        matrixSize += 4;
    int iStart, jStart;

    //eigen matrix
    Eigen::MatrixXd A(matrixSize,matrixSize);
    A = A * 0; //zero matrix.

    std::cout << "PHI" << std::endl;

    /************************************PHI************************************/
    for(int i = 0; i < mesh->numPoints; i++)
    {

        for(int j = i; j < mesh->numPoints; j++)
        {
            double val;
            rbf->computePhi(&mesh->points[i*3], &mesh->points[j*3],&val);
            A(i,j) = val;
            A(j,i) = val;//transposed

        }
    }

    std::cout << "GRAD" << std::endl;
    /************************************GRAD************************************/
    jStart= mesh->numPoints;
    for(int i = 0; i < mesh->numPoints; i++)
    {
        for(int j = 0; j < mesh->numNormals; j++)//no kdtree
        {
            double grad[3];
            rbf->gradient(&mesh->points[i*3],&mesh->normalCenters[j*3],grad);

            A(i,jStart+ j * 3  ) = -grad[0];
            A(i,jStart+ j * 3+1) = -grad[1];
            A(i,jStart+ j * 3+2) = -grad[2];

            //transposed
            A(jStart+ j * 3  ,i) = -grad[0];
            A(jStart+ j * 3+1,i) = -grad[1];
            A(jStart+ j * 3+2,i) = -grad[2];
        }
    }

    std::cout << "HESS " << std::endl;
    /************************************HESS************************************/
    iStart = mesh->numPoints;
    jStart = mesh->numPoints;

    for(int i = 0; i < mesh->numNormals; i++)
    {        
        for(int j = i; j < mesh->numNormals; j++)//no kdtree
        {
            double hess[3][3];
            rbf->hessian(&mesh->normalCenters[i*3],&mesh->normalCenters[j*3],hess);
            for(int l = 0; l < 3; l++)
            {
                for(int c = 0; c <3; c++)
                {
                    if(jStart + j*3+c>=iStart+i*3 + l)
                    {
                        A(iStart+i*3 + l,jStart + j*3+c) = -hess[l][c];
                        //transposed
                        A(jStart + j*3+c,iStart+i*3 + l) = -hess[l][c];
                    }
                }
            }//end copy
        }
    }

    if(mesh->numTangents > 0)
    {
        //BUG CORRECTED
        jStart = mesh->numPoints + mesh->numNormals*3;

        /************************************DOT************************************/
        for(int i = 0; i < mesh->numPoints;i++)
        {
            for(int j = 0; j < mesh->numTangents; j++)
            {
                double grad[3];
                //pj-vi
                rbf->gradient(&mesh->points[i*3],&mesh->tangentPoints[j*3],grad);
                //dot <tj,grad>
                double dot;
                double *tj = &mesh->tangents[j*3];
                dot = VecMath::dot(tj, grad);
                A(i, jStart + j) = -dot;

                A(jStart + j, i) = -dot;//transposed
            }
        }


        /**********************************DOT HESS**********************************/
        iStart = mesh->numPoints;
        jStart = mesh->numPoints + mesh->numNormals * 3;

        for(int i = 0; i < mesh->numNormals; i++)
        {
            for(int j  = 0; j < mesh->numTangents; j++)
            {
                double hess[3][3];
                rbf->hessian(&mesh->tangentPoints[j*3],&mesh->normalCenters[i*3],hess);
                double dot[3];
                double *tj = &mesh->tangents[j*3];
                VecMath::vetMac(tj,hess,dot);
                A(iStart + i*3  ,jStart + j) = - dot[0];
                A(iStart + i*3+1,jStart + j) = - dot[1];
                A(iStart + i*3+2,jStart + j) = - dot[2];

                //transposed
                A(jStart + j, iStart + i*3  ) = - dot[0];
                A(jStart + j, iStart + i*3+1) = - dot[1];
                A(jStart + j, iStart + i*3+2) = - dot[2];
            }
        }
        /**********************************DOT DOT***********************************/
        iStart = mesh->numPoints + mesh->numNormals * 3;
        jStart = iStart;


        for(int i = 0; i < mesh->numTangents; i++)
        {
            for(int j = i; j < mesh->numTangents; j++) //no kdtree
            {
                double hess[3][3];
                rbf->hessian(&mesh->tangentPoints[i*3],&mesh->tangentPoints[j*3],hess);
                double *ti = &mesh->tangents[i*3];
                double *tj = &mesh->tangents[j*3];
                double tjh[3];
                VecMath::vetMac(tj,hess,tjh);
                double dot = VecMath::dot(tjh,ti);
                A(iStart + i, jStart + j) = -dot;

                //transposed
                A(jStart + j, iStart + i) = -dot;
            }
        }
    }
    /************************************POLY************************************/

    std::cout << "POLY" << std::endl;
    if(usePoly)
    {
      //  std::cout << "polynomial" << std::endl;
        iStart = 0;
        jStart = mesh->numPoints + mesh->numNormals * 3 + mesh->numTangents;
        //poly
        for(int i = 0; i < mesh->numPoints; i++)
        {
            double *p = &mesh->points[i*3];
            A(i,jStart  ) =  1;
            A(i,jStart+1) = p[0];
            A(i,jStart+2) = p[1];
            A(i,jStart+3) = p[2];

            //transposed
            A(jStart  ,i) =  1;
            A(jStart+1,i) = p[0];
            A(jStart+2,i) = p[1];
            A(jStart+3,i) = p[2];
        }
        //grad poly
        iStart = mesh->numPoints;
        jStart = mesh->numPoints + mesh->numNormals * 3 + mesh->numTangents;
        for(int i = 0; i < mesh->numNormals; i++)
        {
            A(iStart + i*3  ,jStart+1) = 1;
            A(iStart + i*3+1,jStart+2) = 1;
            A(iStart + i*3+2,jStart+3) = 1;
            //transposed
            A(jStart+1, iStart + i*3  ) = 1;
            A(jStart+2, iStart + i*3+1) = 1;
            A(jStart+3, iStart + i*3+2) = 1;
        }
        //dot poly
        if(mesh->numTangents>0)
        {
          //  std::cout << "dot poly" << std::endl;
            iStart = mesh->numPoints + mesh->numNormals * 3;
            jStart = iStart + mesh->numTangents;
            for(int i = 0; i < mesh->numTangents; i++)
            {
                double *ti = &mesh->tangents[i*3];
                //A.insert(iStart + i, jStart  ) = 0;
                A(iStart + i, jStart+1) = ti[0];
                A(iStart + i, jStart+2) = ti[1];
                A(iStart + i, jStart+3) = ti[2];

                //transposed
                A(jStart+1, iStart + i) = ti[0];
                A(jStart+2, iStart + i) = ti[1];
                A(jStart+3, iStart + i) = ti[2];
            }
        }
    }

    std::cout << "matrix Size: " << matrixSize << std::endl;
    std::ofstream m("cube_next_vert_tan2.xls");
    for(int i = 0; i < matrixSize; i++)
    {
        for(int j = 0; j < matrixSize; j++)
        {
            m << A(i,j) << "\t";
        }
        m<<std::endl;
    }
    m.close();





    Eigen::VectorXd x(matrixSize);

    //gen b
    double *bb = new double[matrixSize];
    for(int i = 0; i < matrixSize; i++)

        bb[i]= 0;
    for(int i = 0; i < mesh->numNormals*3; i++)
         bb[mesh->numPoints+i] = mesh->normals[i];

    std::ofstream vb("b.txt");
    for(int i = 0; i < matrixSize; i++)
        vb << bb[i] << std::endl;
    vb.close();
  //  exit(9);

    Eigen::VectorXd b(matrixSize);
    for(int i = 0; i < matrixSize; i++)
        b(i)= 0;
    for(int i = 0; i < mesh->numNormals*3; i++)
        b(mesh->numPoints+i) = mesh->normals[i];



    //solve
    std::cout << "SOLVE" << std::endl;
    x = A.inverse() * b;


    double *coeffs = new double[matrixSize];

    for(int i = 0; i < matrixSize; i++)
    {
        coeffs[i] = x(i);
    //    std::cout << coeffs[i] << "\t";
    }
    std::cout << std::endl;
    //save coeffs.
    saveCoeffs(coeffsOut, matrixSize, coeffs);
}

void HBRBF::saveCoeffs(const char *file, int matrixSize, double * coefficients)
{

    std::ofstream outFile;
    outFile.open(file, std::ofstream::binary);
    //write data.
    //use poly?
   // std::cout << "WRITE: " << std::endl;
    bool b = usePoly;
    outFile.write((char*)&b,sizeof(bool));
    int dataDim = 3;
    outFile.write((char*)&dataDim,sizeof(int));

    //points
   // std::cout << "points" << std::endl;
    outFile.write((char*)&mesh->numPoints,sizeof(int));
    for(int i = 0; i< mesh->numPoints; i++)
    {
        outFile.write((char*)&mesh->points[i*3  ],sizeof(double));
        outFile.write((char*)&mesh->points[i*3+1],sizeof(double));
        outFile.write((char*)&mesh->points[i*3+2],sizeof(double));
    }

    //normals.
    outFile.write((char*)&mesh->numNormals,sizeof(int));
    for(int i = 0; i< mesh->numNormals; i++)
    {
        outFile.write((char*)&mesh->normals[i*3  ],sizeof(double));
        outFile.write((char*)&mesh->normals[i*3+1],sizeof(double));
        outFile.write((char*)&mesh->normals[i*3+2],sizeof(double));
    }

    //normal centers
    //outFile.write((char*)&mesh->numNormals,sizeof(int));
    for(int i = 0; i< mesh->numNormals; i++)
    {
        outFile.write((char*)&mesh->normalCenters[i*3  ],sizeof(double));
        outFile.write((char*)&mesh->normalCenters[i*3+1],sizeof(double));
        outFile.write((char*)&mesh->normalCenters[i*3+2],sizeof(double));
    }

    //tangents
    outFile.write((char*)&mesh->numTangents,sizeof(int));
    for(int i = 0; i< mesh->numTangents; i++)
    {
        outFile.write((char*)&mesh->tangents[i*3  ],sizeof(double));
        outFile.write((char*)&mesh->tangents[i*3+1],sizeof(double));
        outFile.write((char*)&mesh->tangents[i*3+2],sizeof(double));
    }

    //tangent points
   // outFile.write((char*)&mesh->numTangents,sizeof(int));
    for(int i = 0; i< mesh->numTangents; i++)
    {
        outFile.write((char*)&mesh->tangentPoints[i*3  ],sizeof(double));
        outFile.write((char*)&mesh->tangentPoints[i*3+1],sizeof(double));
        outFile.write((char*)&mesh->tangentPoints[i*3+2],sizeof(double));
    }


    //coeffs
    int s = matrixSize;
    outFile.write((char*)&s,sizeof(int));
    for(int i = 0; i < matrixSize; i++)
    {
        outFile.write((char*)&coefficients[i],sizeof(double));
    }

    outFile.write((char*)&rbf->radius,sizeof(double));

    //std::cout << "WRITEE" << std::endl;
    outFile.close();
}
