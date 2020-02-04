#ifndef __GLWIDGET__
#define __GLWIDGET__

#include <GL/glew.h>

#include <tucano/utils/trackball.hpp>
#include <tucano/effects/ssao.hpp>
#include <tucano/effects/phongshader.hpp>
#include <tucano/effects/orennayar.hpp>
#include <tucano/effects/toon.hpp>
#include <tucano/utils/plyimporter.hpp>
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

    /**
     * @brief meshFile
     * Create the two private vars to handle
     * a Mesh File and Shaders Dir
     */
    string meshFile;
    string shader_dir;

    /// Phong effect to render mesh
    Tucano::Effects::Phong phong;

    /// Screen-Space Ambient Occlusion Effect
	Tucano::Effects::SSAO ssao;

    /// Simple Toon Shader
	Tucano::Effects::Toon toon;

    /// Oren Nayar BRDF
	Tucano::Effects::OrenNayar orennayar;

    /// ID of active effect
    int active_effect;

    /// Flag to draw or not trackball
    bool draw_trackball;

    /// GUI holder
    Tucano::GUI::Base gui;

    /// Reload button
    Tucano::GUI::Button reload_button;

	/// Select group for switching between effects
	Tucano::GUI::SelectGroup select_effect_group;

	/// Buttons to select effects
	Tucano::GUI::SelectButton phong_menu_button;
	Tucano::GUI::SelectButton orennayar_menu_button;
	Tucano::GUI::SelectButton toon_menu_button;
	Tucano::GUI::SelectButton ssao_menu_button;

	/// Boxes to group all gui elements per effect
	Tucano::GUI::GroupBox phong_groupbox;
	Tucano::GUI::GroupBox orennayar_groupbox;
	Tucano::GUI::GroupBox toon_groupbox;
	Tucano::GUI::GroupBox ssao_groupbox;

    /// Slider for phong coefficients
    Tucano::GUI::Slider kd_slider;
    Tucano::GUI::Slider ks_slider;
    Tucano::GUI::Slider ka_slider;
    Tucano::GUI::Slider shininess_slider;

    /// Label for phong coefficients
    Tucano::GUI::Label diffuse_label;
    Tucano::GUI::Label specular_label;
    Tucano::GUI::Label ambient_label;
    Tucano::GUI::Label shininess_label;

    /// Slider for toon quantization levels
    Tucano::GUI::Slider quantization_slider;
	Tucano::GUI::Label quantization_label;

	/// Slider for Oren Nayar roughness coefficient
	Tucano::GUI::Slider roughness_slider;
	Tucano::GUI::Label roughness_label;

	// UI for SSAO
	Tucano::GUI::Slider ssao_radius_slider;
	Tucano::GUI::Label ssao_radius_label;
	Tucano::GUI::Slider ssao_intensity_slider;
	Tucano::GUI::Label ssao_intensity_label;

public:
    
    ModelWidget(void) {}

    /**
     * @brief Initializes the widget and shaders
     * @param width Widget width in pixels
     * @param height Widget height in pixels 
     */
    void initialize(int width, int height);

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
     * Repaints screen buffer.
     */
    virtual void render();

    /**
     * @brief Reload effect shaders.
     */
    void reloadShaders (void)
    {
        ssao.reloadShaders();
        phong.reloadShaders();
        toon.reloadShaders();
        orennayar.reloadShaders();
    }

    /**
     * @brief Toggle draw trackball flag.
     */
    void toggleDrawTrackball (void)
    {
        draw_trackball = !draw_trackball;
    }

};

#endif // GLWIDGET
