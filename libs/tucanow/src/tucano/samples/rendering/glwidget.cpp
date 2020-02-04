#include "glwidget.hpp"

void ModelWidget::initialize (int width, int height)
{
    // initialize the shader effect (if TUCANOSHADERDIR is set, no need to set dir before init)
    phong.initialize();
	ssao.initialize();
	toon.initialize();
	orennayar.initialize();

    mesh.setColor (Eigen::Vector4f(0.8, 0.3, 0.3, 1.0));

    camera.setPerspectiveMatrix(60.0, (float)width/(float)height, 0.1f, 100.0f);
    camera.setRenderFlag(true);
    camera.setViewport(Eigen::Vector2f ((float)width, (float)height));

    light.setRenderFlag(false);
    light.setViewport(Eigen::Vector2f ((float)width, (float)height));

    gui.setViewportSize (width, height);

	gui.add(&select_effect_group);

    phong_menu_button.setPosition( 1, 1 );
    phong_menu_button.onClick ( [&](){phong_groupbox.toggleDisplay(); active_effect=0;} );
    phong_menu_button.setTexture ( "../samples/assets/phong_button_alt.pam" );
    phong_menu_button.setAltTexture ( "../samples/assets/phong_button.pam" );
    phong_menu_button.setDimensionsFromHeight(30);
	select_effect_group.add(&phong_menu_button);

    orennayar_menu_button.setPosition( 11 + phong_menu_button.getDimensions()[0], 1 );
    orennayar_menu_button.onClick ( [&](){orennayar_groupbox.toggleDisplay(); active_effect=1;} );
	orennayar_menu_button.setTexture ( "../samples/assets/orennayar_button_alt.pam" );
	orennayar_menu_button.setAltTexture ( "../samples/assets/orennayar_button.pam" );
    orennayar_menu_button.setDimensionsFromHeight(30);
	select_effect_group.add(&orennayar_menu_button);

    toon_menu_button.setPosition( 21 + 2*phong_menu_button.getDimensions()[0], 1 );
    toon_menu_button.onClick ( [&](){toon_groupbox.toggleDisplay(); active_effect=2;} );
	toon_menu_button.setTexture ( "../samples/assets/toon_button_alt.pam" );
	toon_menu_button.setAltTexture ( "../samples/assets/toon_button.pam" );
    toon_menu_button.setDimensionsFromHeight(30);
	select_effect_group.add(&toon_menu_button);

    ssao_menu_button.setPosition( 31 + 3*phong_menu_button.getDimensions()[0], 1 );
    ssao_menu_button.onClick ( [&](){ssao_groupbox.toggleDisplay(); active_effect=3;} );
	ssao_menu_button.setTexture ( "../samples/assets/ssao_button_alt.pam" );
	ssao_menu_button.setAltTexture ( "../samples/assets/ssao_button.pam" );
    ssao_menu_button.setDimensionsFromHeight(30);
	select_effect_group.add(&ssao_menu_button);

    // SSAO not working very well
    ssao_menu_button.hide();

    int yoffset = 65;

	/*** PHONG ***/

	phong_groupbox.setPosition (1, 1 + yoffset);
	phong_groupbox.setDimensions (100, 220);
	phong_groupbox.setTexture ("../samples/assets/groupbox.pam");
	phong_groupbox.hide();
	gui.add(&phong_groupbox);

    diffuse_label.setPosition(10, 20 + yoffset);
    diffuse_label.setTexture("../samples/assets/label_diffuse.pam");
    diffuse_label.setDimensionsFromHeight(12);
    phong_groupbox.add(&diffuse_label);

    kd_slider.setPosition(10, 40 + yoffset);
    kd_slider.setDimensions(80, 10);
    kd_slider.onValueChanged( [&](float v){phong.setDiffuseCoeff(v);} );
    kd_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    kd_slider.moveSlider(phong.getDiffuseCoeff());
    phong_groupbox.add(&kd_slider);

    specular_label.setPosition(10, 60 + yoffset);
    specular_label.setTexture("../samples/assets/label_specular.pam");
    specular_label.setDimensionsFromHeight(12);
    phong_groupbox.add(&specular_label);

    ks_slider.setPosition(10, 80 + yoffset);
    ks_slider.setDimensions(80, 10);
    ks_slider.onValueChanged( [&](float v){phong.setSpecularCoeff(v);} );
    ks_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ks_slider.moveSlider(phong.getSpecularCoeff());
    phong_groupbox.add(&ks_slider);

    shininess_label.setPosition(10, 100 + yoffset);
    shininess_label.setTexture("../samples/assets/label_shininess.pam");
    shininess_label.setDimensionsFromHeight(12);
    phong_groupbox.add(&shininess_label);

    shininess_slider.setPosition(10, 120 + yoffset);
    shininess_slider.setDimensions(80, 10);
    shininess_slider.onValueChanged( [&](float v){phong.setShininessCoeff(v);} );
    shininess_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    shininess_slider.setMinMaxValues(1.0, 100.0);
    shininess_slider.moveSlider(phong.getShininessCoeff());
    phong_groupbox.add(&shininess_slider);

    ambient_label.setPosition(10, 140 + yoffset);
    ambient_label.setTexture("../samples/assets/label_ambient.pam");
    ambient_label.setDimensionsFromHeight(12);
    phong_groupbox.add(&ambient_label);

    ka_slider.setPosition(10, 160 + yoffset);
    ka_slider.setDimensions(80, 10);
    ka_slider.onValueChanged( [&](float v){phong.setAmbientCoeff(v);} );
    ka_slider.setTexture("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ka_slider.moveSlider(phong.getAmbientCoeff());
    phong_groupbox.add(&ka_slider);

	/*** OREN-NAYAR ***/

	orennayar_groupbox.setPosition (1, 1 + yoffset);
	orennayar_groupbox.setDimensions (100, 220);
	orennayar_groupbox.setTexture ("../samples/assets/groupbox.pam");
	orennayar_groupbox.hide();
	gui.add(&orennayar_groupbox);

	roughness_label.setPosition(10, 20 + yoffset);
	roughness_label.setTexture("../samples/assets/label_roughness.pam");
    roughness_label.setDimensionsFromHeight(12);
    orennayar_groupbox.add(&roughness_label);

	roughness_slider.setPosition(10, 50 + yoffset);
	roughness_slider.setDimensions (80, 10);
    roughness_slider.onValueChanged( [&](float v){orennayar.setRoughnessCoeff(v);} );
	roughness_slider.setTexture ("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    roughness_slider.setMinMaxValues(0.0, 2.0);
	roughness_slider.moveSlider(orennayar.getRoughnessCoeff());
	orennayar_groupbox.add(&roughness_slider);

	/*** TOON ***/

	toon_groupbox.setPosition (1, 1 + yoffset);
	toon_groupbox.setDimensions (100, 220);
	toon_groupbox.setTexture ("../samples/assets/groupbox.pam");
	toon_groupbox.hide();
	gui.add(&toon_groupbox);

	quantization_label.setPosition(10, 20 + yoffset);
	quantization_label.setTexture("../samples/assets/label_quantization.pam");
    quantization_label.setDimensionsFromHeight(25);
    toon_groupbox.add(&quantization_label);

	quantization_slider.setPosition(10, 50 + yoffset);
	quantization_slider.setDimensions (80, 10);
    quantization_slider.onValueChanged( [&](int v){toon.setQuantizationLevel(v);} );
	quantization_slider.setTexture ("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    quantization_slider.setMinMaxValues(1.0, 10.0);
	quantization_slider.moveSlider(toon.getQuantizationLevel());
	toon_groupbox.add(&quantization_slider);

	/*** SSAO ***/

	ssao_groupbox.setPosition (1, 1 + yoffset);
	ssao_groupbox.setDimensions (100, 220);
	ssao_groupbox.setTexture ("../samples/assets/groupbox.pam");
	ssao_groupbox.hide();
	gui.add(&ssao_groupbox);

	ssao_radius_label.setPosition(10, 20 + yoffset);
	ssao_radius_label.setTexture("../samples/assets/label_radius.pam");
	ssao_radius_label.setDimensionsFromHeight(12);
	ssao_groupbox.add(&ssao_radius_label);

	ssao_radius_slider.setPosition(10, 50 + yoffset);
	ssao_radius_slider.setDimensions (80, 10);
    ssao_radius_slider.onValueChanged( [&](int v){ssao.setRadius(v);} );
	ssao_radius_slider.setTexture ("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ssao_radius_slider.setMinMaxValues(1.0, 15.0);
	ssao_radius_slider.moveSlider(ssao.getRadius());
	ssao_groupbox.add(&ssao_radius_slider);

	ssao_intensity_label.setPosition(10, 70 + yoffset);
	ssao_intensity_label.setTexture("../samples/assets/label_intensity.pam");
	ssao_intensity_label.setDimensionsFromHeight(12);
	ssao_groupbox.add(&ssao_intensity_label);

	ssao_intensity_slider.setPosition(10, 90 + yoffset);
	ssao_intensity_slider.setDimensions (80, 10);
    ssao_intensity_slider.onValueChanged( [&](int v){ssao.setIntensity(v);} );
	ssao_intensity_slider.setTexture ("../samples/assets/slider_bar.pam", "../samples/assets/slider.pam");
    ssao_intensity_slider.setMinMaxValues(1.0, 15.0);
	ssao_intensity_slider.moveSlider(ssao.getIntensity());
	ssao_groupbox.add(&ssao_intensity_slider);

    reload_button.setPosition(10, yoffset + 230 );
    reload_button.onClick ( [&](){phong.reloadShaders(); orennayar.reloadShaders(); toon.reloadShaders(); ssao.reloadShaders();} );
    reload_button.setTexture ( "../samples/assets/reload_shaders_button.pam" );
    reload_button.setDimensionsFromHeight(20);
    gui.add(&reload_button);


	select_effect_group.resetSelection(); // selected first element of group

}

void ModelWidget::render (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

   	if (active_effect == 0)
	{
		phong.render(mesh, camera, light);
	}
	if (active_effect == 1)
	{
        orennayar.render(mesh, camera, light);
    }
    if (active_effect == 2)
    {
		toon.render(mesh, camera, light);
	}
	if (active_effect == 3)
	{
		ssao.render(mesh, camera, light);
	}

    light.renderDirection(camera, true);
    camera.render();
    gui.render();
}

void ModelWidget::openMeshFile(std::string fn)
{
    Tucano::MeshImporter::loadPlyFile(&mesh, fn);
    mesh.normalizeModelMatrix();
}
