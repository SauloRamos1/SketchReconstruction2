#include <iostream>

#include "gui_impl.hpp"
#include "scene_impl.hpp"
#include "tucanow/gui.hpp"


namespace tucanow {


Gui::Gui(Scene &s) : scene(s), pimpl(new GuiImpl()) {}

Gui::~Gui() = default;


void Gui::initialize(int width, int height, std::string assets_dir)
{
    setViewport(width, height);
    auto gui = getTucanoGui();

    pimpl->menu_button.setPosition( 10, 10 );
    pimpl->menu_button.onClick ( [&](){pimpl->groupbox.toggleDisplay();} );
    pimpl->menu_button.setTexture ( assets_dir + "menu_button.pam" );
    //menu_button.setHoverTexture ( assets_dir + "reload_button.pam" );
    pimpl->menu_button.setDimensionsFromHeight(30);
    gui->add(&pimpl->menu_button);

    int yoffset = 50;
    pimpl->groupbox.setPosition (1, 1 + yoffset);
    pimpl->groupbox.setDimensions (100, 220);
    pimpl->groupbox.setTexture (assets_dir + "groupbox.pam");
    gui->add(&pimpl->groupbox);

    pimpl->reload_button.setPosition( 10, 10 + yoffset );
    pimpl->reload_button.onClick ( [&](){scene.pimpl->phong.reloadShaders();} );
    pimpl->reload_button.setTexture ( assets_dir + "reload_button.pam" );
    pimpl->reload_button.setDimensionsFromHeight(30);
    pimpl->groupbox.add(&pimpl->reload_button);

    pimpl->diffuse_label.setPosition(10, 50 + yoffset);
    pimpl->diffuse_label.setTexture(assets_dir + "label_diffuse.pam");
    pimpl->diffuse_label.setDimensionsFromHeight(12);
    pimpl->groupbox.add(&pimpl->diffuse_label);

    pimpl->kd_slider.setPosition(10, 70 + yoffset);
    pimpl->kd_slider.setDimensions(80, 10);
    pimpl->kd_slider.onValueChanged( [&](float v){scene.pimpl->phong.setDiffuseCoeff(v); std::cout << "DiffuseCoeff: " << v <<"\n";} );
    pimpl->kd_slider.setTexture(assets_dir + "slider_bar.pam", assets_dir + "slider.pam");
    pimpl->kd_slider.moveSlider(scene.pimpl->phong.getDiffuseCoeff());
    pimpl->groupbox.add(&pimpl->kd_slider);

    pimpl->specular_label.setPosition(10, 90 + yoffset);
    pimpl->specular_label.setTexture(assets_dir + "label_specular.pam");
    pimpl->specular_label.setDimensionsFromHeight(12);
    pimpl->groupbox.add(&pimpl->specular_label);

    pimpl->ks_slider.setPosition(10, 110 + yoffset);
    pimpl->ks_slider.setDimensions(80, 10);
    pimpl->ks_slider.onValueChanged( [&](float v){scene.pimpl->phong.setSpecularCoeff(v); std::cout << "SpecularCoeff: " << v <<"\n";} );
    pimpl->ks_slider.setTexture(assets_dir + "slider_bar.pam", assets_dir + "slider.pam");
    pimpl->ks_slider.moveSlider(scene.pimpl->phong.getSpecularCoeff());
    pimpl->groupbox.add(&pimpl->ks_slider);

    pimpl->shininess_label.setPosition(10, 130 + yoffset);
    pimpl->shininess_label.setTexture(assets_dir + "label_shininess.pam");
    pimpl->shininess_label.setDimensionsFromHeight(12);
    pimpl->groupbox.add(&pimpl->shininess_label);

    pimpl->shininess_slider.setPosition(10, 150 + yoffset);
    pimpl->shininess_slider.setDimensions(80, 10);
    pimpl->shininess_slider.onValueChanged( [&](float v){scene.pimpl->phong.setShininessCoeff(v); std::cout << "ShininessCoeff: " << v <<"\n";} );
    pimpl->shininess_slider.setTexture(assets_dir + "slider_bar.pam", assets_dir + "slider.pam");
    pimpl->shininess_slider.setMinMaxValues(1.0, 100.0);
    pimpl->shininess_slider.moveSlider(scene.pimpl->phong.getShininessCoeff());
    pimpl->groupbox.add(&pimpl->shininess_slider);

    pimpl->ambient_label.setPosition(10, 170 + yoffset);
    pimpl->ambient_label.setTexture(assets_dir + "label_ambient.pam");
    pimpl->ambient_label.setDimensionsFromHeight(12);
    pimpl->groupbox.add(&pimpl->ambient_label);

    pimpl->ka_slider.setPosition(10, 190 + yoffset);
    pimpl->ka_slider.setDimensions(80, 10);
    pimpl->ka_slider.onValueChanged( [&](float v){scene.pimpl->phong.setAmbientCoeff(v); std::cout << "AmbientCoeff: " << v <<"\n";} );
    pimpl->ka_slider.setTexture(assets_dir + "slider_bar.pam", assets_dir + "slider.pam");
    pimpl->ka_slider.moveSlider(scene.pimpl->phong.getAmbientCoeff());
    pimpl->groupbox.add(&pimpl->ka_slider);
}

bool Gui::setViewport(int width, int height)
{
    if ( ( width < 1 ) || ( height < 1 ) )
    {
        return false;
    }

    auto gui = getTucanoGui();
    gui->setViewportSize (width, height);

    scene.setViewport(width, height);

    return true;
}

void Gui::render()
{
    auto gui = getTucanoGui();
    gui->render();
}

bool Gui::leftButtonPressed(float xpos, float ypos)
{
    float scale_width, scale_height;
    scene.getScreenScale(scale_width, scale_height);

    double scaled_xpos = scale_width * xpos;
    double scaled_ypos = scale_height * ypos;

    auto gui = getTucanoGui();
    return gui->leftButtonPressed (scaled_xpos, scaled_ypos);
}

bool Gui::leftButtonReleased(float xpos, float ypos)
{
    float scale_width, scale_height;
    scene.getScreenScale(scale_width, scale_height);

    double scaled_xpos = scale_width * xpos;
    double scaled_ypos = scale_height * ypos;

    auto gui = getTucanoGui();
    return gui->leftButtonReleased (scaled_xpos, scaled_ypos);
}

bool Gui::cursorMove(float xpos, float ypos)
{
    float scale_width, scale_height;
    scene.getScreenScale(scale_width, scale_height);

    double scaled_xpos = scale_width * xpos;
    double scaled_ypos = scale_height * ypos;

    auto gui = getTucanoGui();
    return gui->cursorMove (scaled_xpos, scaled_ypos);
}

Tucano::GUI::Base* Gui::getTucanoGui()
{
    return &(pimpl->gui);
}

} //namespace tucanow
