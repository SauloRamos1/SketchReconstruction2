#ifndef __TEXTUREWIDGET__
#define __TEXTUREWIDGET__

#include <GL/glew.h>
#include <tucano/effects/rendertexture.hpp>
#include <tucano/utils/imageIO.hpp>


class TextureWidget 
{
    
private:

    /// Render image effect (simply renders a texture)
    Tucano::Effects::RenderTexture rendertexture;

    /// Texture to hold input image
    Tucano::Texture image_texture;

    /// Dimensions
    Eigen::Vector2i size;


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

};

#endif // TEXTUREWIDGET
