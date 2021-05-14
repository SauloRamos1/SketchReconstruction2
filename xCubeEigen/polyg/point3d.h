#ifndef __POINT3D_H__

    #define __POINT3D_H__

    struct Vec3 {
        double c[3];
        Vec3(double x = 0, double y = 0, double z = 0) { c[0] = x; c[1] = y; c[2] = z; }
        double& operator[](int i) { return c[i]; }
        double operator[](int i) const { return c[i]; }
    };

    class Point3D {
	public:
		double c[3];
        Point3D(double x = 0, double y = 0, double z = 0) { c[0] = x; c[1] = y; c[2] = z; }
        double& X(void) { return c[0]; }
        double  X(void) const { return c[0]; }
        double& Y(void) { return c[1]; }
        double  Y(void) const { return c[1]; }
        double& Z(void) { return c[2]; }
        double  Z(void) const { return c[2]; }
        double& operator[](int i) { return c[i]; }
        double operator[](int i) const { return c[i]; }
    };

#endif // __POINT3D_H__

