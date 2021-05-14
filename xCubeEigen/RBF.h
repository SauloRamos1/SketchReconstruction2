#ifndef RBF_H
#define RBF_H
class RBF
{
    public:
    virtual void computePhi(double *center, double *vertex, double *ret){}

    virtual void gradient(double *center, double *vertex, double grad[3]){}

    virtual void hessian(double *center, double *vetex, double hess[3][3]){}

    virtual void eval_hessian(double center[3], double *vertex,double phessian[3], double r[3])
    {
        double h[3][3];
        hessian(center,vertex,h);
        r[0] = 0; r[1] = 0; r[2] = 0;

        r[0] = h[0][0]*phessian[0] + h[0][1]*phessian[1] + h[0][2]*phessian[2];
        r[1] = h[1][0]*phessian[0] + h[1][1]*phessian[1] + h[1][2]*phessian[2];
        r[2] = h[2][0]*phessian[0] + h[2][1]*phessian[1] + h[2][2]*phessian[2];
    };
    double radius;
};

#endif // RBF_H
