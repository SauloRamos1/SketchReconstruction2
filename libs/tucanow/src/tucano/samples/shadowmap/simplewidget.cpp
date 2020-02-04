#include "simplewidget.hpp"

void ModelWidget::initialize (int width, int height)
{
    viewport = Eigen::Vector2i(width, height);
    float ratio = width/(float)height;

    observer_camera.setPerspectiveMatrix(60.0, ratio, 0.1f, 300.0f);
    observer_camera.setRenderFlag(true);
    observer_camera.setViewport(Eigen::Vector2f ((float)viewport(0), (float)viewport(1)));
    observer_camera.setDefaultTranslation (Eigen::Vector3f (0.0, 0.0, -5.0));

    camera.setPerspectiveMatrix(60.0, ratio, 0.1f, 150.0f);
    camera.setRenderFlag(true);
    camera.setViewport(Eigen::Vector2f ((float)viewport(0), (float)viewport(1)));
    camera.setDefaultTranslation (Eigen::Vector3f (0.0, 0.0, -20.0));

    //model positioning
    far_mesh.modelMatrix()->translate(Eigen::Vector3f(-3.0, 0.0, -50.0));
    middle_mesh.modelMatrix()->translate(Eigen::Vector3f(3.0, 0.0, -25.0));
    //near_mesh is not moved

    //light positioning
    light.setPerspectiveMatrix(60.0, ratio, 0.1f, 500.0f);
    light.setViewport(Eigen::Vector2f ((float)viewport(0), (float)viewport(1)));
    light.setDefaultTranslation (Eigen::Vector3f (0.0, 0.0, -50.0));

    //floor positioning
    floor = Tucano::Shapes::Box (100.0, 100.0, 0.2); 
    floor.modelMatrix()->rotate(Eigen::AngleAxisf(-0.5*M_PI, Eigen::Vector3f::UnitX()));
    floor.modelMatrix()->translate(Eigen::Vector3f(0.0, 45.0, -0.95));
    floor.setColor(Eigen::Vector4f(0.4, 0.75, 0.95, 1.0));

    // initialize shadowmap vector
    initializeShadowmaps();

    // Initialize Phong effect
    phongshadowmap.initialize();
    phongshadowmap.setMultisampling(false);

    // initialize the cascaded shadowmap effect
    cascadedshadowmap.initialize();

    //Inicialization and configuring of the menu
    gui.setViewportSize (viewport(0), viewport(1));

    menu_button.setPosition( 10, 10 );
    menu_button.onClick ( [&](){groupbox.toggleDisplay();} );
    menu_button.setTexture ( "../samples/assets/menu_button.pam" );
    menu_button.setDimensionsFromHeight(30);
    gui.add(&menu_button);

    int yoffset = 50;
    groupbox.setPosition (1, 1 + yoffset);
    groupbox.setDimensions (100, 260);
    groupbox.setTexture ("../samples/assets/groupbox.pam");
    gui.add(&groupbox);

    reload_button.setPosition( 15, 10 + yoffset );
    reload_button.onClick ( [&](){phongshadowmap.reloadShaders();} );
    reload_button.setTexture ( "../samples/assets/reload_button.pam" );
    reload_button.setDimensionsFromHeight(30);
    groupbox.add(&reload_button);

    shadowmap_button.setPosition( 15, 50 + yoffset );
    shadowmap_button.onClick ( [&](){switchShadowmaping();} );
    shadowmap_button.setTexture ( "../samples/assets/shadowmap_button.pam" );
    shadowmap_button.setDimensionsFromHeight(30);
    groupbox.add(&shadowmap_button);

    multisample_button.setPosition( 55, 50 + yoffset );
    multisample_button.onClick ( [&](){switchMultisampling();} );
    multisample_button.setTexture ( "../samples/assets/multisample_button.pam" );
    multisample_button.setDimensionsFromHeight(30);
    groupbox.add(&multisample_button);

    diffuse_label.setPosition(10, 90 + yoffset);
    diffuse_label.setTexture("../samples/assets/label_diffuse.pam");
    diffuse_label.setDimensionsFromHeight(12);
    groupbox.add(&diffuse_label);

    kd_slider.setPosition(10, 110 + yoffset);
    kd_slider.setDimensions(80, 10);
    kd_slider.onValueChanged( [&](float v){phongshadowmap.setDiffuseCoeff(v);} );
    kd_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    kd_slider.moveSlider(phongshadowmap.getDiffuseCoeff());
    groupbox.add(&kd_slider);

    specular_label.setPosition(10, 130 + yoffset);
    specular_label.setTexture("../samples/assets/label_specular.pam");
    specular_label.setDimensionsFromHeight(12);
    groupbox.add(&specular_label);

    ks_slider.setPosition(10, 150 + yoffset);
    ks_slider.setDimensions(80, 10);
    ks_slider.onValueChanged( [&](float v){phongshadowmap.setSpecularCoeff(v);} );
    ks_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ks_slider.moveSlider(phongshadowmap.getSpecularCoeff());
    groupbox.add(&ks_slider);

    shininess_label.setPosition(10, 170 + yoffset);
    shininess_label.setTexture("../samples/assets/label_shininess.pam");
    shininess_label.setDimensionsFromHeight(12);
    groupbox.add(&shininess_label);

    shininess_slider.setPosition(10, 190 + yoffset);
    shininess_slider.setDimensions(80, 10);
    shininess_slider.onValueChanged( [&](float v){phongshadowmap.setShininessCoeff(v);} );
    shininess_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    shininess_slider.setMinMaxValues(1.0, 100.0);
    shininess_slider.moveSlider(phongshadowmap.getShininessCoeff());
    groupbox.add(&shininess_slider);

    ambient_label.setPosition(10, 210 + yoffset);
    ambient_label.setTexture("../samples/assets/label_ambient.pam");
    ambient_label.setDimensionsFromHeight(12);
    groupbox.add(&ambient_label);

    ka_slider.setPosition(10, 230 + yoffset);
    ka_slider.setDimensions(80, 10);
    ka_slider.onValueChanged( [&](float v){phongshadowmap.setAmbientCoeff(v);} );
    ka_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ka_slider.moveSlider(phongshadowmap.getAmbientCoeff());
    groupbox.add(&ka_slider);
}

