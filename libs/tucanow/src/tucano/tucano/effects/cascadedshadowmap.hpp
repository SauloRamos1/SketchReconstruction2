#ifndef __CASCADEDSHADOWMAP__
#define __CASCADEDSHADOWMAP__

#include <boundingbox.hpp>
#include <tucano/shapes/frustrum.hpp>
#include <tucano/shapes/box.hpp>

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a group of meshes through the Phong effect and a cascaded shadowmap 
 */
class CascadedShadowmap : public Tucano::Effect
{

private:
    int num_slices = 1;
    int focus_split = 0;

public:

    /**
     * @brief Default constructor.
     */
    CascadedShadowmap (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {

    }

    void setNumSlices(int n)
    {
        if(num_slices != n and n > 0)
        {
            num_slices = n;
            setFocusSplit(focus_split);
        }
    }

    void setFocusSplit(int focus)
    {
        if(focus < 0)
        {
            focus_split = 0;
        }
        else if(focus >= num_slices)
        {
            focus_split = num_slices - 1;
        }
        else
        {
            focus_split = focus;
        }
    }

    int getFocusSplit()
    {
        return focus_split;
    }

    int getNumSlices()
    {
        return num_slices;
    }

    Eigen::Matrix4f calculateCropMatrix(const Tucano::Camera& camera, const Tucano::Camera& light, float split_near_plane, float split_far_plane)
    {
        BoundingBox cropBB = createAABB(camera, light, split_near_plane, split_far_plane);

        cropBB.setMinZ(0.0);

        float scaleX, scaleY, scaleZ;  
        float offsetX, offsetY, offsetZ;  
        scaleX = 2.0f / (cropBB.getMaxX() - cropBB.getMinX());  
        scaleY = 2.0f / (cropBB.getMaxY() - cropBB.getMinY());  
        offsetX = -0.5f * (cropBB.getMaxX() + cropBB.getMinX()) * scaleX;  
        offsetY = -0.5f * (cropBB.getMaxY() + cropBB.getMinY()) * scaleY;  
        scaleZ = 1.0f / (cropBB.getMaxZ() - cropBB.getMinZ());  
        offsetZ = -cropBB.getMinZ() * scaleZ; 

        Eigen::Matrix4f m;

        m <<    scaleX   , 0.0       , 0.0       , offsetX,
                0.0      , scaleY    , 0.0       , offsetY,
                0.0      , 0.0       , scaleZ    , offsetZ,
                0.0      , 0.0       , 0.0       , 1.0;

        return m; 
    }

    BoundingBox createAABB(const Tucano::Camera& camera, const Tucano::Camera& light, float split_near_plane, float split_far_plane)
    {
        Eigen::Vector4f center(camera.getCenter()(0), camera.getCenter()(1), 0, 0);

        float fovy = camera.getFovy()*M_PI/180;

        float near_dy = split_near_plane*tan(fovy/2);
        float near_dx = split_near_plane*tan((fovy*camera.getViewportAspectRatio())/2);

        float far_dy = split_far_plane*tan(fovy/2);
        float far_dx = split_far_plane*tan((fovy*camera.getViewportAspectRatio())/2);

        vector<Eigen::Vector4f> frustrum(8);

        frustrum[0] = Eigen::Vector4f(center[0] - near_dx, center[1] + near_dy , -split_near_plane, 1);
        frustrum[1] = Eigen::Vector4f(center[0] + near_dx, center[1] + near_dy , -split_near_plane, 1);
        frustrum[2] = Eigen::Vector4f(center[0] - near_dx, center[1] - near_dy , -split_near_plane, 1);
        frustrum[3] = Eigen::Vector4f(center[0] + near_dx, center[1] - near_dy , -split_near_plane, 1);

        frustrum[4] = Eigen::Vector4f(center[0] - far_dx, center[1] + far_dy , -split_far_plane, 1);
        frustrum[5] = Eigen::Vector4f(center[0] + far_dx, center[1] + far_dy , -split_far_plane, 1);
        frustrum[6] = Eigen::Vector4f(center[0] - far_dx, center[1] - far_dy , -split_far_plane, 1);
        frustrum[7] = Eigen::Vector4f(center[0] + far_dx, center[1] - far_dy , -split_far_plane, 1);

        vector<Eigen::Vector4f> crop_light_frustrum(8);

        Eigen::Matrix4f crop_light_transformation = light.getProjectionMatrix()*light.getViewMatrix().matrix()*camera.getViewMatrix().inverse().matrix();
        for(int i=0; i<8; i++)
        {
            crop_light_frustrum[i] = crop_light_transformation*frustrum[i];
            crop_light_frustrum[i] = crop_light_frustrum[i]/crop_light_frustrum[i](3);
        }

        BoundingBox crop_bbox = calculateBoundingBox(crop_light_frustrum);

        return crop_bbox;
    }

    BoundingBox calculateBoundingBox(vector<Eigen::Vector4f> &frustrum)
    {
        BoundingBox boundingbox; 

        for(int i=0; i<3; i++)
        {
            float min = frustrum[0](i);
            for(int j=0; j<8; j++)
            {
                if(frustrum[j](i) < min){min = frustrum[j](i);}
            }
            boundingbox.setMin(i, min);
        }

        for(int i=0; i<3; i++)
        {
            float max = frustrum[0](i);
            for(int j=0; j<8; j++)
            {
                if(frustrum[j](i) > max){max = frustrum[j](i);}
            }
            boundingbox.setMax(i, max);
        }

        return boundingbox;
    }

    void renderFrustrum (const Tucano::Camera& camera, const Tucano::Camera& light)
    {
        renderFrustrum(camera, light, camera);
    }

    void renderFrustrum (const Tucano::Camera& camera, const Tucano::Camera& light, const Tucano::Camera& observer)
    {
        float split_size = (camera.getFarPlane() - camera.getNearPlane())/num_slices;
        float split_near_plane = camera.getNearPlane() + focus_split*split_size;
        float split_far_plane = camera.getNearPlane() + (focus_split + 1)*split_size;

        Tucano::Shapes::Frustrum split_frustrum(split_near_plane, split_far_plane, camera.getFovy(), camera.getViewportAspectRatio());
        split_frustrum.setModelMatrix(camera.getViewMatrix().inverse());
        split_frustrum.setColor(Eigen::Vector4f(0.0, 0.0, 1.0, 0.0));

        split_frustrum.render(observer, light);
    }

    void renderBBox(const Tucano::Camera& camera, const Tucano::Trackball& light, const Tucano::Camera& observer)
    {
        Eigen::Vector4f center(camera.getCenter()(0), camera.getCenter()(1), 0, 0);

        float split_size = (camera.getFarPlane() - camera.getNearPlane())/num_slices;
        float split_near_plane = camera.getNearPlane() + focus_split*split_size;
        float split_far_plane = camera.getNearPlane() + (focus_split + 1)*split_size;

        float fovy = camera.getFovy()*M_PI/180;

        float near_dy = split_near_plane*tan(fovy/2);
        float near_dx = split_near_plane*tan((fovy*camera.getViewportAspectRatio())/2);

        float far_dy = split_far_plane*tan(fovy/2);
        float far_dx = split_far_plane*tan((fovy*camera.getViewportAspectRatio())/2);

        vector<Eigen::Vector4f> frustrum(8);

        frustrum[0] = Eigen::Vector4f(center[0] - near_dx, center[1] + near_dy , -split_near_plane, 1);
        frustrum[1] = Eigen::Vector4f(center[0] + near_dx, center[1] + near_dy , -split_near_plane, 1);
        frustrum[2] = Eigen::Vector4f(center[0] - near_dx, center[1] - near_dy , -split_near_plane, 1);
        frustrum[3] = Eigen::Vector4f(center[0] + near_dx, center[1] - near_dy , -split_near_plane, 1);

        frustrum[4] = Eigen::Vector4f(center[0] - far_dx, center[1] + far_dy , -split_far_plane, 1);
        frustrum[5] = Eigen::Vector4f(center[0] + far_dx, center[1] + far_dy , -split_far_plane, 1);
        frustrum[6] = Eigen::Vector4f(center[0] - far_dx, center[1] - far_dy , -split_far_plane, 1);
        frustrum[7] = Eigen::Vector4f(center[0] + far_dx, center[1] - far_dy , -split_far_plane, 1);

        Eigen::Matrix4f draw_light_transformation = light.getViewMatrix().matrix()*camera.getViewMatrix().inverse().matrix();

        for(int i=0; i<8; i++)
        {
            frustrum[i] = draw_light_transformation*frustrum[i];
        }

        BoundingBox draw_bbox = calculateBoundingBox(frustrum);

        float width     = abs(draw_bbox.getMaxX() - draw_bbox.getMinX());
        float height    = abs(draw_bbox.getMaxY() - draw_bbox.getMinY());
        float depth     = abs(draw_bbox.getMaxZ() - draw_bbox.getMinZ());
        
        Tucano::Shapes::Box box(width, height, depth);
        Eigen::Affine3f translation(Eigen::Translation3f(   width/2 + draw_bbox.getMinX(),
                                                            height/2 + draw_bbox.getMinY(),
                                                            draw_bbox.getMaxZ() - depth/2));

        box.setModelMatrix(light.getViewMatrix().inverse()*translation);

        box.render(observer,light);
    }

    /** * @brief Render the meshes given a camera and light
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (const Tucano::Camera& camera, const Tucano::Camera& light, vector<Eigen::Matrix4f>& crop_vector)
    {
        // cascaded shadowmap
        float split_size = (camera.getFarPlane() - camera.getNearPlane())/num_slices;

        crop_vector.clear();
        for(int j=0; j<num_slices; j++)
        {
            crop_vector.push_back(calculateCropMatrix(  camera, light,    
                                                        camera.getNearPlane() + j*split_size,
                                                        camera.getNearPlane() + (j+1)*split_size)); 
        }
    }
};
}
}


#endif
