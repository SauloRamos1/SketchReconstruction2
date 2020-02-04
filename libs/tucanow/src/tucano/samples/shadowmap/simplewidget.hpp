#ifndef __MODELWIDGET__
#define __MODELWIDGET__

#include <GL/glew.h>
#include <tucano/utils/trackball.hpp>
#include <tucano/utils/plyimporter.hpp>
#include <tucano/utils/imageIO.hpp>
#include <tucano/gui/base.hpp>
#include <shadowmap.hpp>
#include <phongshadowmap.hpp>
#include <cascadedshadowmap.hpp>

class ModelWidget 
{

private:
    /// Meshes to be rendered in this widget
    Tucano::Mesh near_mesh;
    Tucano::Mesh middle_mesh;
    Tucano::Mesh far_mesh;

    // Planes to be rendered bellow and behind the model
    Tucano::Shapes::Box floor;

    /// Trackball for manipulating the camera
    Tucano::Trackball observer_camera;
    Tucano::Trackball camera;

    /// Trackball for manipulating light direction
    Tucano::DirectionalTrackball light;

    /**
     * @brief meshFile
     * Create the two private vars to handle
     * a Mesh File and Shaders Dir
     */
    string meshFile;
    string shader_dir;

    /// Cascaded shadowmap effect
    Tucano::Effects::CascadedShadowmap cascadedshadowmap;

    /// Phong shadowmap effect
    Tucano::Effects::PhongShadowmap phongshadowmap;

    /// Vector of shadowmaps
    std::vector<Tucano::Effects::ShadowMap> shadowmaps;

    /// GUI holder
    Tucano::GUI::Base gui;

	/// Box to group all gui elements
	Tucano::GUI::GroupBox groupbox;

    /// Reload button
    Tucano::GUI::Button reload_button;

    /// Shadowmap and Multisample buttons
    Tucano::GUI::Button shadowmap_button;
    Tucano::GUI::Button multisample_button;

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

    Eigen::Vector2i viewport;

    bool render_frustrum = false;
    bool render_bbox = false;
    bool render_buffer = false;

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
        return &observer_camera;
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

    void initializeShadowmaps();

    void reset();

    void reloadShaders();

    void setShaderDir(string dir);

    void setCamera();

    void addSlice();

    void removeSlice();

    void nextFocusSplit();

    void previousFocusSplit();

    void switchShadowmaping();

    void switchMultisampling();

    void switchRenderFrustrum();

    void switchRenderBBox();

    void switchRenderBuffer();
};

#endif // MODELWIDGET
