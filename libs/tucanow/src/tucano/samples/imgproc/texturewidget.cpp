#include "texturewidget.hpp"

void TextureWidget::initialize (int w, int h)
{    
    // initialize the shader effects
    meanfilter.initialize();
    gradientfilter.initialize();
    rendertexture.initialize();

    // initialize the FBO and filles last attachment with original image
    // the other two attachments are used for multipass
    fbo.create(w, h, 3);
    // initialize texture with given image
    Tucano::ImageImporter::loadImage("../samples/images/camelo.ppm", fbo.getTexture(2));

    size = Eigen::Vector2i (w, h);
    gui.setViewportSize (w, h);

    menu_button.setPosition( 10, 10 );
    menu_button.onClick ( [&](){groupbox.toggleDisplay();} );
    menu_button.setTexture ( "../samples/assets/menu_button.pam" );
    menu_button.setDimensionsFromHeight(30);
    gui.add(&menu_button);

    int yoffset = 50;
	groupbox.setPosition (1, 1 + yoffset);
	groupbox.setDimensions (100, 220);
	groupbox.setTexture ("../samples/assets/groupbox.pam");
	gui.add(&groupbox);

    reload_button.setPosition( 10, 10 + yoffset );
    reload_button.onClick ( [&](){meanfilter.reloadShaders();} );
    reload_button.setTexture ( "../samples/assets/reload_button.pam" );
    reload_button.setDimensionsFromHeight(30);
    groupbox.add(&reload_button);

    meanfilter_button.setPosition(10, 50 + yoffset);
    meanfilter_button.onClick( [&](){this->toggleMeanFilter();} );
    meanfilter_button.setTexture("../samples/assets/meanfilter_button.pam");
    meanfilter_button.setDimensionsFromHeight(20);
    groupbox.add(&meanfilter_button);

    meanfilter_slider.setPosition(10, 80 + yoffset);
    meanfilter_slider.setDimensions(80, 10);
    meanfilter_slider.onValueChanged( [&](float v){meanfilter.setKernel(v);} );
    meanfilter_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    meanfilter_slider.setMinMaxValues(1.0, 25.0);
    meanfilter_slider.moveSlider(meanfilter.getKernelSize());
    groupbox.add(&meanfilter_slider);

    vgrad_button.setPosition(10, 100 + yoffset);
    vgrad_button.onClick( [&](){this->toggleVGradient();} );
    vgrad_button.setTexture("../samples/assets/vgrad_button.pam");
    vgrad_button.setDimensionsFromHeight(20);
    groupbox.add(&vgrad_button);

    hgrad_button.setPosition(10, 130 + yoffset);
    hgrad_button.onClick( [&](){this->toggleHGradient();} );
    hgrad_button.setTexture("../samples/assets/hgrad_button.pam");
    hgrad_button.setDimensionsFromHeight(20);
    groupbox.add(&hgrad_button);


    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
}


void TextureWidget::render (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    Eigen::Vector2i viewport = size ;

    // sets the read buffer as attachment 2 (original image)
    int write_tex = 0;
    int read_tex = 2;

    // for each filter, switches between read/write buffers
    // note that any number of filters can be attached to this example
	
	fbo.clearAttachment (0);
	fbo.clearAttachment (1);

    if (apply_hgradient || apply_vgradient)
    {
        fbo.bindRenderBuffer(write_tex);
        gradientfilter.renderTexture(*fbo.getTexture(read_tex), viewport);
        fbo.unbind();
        read_tex = write_tex;
        write_tex = !write_tex;
		fbo.clearAttachment(write_tex);
    }
    if (apply_mean)
    {
        fbo.bindRenderBuffer(write_tex);
        meanfilter.renderTexture(*fbo.getTexture(read_tex), viewport);
        fbo.unbind();		
        read_tex = write_tex;
        write_tex = !write_tex;
		fbo.clearAttachment(write_tex);
    }

    // renders the resulting image (or original image if no filter was applied)
    rendertexture.renderTexture(*fbo.getTexture(read_tex), viewport);
    gui.render();
}
