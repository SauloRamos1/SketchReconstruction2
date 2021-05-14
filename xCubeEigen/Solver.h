#ifndef SOLVER_H
#define SOLVER_H

class Solver
{
    public:
    static double *gaussSeidel(double **A, double *b, int matrixSize, int maxIter, double minError);
    static double *jacobi();
};

#endif
