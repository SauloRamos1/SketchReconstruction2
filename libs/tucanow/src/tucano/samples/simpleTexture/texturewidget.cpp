#include "texturewidget.hpp"

void TextureWidget::initialize (int w, int h)
{
    rendertexture.initialize();

    size = Eigen::Vector2i (w, h);

    // initialize texture with given image
    Tucano::ImageImporter::loadImage("../samples/images/camelo.ppm", &image_texture);

    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
}


void TextureWidget::render (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // renders the given image, not that we are setting a fixed viewport that follows the widgets size
    // so it may not be scaled correctly with the image's size (just to keep the example simple)
    rendertexture.renderTexture(image_texture, size);
}
