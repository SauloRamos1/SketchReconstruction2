#include "scene_impl.hpp"
#include "tucanow/scene.hpp"
/* #include "tucanow/misc.hpp" */
/* #include <iostream> */


namespace tucanow {


Scene::Scene() //: pimpl(new SceneImpl)
{
    // Glew must be initialized before any Tucano object is created
    /* misc::initGlew(); */

    pimpl.reset(new SceneImpl);

    // Set light cyan as the background colour
    setClearColor(224, 255, 255, 255);
}

Scene::~Scene() = default;

Scene::Scene(Scene &&) = default;
Scene& Scene::operator=(Scene &&) = default;

/* void Scene::initialize(int width, int height, std::string assets_dir /1* = "../samples/assets/" *1/) */
void Scene::initialize(int width, int height)
{
    if ( width < 1 )
        width = 1;

    if ( height < 1 )
        height = 1;

    // initialize the shader effect (if TUCANOSHADERDIR is set, no need to set dir before init)
    pimpl->phong.initialize();
    pimpl->wireframe.initialize();

    pimpl->camera.setPerspectiveMatrix(60.0, (float)width/(float)height, 0.1f, 100.0f);
    pimpl->camera.setRenderFlag(false);
    pimpl->camera.setViewport(Eigen::Vector2f ((float)width, (float)height));

    pimpl->light.setRenderFlag(false);
    pimpl->light.setViewport(Eigen::Vector2f ((float)width, (float)height));
    setHeadlight(headlight_camera);

    // Manually tuned parameters
    pimpl->phong.setAmbientCoeff(0.525);
    pimpl->phong.setDiffuseCoeff(0.75);
    pimpl->phong.setSpecularCoeff(0.0875);
    pimpl->phong.setShininessCoeff(3.475);

    glEnable(GL_DEPTH_TEST);
}

void Scene::render(void)
{
    glClearColor(
            pimpl->clear_color[0], 
            pimpl->clear_color[1], 
            pimpl->clear_color[2],
            pimpl->clear_color[3]
        );
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if ( render_wireframe )
    {
        pimpl->wireframe.render(pimpl->mesh, pimpl->camera, pimpl->light);
    }
    else
    {
        pimpl->phong.render(pimpl->mesh, pimpl->camera, pimpl->light);
    }
    pimpl->camera.render();
}

void Scene::renderWireframe(bool wireframe)
{
    render_wireframe = wireframe;
}

void Scene::toggleRenderWireframe()
{
    render_wireframe = !render_wireframe;
}

// /**
//  * @brief Set path for the shader's dir
//  *
//  * @param dir Path to shader's dir
//  */

// /* void setShaderDir(std::string dir); */

/* void Scene::setShaderDir(std::string dir) */
/* { */
/*     pimpl->shader_dir = dir; */
/* } */

void Scene::setClearColor(float r, float g, float b, float a)
{
    pimpl->clear_color = Eigen::Vector4f(r, g, b, a);
}

void Scene::setClearColor(int r, int g, int b, int a)
{
    setClearColor((float)(r)/255.0f, (float)(g)/255.0f, (float)(b)/255.0f, (float)(a)/255.0f);
}

bool Scene::setMesh(const std::vector<float> &vertices, const std::vector<GLuint> &indices, const std::vector<float> &vertex_normals)
{
    if ( !vertex_normals.empty() )
    if ( vertices.size() != vertex_normals.size() )
    {
        /* std::cout << "\nError: vertices.size() != normals.size()\n"; */
        return false;
    }
    
    if ( vertices.empty() )
    {
        /* std::cout << "\nError: vertices.empty() != true\n"; */
        return false;
    }

    if ( pimpl->mesh_t != MeshType::NONE )
    {
        pimpl->mesh.reset();
    }

    bool success = pimpl->mesh.loadVertices(vertices);
    if ( success )
        /* std::cout << "\nGot vertices\n"; */


    if ( success && !indices.empty() )
    {
        pimpl->mesh.loadIndices(indices);
        /* std::cout << "\nGot indices\n"; */
    }

    if ( success && !vertex_normals.empty() )
    {
        pimpl->mesh.loadNormals(vertex_normals);
        /* std::cout << "\nGot normals\n"; */
    }

    if ( success )
    {
        pimpl->mesh.normalizeModelMatrix();
        pimpl->mesh_t = MeshType::FROM_VECTORS;
        /* std::cout << "\nGot mesh\n"; */
    }

    return success;
}

/* bool Scene::setVertices(std::vector<float> &vertices) */
/* { */
/*     if ( pimpl->mesh_t != MeshType::NONE ) */
/*     { */
/*         pimpl->mesh.reset(); */
/*     } */

/*     if ( vertices.empty() ) */
/*     { */
/*         return false; */
/*     } */

/*     bool success = pimpl->mesh.loadVertices(vertices); */
/*     if ( success ) */
/*     { */
/*         pimpl->mesh.normalizeModelMatrix(); */
/*         mesh_t = MeshType::FROM_VECTORS; */
/*     } */

/*     return success; */
/* } */

/* bool Scene::setIndices(std::vector<unsigned int> &indices) */
/* { */
/*     if ( mesh_t != MeshType::FROM_VECTORS ) */
/*     { */
/*         return false; */
/*     } */

/*     if ( indices.empty() ) */
/*     { */
/*         return false; */
/*     } */

/*     pimpl->mesh.loadIndices(indices); */

/*     return true; */
/* } */

/* bool Scene::setNormals(std::vector<float> &normals) */
/* { */
/*     if ( mesh_t != MeshType::FROM_VECTORS ) */
/*     { */
/*         return false; */
/*     } */

/*     if ( normals.empty() ) */
/*     { */
/*         return false; */
/*     } */

/*     return pimpl->mesh.loadNormals(normals); */
/* } */

void Scene::setMeshColor(float r, float g, float b, float a)
{
    /* pimpl->phong.setDefaultColor(Eigen::Vector4f(r, g, b, a)); */
    pimpl->mesh.setColor(Eigen::Vector4f(r, g, b, a));
}

void Scene::setMeshColor(int r, int g, int b, int a)
{
    setMeshColor((float)(r)/255.0f, (float)(g)/255.0f, (float)(b)/255.0f, (float)(a)/255.0f);
}

bool Scene::setMeshColorsRGB(std::vector<float> &colors)
{
    if ( pimpl->mesh_t != MeshType::FROM_VECTORS )
    {
        return false;
    }

    return pimpl->mesh.loadColorsRGB(colors);
}

bool Scene::setMeshColorsRGBA(std::vector<float> &colors)
{
    if ( pimpl->mesh_t != MeshType::FROM_VECTORS )
    {
        return false;
    }

    return pimpl->mesh.loadColorsRGBA(colors);
}

bool Scene::setMeshTexCoords(std::vector<float> &texture)
{
    if ( pimpl->mesh_t != MeshType::FROM_VECTORS )
    {
        return false;
    }

    if ( texture.empty() )
    {
        return false;
    }

    return pimpl->mesh.loadTexCoords(texture);
}

bool Scene::loadMeshFromPLY(std::string filename)
{
    if ( pimpl->mesh_t != MeshType::NONE )
    {
        pimpl->mesh.reset();
    }

    bool success = Tucano::MeshImporter::loadPlyFile(&pimpl->mesh, filename);
    if (success)
    {
        pimpl->mesh.normalizeModelMatrix();
        std::string tex_file = Tucano::MeshImporter::getPlyTextureFile(filename);
        if (!tex_file.empty())
        {
            size_t found = filename.find_last_of("/\\");
            string dir = filename.substr(0, found);
            string tex_file_with_dir = dir + "/" + tex_file;
 
            setModelTexture(tex_file_with_dir);
        }

        pimpl->mesh_t = MeshType::FROM_FILE;
    }

    return success;
}

bool Scene::setModelTexture(std::string tex_file)
{
    Tucano::Texture texture;
    bool success = Tucano::ImageImporter::loadImage(tex_file, &texture);
    if (success)
    {        
        pimpl->phong.setTexture(texture);
        pimpl->phong.getTexture()->setTexParameters( GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR );
    }

    return success;
}

bool Scene::setViewport(int width, int height)
{
    if ( ( width < 1 ) || ( height < 1 ) )
    {
        return false;
    }

    pimpl->camera.setViewport(Eigen::Vector2f((float)width, (float)height));
    pimpl->light.setViewport (Eigen::Vector2f((float)width, (float)height));

    return true;
}

void Scene::setHeadlight(bool headlight)
{
    headlight_camera = headlight;

    if (headlight_camera)
    {
        *(pimpl->light.viewMatrix()) = *(pimpl->camera.viewMatrix());
    }
}

void Scene::toggleHeadlight()
{
    setHeadlight(!headlight_camera);
}

void Scene::resetCamera()
{
    pimpl->camera.reset();
    pimpl->light.reset();
}

void Scene::increaseCameraZoom()
{
        pimpl->camera.increaseZoom(1.05);
}

void Scene::decreaseCameraZoom()
{
        pimpl->camera.increaseZoom(1.0/1.05);
}

void Scene::rotateCamera(float xpos, float ypos)
{
    float scaled_xpos = scale_width * xpos;
    float scaled_ypos = scale_height * ypos;

    /* std::cout << "\nxpos = " << xpos << "; ypos = " << ypos << "\n"; */
    /* std::cout << "\nscaled_xpos = " << scaled_xpos << "; scaled_ypos = " << scaled_ypos << "\n"; */

    pimpl->camera.rotateCamera( Eigen::Vector2f (scaled_xpos, scaled_ypos) );

    if( headlight_camera )
    {
        *(pimpl->light.viewMatrix()) = *(pimpl->camera.viewMatrix());
    }
}

void Scene::stopRotateCamera()
{
    pimpl->camera.endRotation();

    if( headlight_camera )
    {
        *(pimpl->light.viewMatrix()) = *(pimpl->camera.viewMatrix());
    }
}

void Scene::translateCamera(float xpos, float ypos)
{
    float scaled_xpos = scale_width * xpos;
    float scaled_ypos = scale_height * ypos;

    pimpl->camera.translateCamera( Eigen::Vector2f(scaled_xpos, scaled_ypos) );
}

void Scene::stopTranslateCamera()
{
    pimpl->camera.endTranslation();
}

void Scene::rotateLight(float xpos, float ypos)
{
    float scaled_xpos = scale_width * xpos;
    float scaled_ypos = scale_height * ypos;

    if (!headlight_camera)
    {
        pimpl->light.rotateCamera( Eigen::Vector2f (scaled_xpos, scaled_ypos) );
    }
}

void Scene::stopRotateLight()
{
    if (!headlight_camera)
    {
        pimpl->light.endRotation();
    }
}


bool Scene::setScreenScale( float swidth, float sheight )
{
    bool success = true;
    success &= swidth > 0;
    success &= sheight > 0;

    if ( success )
    {
        scale_width = swidth;
        scale_height = sheight;
    }

    return success;
}

void Scene::getScreenScale( float& swidth, float& sheight )
{
    swidth = scale_width;
    sheight = scale_height;
}

} // namespace tucanow


