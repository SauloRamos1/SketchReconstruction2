#ifndef __MODELWIDGET__
#define __MODELWIDGET__

#include <GL/glew.h>

#include <tucano/utils/trackball.hpp>
#include <tucano/effects/phongshader.hpp>
#include <tucano/utils/plyimporter.hpp>
#include <tucano/utils/imageIO.hpp>
#include <tucano/gui/base.hpp>


class ModelWidget 
{

private:

    /// Mesh to be rendered in this widget
    Tucano::Mesh mesh;

    /// Trackball for manipulating the camera
    Tucano::Trackball camera;

    /// Trackball for manipulating light direction
    Tucano::DirectionalTrackball light;

	string meshFile;
    string shader_dir;

    /// Phong effect to render mesh
    Tucano::Effects::Phong phong;

    /// GUI holder
    Tucano::GUI::Base gui;

	/// Box to group all gui elements
	Tucano::GUI::GroupBox groupbox;

    /// Reload button
    Tucano::GUI::Button reload_button;

	/// Menu show/hide button
	Tucano::GUI::SelectButton menu_button;

    /// Slider for diffuse coefficient
    Tucano::GUI::Slider kd_slider;
    Tucano::GUI::Slider ks_slider;
    Tucano::GUI::Slider ka_slider;
    Tucano::GUI::Slider shininess_slider;

    /// Label for diffuse text
    Tucano::GUI::Label diffuse_label;
    Tucano::GUI::Label specular_label;
    Tucano::GUI::Label ambient_label;
    Tucano::GUI::Label shininess_label;

public:

    ModelWidget(void) {}
    
    /**
     * @brief Initializes the widget and shaders
     * @param width Widget width in pixels
     * @param height Widget height in pixels 
     */
    void initialize(int width, int height);

    /**
     * Repaints screen buffer.
     **/
    virtual void render();

    /**
    * @brief Returns a pointer to the camera instance
    * @return pointer to trackball camera
    **/
    Tucano::Trackball* getCamera (void)
    {
        return &camera;
    }


    /**
     * @brief Returns pointer to GUI
     * @return pointer to GUI
     */
    Tucano::GUI::Base* getGUI (void)
    {
        return &gui;
    }

    /**
    * @brief Returns a pointer to the light instance
    * @return pointer to trackball light
    **/
    Tucano::DirectionalTrackball* getLight (void)
    {
        return &light;
    }

    /**
     * @brief setMeshFile
     * @param fn
     * New function to setup a meshFile var
     */
    void openMeshFile(string fn);

    /**
     * @brief setShaderDir
     * @param dir
     * New function to setup a shaderVar
     */

    void setShaderDir(string dir);

    /**
     * @brief Sets a texture for the model
     *
     * Mesh must have tex coords to work properly
     * @param tex_file Texture filename
     */
    void setModelTexture(std::string tex_file);

};

#endif // MODELWIDGET