void ModelWidget::reloadShaders()
{
    cascadedshadowmap.reloadShaders();
    phongshadowmap.reloadShaders();
    
    initializeShadowmaps();
}

void ModelWidget::initializeShadowmaps()
{
    shadowmaps = std::vector<Tucano::Effects::ShadowMap>();
    for(int i=0; i<cascadedshadowmap.getNumSlices(); i++)
    {
        shadowmaps.push_back(Tucano::Effects::ShadowMap());

        shadowmaps.at(i).initialize();
        shadowmaps.at(i).setBufferSize(Eigen::Vector2i (viewport(0), viewport(1)));
        shadowmaps.at(i).setNumSamples(1);            
    }
}

void ModelWidget::openMeshFile(std::string fn)
{
    Tucano::MeshImporter::loadPlyFile(&near_mesh, fn);
    Tucano::MeshImporter::loadPlyFile(&middle_mesh, fn);
    Tucano::MeshImporter::loadPlyFile(&far_mesh, fn);
    near_mesh.normalizeModelMatrix();
    middle_mesh.normalizeModelMatrix();
    far_mesh.normalizeModelMatrix();
}

void ModelWidget::reset()
{
    observer_camera.reset();
    camera.reset();
    light.reset();

    phongshadowmap.setMultisampling(false);
    cascadedshadowmap.setNumSlices(1);

    reloadShaders();
}

void ModelWidget::setShaderDir(std::string dir)
{
    shader_dir = dir;
    phongshadowmap.setShadersDir(dir);
    for(unsigned int i=0; i<shadowmaps.size(); i++)
    {
        shadowmaps[i].setShadersDir(dir);
    }
    std::cout << dir << std::endl;
}

void ModelWidget::setCamera()
{
    camera.setViewMatrix(observer_camera.getViewMatrix());
}

void ModelWidget::addSlice()
{
    if(cascadedshadowmap.getNumSlices() < 8)
    {
        cascadedshadowmap.setNumSlices(cascadedshadowmap.getNumSlices() + 1);
        initializeShadowmaps();
    }
}

void ModelWidget::removeSlice()
{
    cascadedshadowmap.setNumSlices(cascadedshadowmap.getNumSlices() - 1);
    initializeShadowmaps();
}

void ModelWidget::nextFocusSplit()
{
    cascadedshadowmap.setFocusSplit(cascadedshadowmap.getFocusSplit() + 1);
}

void ModelWidget::previousFocusSplit()
{
    cascadedshadowmap.setFocusSplit(cascadedshadowmap.getFocusSplit() - 1);
}

void ModelWidget::switchShadowmaping()
{
    phongshadowmap.setShadowmapEnabled(!phongshadowmap.getShadowmapEnabled());
}

void ModelWidget::switchMultisampling()
{
    phongshadowmap.setMultisampling(!phongshadowmap.getMultisampling());
}

void ModelWidget::switchRenderBuffer()
{
    render_buffer = !render_buffer;
}

void ModelWidget::switchRenderFrustrum()
{
    render_frustrum = !render_frustrum;
}

void ModelWidget::switchRenderBBox()
{
    render_bbox = !render_bbox;
}

void ModelWidget::render (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    vector<Eigen::Matrix4f> crop_vector(cascadedshadowmap.getNumSlices());

    cascadedshadowmap.render(camera, light, crop_vector);

    // Clears and generates new shadowmaps
    for(unsigned int j=0; j<shadowmaps.size(); j++)
    {
        shadowmaps.at(j).clearShadowBuffer(); 

        shadowmaps.at(j).render(near_mesh   , camera, light, crop_vector[j]);
        shadowmaps.at(j).render(middle_mesh , camera, light, crop_vector[j]);
        shadowmaps.at(j).render(far_mesh    , camera, light, crop_vector[j]);
    }

    if(render_buffer)
    {
        shadowmaps.at(cascadedshadowmap.getFocusSplit()).renderBuffer(light);
    }
    else
    {
        if(render_frustrum)
        {
            cascadedshadowmap.renderFrustrum(camera, light, observer_camera);
        }
        if(render_bbox)
        {
            cascadedshadowmap.renderBBox(camera, light, observer_camera);
        }

        Tucano::Texture texture;
        texture.create(GL_TEXTURE_2D_ARRAY, GL_RGBA32F, viewport[0], viewport[1], GL_RGBA, GL_FLOAT, NULL, shadowmaps.size());

        for(unsigned int i=0; i<shadowmaps.size(); i++)
        {
            vector<float> aux_vec;
            shadowmaps[i].getShadowMap()->readBuffer(0, aux_vec);

            texture.update(aux_vec.data(), i);
        }

        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);

        phongshadowmap.render(near_mesh     , observer_camera, light, texture, crop_vector);
        phongshadowmap.render(middle_mesh   , observer_camera, light, texture, crop_vector);
        phongshadowmap.render(far_mesh      , observer_camera, light, texture, crop_vector);
        phongshadowmap.render(floor         , observer_camera, light, texture, crop_vector);

        light.renderDirection(observer_camera, true);
        observer_camera.render();
        gui.render();
    }
}
