#include "simplewidget.hpp"

void ModelWidget::initialize (int width, int height)
{
    // initialize the shader effect (if TUCANOSHADERDIR is set, no need to set dir before init)
    phong.initialize();

    camera.setPerspectiveMatrix(60.0, (float)width/(float)height, 0.1f, 100.0f);
    camera.setRenderFlag(true);
    camera.setViewport(Eigen::Vector2f ((float)width, (float)height));

    light.setRenderFlag(false);
    light.setViewport(Eigen::Vector2f ((float)width, (float)height));

    gui.setViewportSize (width, height);

    menu_button.setPosition( 10, 10 );
    menu_button.onClick ( [&](){groupbox.toggleDisplay();} );
    menu_button.setTexture ( "../samples/assets/menu_button.pam" );
    //menu_button.setHoverTexture ( "../samples/assets/reload_button.pam" );
    menu_button.setDimensionsFromHeight(30);
    gui.add(&menu_button);

    int yoffset = 50;
	groupbox.setPosition (1, 1 + yoffset);
	groupbox.setDimensions (100, 220);
	groupbox.setTexture ("../samples/assets/groupbox.pam");
	gui.add(&groupbox);

    reload_button.setPosition( 10, 10 + yoffset );
    reload_button.onClick ( [&](){phong.reloadShaders();} );
    reload_button.setTexture ( "../samples/assets/reload_button.pam" );
    reload_button.setDimensionsFromHeight(30);
    groupbox.add(&reload_button);

    diffuse_label.setPosition(10, 50 + yoffset);
    diffuse_label.setTexture("../samples/assets/label_diffuse.pam");
    diffuse_label.setDimensionsFromHeight(12);
    groupbox.add(&diffuse_label);

    kd_slider.setPosition(10, 70 + yoffset);
    kd_slider.setDimensions(80, 10);
    kd_slider.onValueChanged( [&](float v){phong.setDiffuseCoeff(v);} );
    kd_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    kd_slider.moveSlider(phong.getDiffuseCoeff());
    groupbox.add(&kd_slider);

    specular_label.setPosition(10, 90 + yoffset);
    specular_label.setTexture("../samples/assets/label_specular.pam");
    specular_label.setDimensionsFromHeight(12);
    groupbox.add(&specular_label);

    ks_slider.setPosition(10, 110 + yoffset);
    ks_slider.setDimensions(80, 10);
    ks_slider.onValueChanged( [&](float v){phong.setSpecularCoeff(v);} );
    ks_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ks_slider.moveSlider(phong.getSpecularCoeff());
    groupbox.add(&ks_slider);

    shininess_label.setPosition(10, 130 + yoffset);
    shininess_label.setTexture("../samples/assets/label_shininess.pam");
    shininess_label.setDimensionsFromHeight(12);
    groupbox.add(&shininess_label);

    shininess_slider.setPosition(10, 150 + yoffset);
    shininess_slider.setDimensions(80, 10);
    shininess_slider.onValueChanged( [&](float v){phong.setShininessCoeff(v);} );
    shininess_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    shininess_slider.setMinMaxValues(1.0, 100.0);
    shininess_slider.moveSlider(phong.getShininessCoeff());
    groupbox.add(&shininess_slider);

    ambient_label.setPosition(10, 170 + yoffset);
    ambient_label.setTexture("../samples/assets/label_ambient.pam");
    ambient_label.setDimensionsFromHeight(12);
    groupbox.add(&ambient_label);

    ka_slider.setPosition(10, 190 + yoffset);
    ka_slider.setDimensions(80, 10);
    ka_slider.onValueChanged( [&](float v){phong.setAmbientCoeff(v);} );
    ka_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ka_slider.moveSlider(phong.getAmbientCoeff());
    groupbox.add(&ka_slider);

    glEnable(GL_DEPTH_TEST);
}

void ModelWidget::render (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    phong.render(mesh, camera, light);
    camera.render();
    gui.render();
}

void ModelWidget::setShaderDir(std::string dir)
{
    shader_dir = dir;
}

void ModelWidget::openMeshFile(std::string fn)
{
    if (Tucano::MeshImporter::loadPlyFile(&mesh, fn))
    {
        mesh.normalizeModelMatrix();
        std::string tex_file = Tucano::MeshImporter::getPlyTextureFile(fn);
        if (!tex_file.empty())
        {
            size_t found = fn.find_last_of("/\\");
            string dir = fn.substr(0, found);
            string tex_file_with_dir = dir + "/" + tex_file;
 
            setModelTexture(tex_file_with_dir);
        }
    }
}
void ModelWidget::setModelTexture(std::string tex_file)
{
    Tucano::Texture texture;
    if (Tucano::ImageImporter::loadImage(tex_file, &texture))
    {        
        phong.setTexture(texture);
        phong.getTexture()->setTexParameters( GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR );
    }
}

