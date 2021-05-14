#include "Solver.h"

double *Solver::gaussSeidel(double **A, double *b, int matrixSize, int maxIter, double minError)
{
    double *x = new double[matrixSize];
    double error = 0;

    for(int i = 0; i < maxIter; i++)
    {


        if(error < minError)
        {
            return &error;
        }
    }
    return x;
}
