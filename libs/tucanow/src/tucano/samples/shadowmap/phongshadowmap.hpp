/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PHONGSHADOWMAP__
#define __PHONGSHADOWMAP__

#include <tucano/tucano.hpp>

namespace Tucano
{
namespace Effects
{

/**
 * @brief Renders a mesh using a Phong shader and a shadow map
 */
class PhongShadowmap : public Tucano::Effect
{

private:

    bool multisampling_enabled = false;
    bool shadowmap_enabled = true;

    /// Phong Shader
    Tucano::Shader phong_shader;

    /// Default color
    Eigen::Vector4f default_color = Eigen::Vector4f (0.7, 0.7, 0.7, 1.0);

    /// Ambient coefficient
    float ka = 0.5;

    /// Diffuse coefficient
    float kd = 0.8;

    /// Specular coefficient
    float ks = 0.5;

    /// Shininess
    float shininess = 10;

public:

    /**
     * @brief Default constructor.
     */
    PhongShadowmap (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(phong_shader, "phongshadow") ;
    }

    void setShadowmapEnabled(bool n)
    {
        shadowmap_enabled = n;
    }

    bool getShadowmapEnabled()
    {
        return shadowmap_enabled;
    }

    void setMultisampling(bool n)
    {
        multisampling_enabled = n;
    }

    bool getMultisampling()
    {
        return multisampling_enabled;
    }

    /**
    * @brief Sets the default color, usually used for meshes without color attribute
    */
    void setDefaultColor ( const Eigen::Vector4f& color )
    {
        default_color = color;
    }

    /**
    * @brief Set ambient coefficient
    * @param value New ambient coeff (ka)
    */
    void setAmbientCoeff (float value)
    {
        ka = value;
    }

    /**
    * @brief Set diffuse coefficient
    * @param value New diffuse coeff (kd)
    */
    void setDiffuseCoeff (float value)
    {
        kd = value;
    }

    /**
    * @brief Set specular coefficient
    * @param New specular coeff (ks)
    */
    void setSpecularCoeff (float value)
    {
        ks = value;
    }

    /**
    * @brief Set shininess exponent
    * @param New shininess coeff (shininess)
    */
    void setShininessCoeff (float value)
    {
        shininess = value;
    }


    float getDiffuseCoeff (void ) {return kd;}
    float getAmbientCoeff (void ) {return ka;}
    float getSpecularCoeff (void ) {return ks;}
    float getShininessCoeff (void ) {return shininess;}

    /** * @brief Render the mesh given a camera and light, using a Phong shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& light, Tucano::Texture& shadowmaps, vector<Eigen::Matrix4f>& crop_vector)
    {
        int num_slices = crop_vector.size();

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        phong_shader.bind();

        Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();
        for(int i=0; i<8; i++)
        {
            if(i < num_slices)
            {
                phong_shader.setUniform(std::string("cropMatrix" + std::to_string(i)).c_str(), crop_vector[i]);
            }
            else
            {
                phong_shader.setUniform(std::string("cropMatrix" + std::to_string(i)).c_str(), identity);
            }
        }

        phong_shader.setUniform("shadowmaps", shadowmaps.bind());
        phong_shader.setUniform("num_slices", num_slices);
        phong_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        phong_shader.setUniform("modelMatrix", mesh.getModelMatrix());
        phong_shader.setUniform("viewMatrix", camera.getViewMatrix());
        phong_shader.setUniform("lightViewMatrix", light.getViewMatrix());
        phong_shader.setUniform("lightProjectionMatrix", light.getProjectionMatrix());
        phong_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
		phong_shader.setUniform("default_color", mesh.getColor());
        
        phong_shader.setUniform("multisampling_enabled", multisampling_enabled);
        phong_shader.setUniform("shadowmap_enabled", shadowmap_enabled);
        phong_shader.setUniform("ka", ka);
        phong_shader.setUniform("kd", kd);
        phong_shader.setUniform("ks", ks);
        phong_shader.setUniform("shininess", shininess);

        mesh.setAttributeLocation(phong_shader);

        glEnable(GL_DEPTH_TEST);
        mesh.render();

        shadowmaps.unbind();
        phong_shader.unbind();
    }
};
}
}


#endif
