#ifndef __FLYSCENE__
#define __FLYSCENE__

#include <GL/glew.h>

#include <tucano/effects/phongshader.hpp>
#include <tucano/utils/flycamera.hpp>
#include <tucano/utils/path.hpp>
#include <tucano/shapes/camerarep.hpp>
#include <tucano/mesh.hpp>
#include <tucano/utils/objimporter.hpp>

class Flyscene 
{

public:

    Flyscene(void){}
    
    /**
     * @brief Initializes the shader effect
	 * @param width Window width in pixels
	 * @param height Window height in pixels
     */
    void initialize(int width, int height);

    /**
     * Repaints screen buffer.
     **/
    virtual void paintGL();

	/**
	* Returns the pointer to the flycamera instance
	* @return pointer to flycamera
	**/
    Tucano::Flycamera* getCamera(void)
	{
		return &flycamera;
	}

	/**
	* Returns a pointer to the camerapath instance
	* @return pointer to camerapath
	*/
    Tucano::Path* cameraPath(void)
	{
		return &camerapath;
	}

	void addKeyPoint (void);

	void switchFlyCamera (void)
	{
		camera_type = 1;
	}

	void switchPathCamera (void)
	{
		camera_type = 2;
	}

private:

	// A simple phong shader for rendering meshes
    Tucano::Effects::Phong phong;

	// A fly through camera
    Tucano::Flycamera flycamera;

	// A camera path for smooth camera animation
	Tucano::Path camerapath;

	// A camera representation for animating path
	Tucano::Shapes::CameraRep camerarep;

	// Light represented as a camera
	Tucano::Camera light;

	// Path camera
	Tucano::Camera follow_cam;

	// Camera type to use for rendering (1=FlyCamera, 2=PathCamera)
	int camera_type = 1;
	
	// A mesh
    Tucano::Mesh mesh;
};

#endif // FLYSCENE
