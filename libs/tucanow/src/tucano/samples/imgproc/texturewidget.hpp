#ifndef __TEXTUREWIDGET__
#define __TEXTUREWIDGET__

#include <GL/glew.h>
#include <tucano/effects/rendertexture.hpp>
#include <tucano/effects/gradientfilter.hpp>
#include <tucano/effects/meanfilter.hpp>
#include <tucano/utils/imageIO.hpp>
#include <tucano/gui/base.hpp>

class TextureWidget 
{
    
private:

    /// Render image effect (simply renders a texture)
    Tucano::Effects::RenderTexture rendertexture;

    /// Mean filter effect.
    Tucano::Effects::MeanFilter meanfilter;

    /// Gradient filter effect.
    Tucano::Effects::GradientFilter gradientfilter;

    /// Texture to hold input image
    Tucano::Texture image_texture;

    /// Dimensions
    Eigen::Vector2i size;

    /// FBO for multipass (enables applying more than one filter in sequence)
    Tucano::Framebuffer fbo;

    /// Flag to apply mean filter.
    bool apply_mean = false;

    /// Flag to apply horizontal gradient filter.
    bool apply_hgradient = false;

    /// Flag to apply vertical gradient filter.
    bool apply_vgradient = false;

    /// GUI holder
    Tucano::GUI::Base gui;

	/// Box to group all gui elements
	Tucano::GUI::GroupBox groupbox;

    /// Reload button
    Tucano::GUI::Button reload_button;

	/// Menu show/hide button
	Tucano::GUI::SelectButton menu_button;

    /// Slider for diffuse coefficient
    Tucano::GUI::Slider meanfilter_slider;

    /// Label for mean filter text
    Tucano::GUI::Label meanfilter_label;

    /// apply mean filter
    Tucano::GUI::SelectButton meanfilter_button;

    /// apply horizontal gradient (Sobel)
    Tucano::GUI::SelectButton hgrad_button;

    /// apply vertical gradient (Sobel)
    Tucano::GUI::SelectButton vgrad_button;
 

public:

    explicit TextureWidget(void) {}
    
    /**
     * @brief Initializes the shader effect
     * @param w Width
     * @param h Height
     */
    void initialize (int w, int h);

    /**
     * @brief Resize
     * @param w Width
     * @param h Height
     */
    void resize (int w, int h)
    {
        size = Eigen::Vector2i (w, h);
    }

    /**
     * @brief Repaints screen buffer.
     */
    virtual void render (void);

    /**
     * @brief Returns pointer to GUI
     * @return pointer to GUI
     */
    Tucano::GUI::Base* getGUI (void)
    {
        return &gui;
    }


    /**
     * @brief Toggles mean filter flag
     */
    void toggleMeanFilter (void)
    {
        apply_mean = !apply_mean;
    }

    /**
     * @brief Toggles horizontal gradient filter flag
     */
    void toggleHGradient (void)
    {
        apply_hgradient = !apply_hgradient;
        gradientfilter.setDirections (apply_hgradient, apply_vgradient);
   }

    /**
     * @brief Toggles vertical gradient filter flag
     */
    void toggleVGradient (void)
    {
        apply_vgradient = !apply_vgradient;
        gradientfilter.setDirections (apply_hgradient, apply_vgradient);

    }


};

#endif // TEXTUREWIDGET
