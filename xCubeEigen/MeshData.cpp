#include "MeshData.h"

MeshData::MeshData()
{
    numPoints = 0;
    points = NULL;

    numNormals = 0;
    normals = NULL;
    normalCenters = NULL;

    //TODO PUT TANGENTS HERE

}

MeshData::MeshData(QString const &file)
{
    //std::ifstream in(file);
    std:: ifstream in;
    in.open (file.toUtf8(),std::ifstream::in);
    if (!in.is_open()) {
        qWarning ("Cannot open file.");
        return;
    }

    if(in)
    {
        int dataDim;
        in >> dataDim;

        in >> numPoints;
        points = new double [numPoints * dataDim];
        for(int i = 0; i < numPoints; i++)
        {
            for(int j = 0; j < dataDim; j++)
                in >> points[i*dataDim+j];
        }

        in >> numNormals;
        normals         = new double[numNormals * dataDim];
        normalCenters   = new double[numNormals * dataDim];
        for(int i = 0; i < numNormals; i++)//for each normal
        {
            for(int j = 0; j<dataDim;j++)//for each point
                in>>normals[i*dataDim + j];

            double *n = &normals[i*3];

            VecMath::normalize(n);

            for(int j = 0; j<dataDim;j++)
                in>>normalCenters[i*dataDim + j];
        }

        in >> numTangents;

        tangents        = new double[numTangents * 3];
        tangentPoints   = new double[numTangents * 3];

        for(int i = 0; i < numTangents; i++)
        {
            for(int j = 0; j < dataDim; j++)
                in >> tangents[i * dataDim + j];
            double *t = &tangents[i*3];
            VecMath::normalize(t);

            for(int j = 0; j < dataDim; j++)
                in >> tangentPoints[i * dataDim + j];
        }

        in.close();

    }
    else
    {
        std::cout << "file error " << std::endl;
    }
}
