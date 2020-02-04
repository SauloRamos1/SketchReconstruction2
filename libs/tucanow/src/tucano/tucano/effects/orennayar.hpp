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

#ifndef __ORENNAYAR__
#define __ORENNAYAR__

#include <tucano/effect.hpp>
#include <tucano/camera.hpp>

namespace Tucano
{

namespace Effects
{

/**
 * @brief Renders a mesh using a Oren-Nayar BRDF shader.
 */
class OrenNayar : public Tucano::Effect
{

private:

    /// Phong Shader
    Tucano::Shader orennayar_shader;

    /// Roughness coefficient, can be in range [0, inf)
    float roughness = 1.0;

public:

    /**
     * @brief Default constructor.
     */
    OrenNayar (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        loadShader(orennayar_shader, "orennayar") ;
    }

    /**
    * @brief Get roughness coefficient
    * @return Current roughness value (ro)
    */
    float getRoughnessCoeff (void)
    {
        return roughness;
    }


    /**
    * @brief Set roughness coefficient
    * @param value New roughness coeff (ro)
    */
    void setRoughnessCoeff (float value)
    {
        roughness = value;
    }

    /** * @brief Render the mesh given a camera and light, using a Oren Nayar brdf shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera& light)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        orennayar_shader.bind();

        // sets all uniform variables for the Oren-Nayar shader
        orennayar_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        orennayar_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        orennayar_shader.setUniform("viewMatrix", camera.getViewMatrix());
        orennayar_shader.setUniform("lightViewMatrix", light.getViewMatrix());
        orennayar_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));
		orennayar_shader.setUniform("default_color", mesh.getColor());
        orennayar_shader.setUniform("sigma", roughness);

        mesh.setAttributeLocation(orennayar_shader);

        mesh.render();

        orennayar_shader.unbind();
    }

};
}

}

#endif
