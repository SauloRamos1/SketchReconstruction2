#ifndef __TUCANOW_SCENE_IMPL__
#define __TUCANOW_SCENE_IMPL__


/** @file scene_impl.hpp src/scene_impl.hpp
 * */


/* #include <GL/glew.h> */

#include <tucano/utils/trackball.hpp>
#include <tucano/effects/phongshader.hpp>
#include <tucano/effects/wireframe.hpp>
#include <tucano/utils/plyimporter.hpp>
#include <tucano/utils/imageIO.hpp>
#include <tucano/gui/base.hpp>

#include "tucanow/scene.hpp"


namespace tucanow {


struct SceneImpl 
{
    /// Mesh to be rendered in this widget
    Tucano::Mesh mesh;

    /// Phong effect to render mesh
    Tucano::Effects::Phong phong;

    /// Single pass wireframe shader
    Tucano::Effects::Wireframe wireframe;

    /// Trackball for manipulating the camera
    Tucano::Trackball camera;

    /// Trackball for manipulating light direction
    Tucano::DirectionalTrackball light;

    /// Path to shader's dir
    string shader_dir;

    /// OpenGL clear color
    Eigen::Vector4f clear_color = Eigen::Vector4f(1.f, 1.f, 1.f, 0.f);

    /// Source of current mesh
    enum class MeshType { NONE, FROM_VECTORS, FROM_FILE } mesh_t = MeshType::NONE;
};

using MeshType = SceneImpl::MeshType; ///<-- Rename SceneImpl::MeshType in tucanow namespace


} // namespace tucanow


#endif
