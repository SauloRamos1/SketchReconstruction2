#ifndef __TUCANOW_SPHERE__
#define __TUCANOW_SPHERE__


#include <vector>
#include <Eigen/Dense>


class Sphere {
    public:
        void setCenter( float x, float y, float z)
        {
            center_x = x;
            center_y = y;
            center_z = z;
        }

        bool setRadius( float r )
        {
            if ( r <= 0.f )
                return false;

            radius = r;

            return true;
        }

        void getMesh(std::vector<float> &vertices, std::vector<unsigned int> &faces, std::vector<float> &normals, size_t subdivisions = 4)
        {
            std::vector< Eigen::Vector4f > vert;

            vert.push_back ( Eigen::Vector4f( 1.0, 0.0, 0.0, 1.0) );
            vert.push_back ( Eigen::Vector4f(-1.0, 0.0, 0.0, 1.0) );
            vert.push_back ( Eigen::Vector4f( 0.0, 1.0, 0.0, 1.0) );
            vert.push_back ( Eigen::Vector4f( 0.0,-1.0, 0.0, 1.0) );
            vert.push_back ( Eigen::Vector4f( 0.0, 0.0, 1.0, 1.0) );
            vert.push_back ( Eigen::Vector4f( 0.0, 0.0,-1.0, 1.0) );

            /* int a[24] = { 0, 4, 2, 2, 4, 1, 1, 4, 3, 3, 4, 0, 0, 2, 5, 2, 1, 5, 1, 3, 5, 3, 0, 5 }; */
            /* faces.insert(faces.end(), a, a+24); */
            faces = { 0, 4, 2, 2, 4, 1, 1, 4, 3, 3, 4, 0, 0, 2, 5, 2, 1, 5, 1, 3, 5, 3, 0, 5 };

            // now subdivide, divide each triangle into 4
            for (unsigned int s = 0; s < subdivisions; ++s)
            {
                std::vector< unsigned int > sub_faces;
                for (unsigned int i = 0; i < (unsigned int)faces.size(); i+=3)
                {
                    Eigen::Vector4f p0 = vert[faces[i+0]];
                    Eigen::Vector4f p1 = vert[faces[i+1]];
                    Eigen::Vector4f p2 = vert[faces[i+2]];

                    Eigen::Vector4f p3 = (p0 + p1)*0.5;
                    Eigen::Vector4f p4 = (p0 + p2)*0.5;
                    Eigen::Vector4f p5 = (p1 + p2)*0.5;
                    p3.head(3).normalize();
                    p4.head(3).normalize();
                    p5.head(3).normalize();

                    vert.push_back(p3);
                    vert.push_back(p4);
                    vert.push_back(p5);

                    unsigned int ind = vert.size()-3;
                    // new faces are: (p0, p3, p4), (p4, p5, p2), (p3, p5, p4), (p3, p1, p5)
                    unsigned int b[12] = {
                        (unsigned int)faces[i+0], ind, ind+1, 
                        ind+1, ind+2, (unsigned int)faces[i+2], 
                        ind, ind+2, ind+1, 
                        ind, (unsigned int)faces[i+1], ind+2};
                    sub_faces.insert(sub_faces.end(), b, b+12);  
                }
                faces.clear();
                faces = sub_faces;
            }

            vertices.resize(3*vert.size());
            normals.resize(3*vert.size());

            /* std::cout << "\nSphere (" << center_x << ", " << center_y << ", " << center_z << "), r = " << radius << ": \n"; */

            for ( size_t i = 0; i < vert.size(); ++i )
            {
                vertices[3*i + 0] = center_x + radius*vert[i][0];
                vertices[3*i + 1] = center_y + radius*vert[i][1];
                vertices[3*i + 2] = center_z + radius*vert[i][2];

                /* std::cout << "( x = " << vertices[3*i + 0] << ", y = " << vertices[3*i + 1] << ", z = " << vertices[3*i + 2] << ")\n"; */

                normals[3*i + 0] = vert[i][0];
                normals[3*i + 1] = vert[i][1];
                normals[3*i + 2] = vert[i][2];
            }
        }

    private:
        float center_x = 0.f;
        float center_y = 0.f;
        float center_z = 0.f;
        float radius = 1.f;
};

#endif
