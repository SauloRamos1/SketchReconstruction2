#ifndef __TUCANOW_GUI_IMPL__
#define __TUCANOW_GUI_IMPL__


/** @file gui_impl.hpp src/gui_impl.hpp
 * */


/* #include <GL/glew.h> */

/* #include <tucano/tucano.hpp> */
/* #include <tucano/utils/trackball.hpp> */
/* #include <tucano/effects/phongshader.hpp> */
/* #include <tucano/utils/plyimporter.hpp> */
/* #include <tucano/utils/imageIO.hpp> */
#include <tucano/gui/base.hpp>
#include "tucanow/scene.hpp"


namespace tucanow {


struct GuiImpl 
{
        /// GUI holder
        Tucano::GUI::Base gui;

        /// Box to group all gui elements
        Tucano::GUI::GroupBox groupbox;

        /// Reload button
        Tucano::GUI::Button reload_button;

        /// Menu show/hide button
        Tucano::GUI::SelectButton menu_button;

        /// Slider for diffuse coefficient
        Tucano::GUI::Slider kd_slider;

        /// Slider for specular coefficient
        Tucano::GUI::Slider ks_slider;

        /// Slider for ambient coefficient
        Tucano::GUI::Slider ka_slider;

        /// Slider for shininess coefficient
        Tucano::GUI::Slider shininess_slider;

        /// Label for diffuse text
        Tucano::GUI::Label diffuse_label;

        /// Label for specular text
        Tucano::GUI::Label specular_label;

        /// Label for ambient text
        Tucano::GUI::Label ambient_label;

        /// Label for shininess text
        Tucano::GUI::Label shininess_label;
};


} // namespace tucanow


#endif
