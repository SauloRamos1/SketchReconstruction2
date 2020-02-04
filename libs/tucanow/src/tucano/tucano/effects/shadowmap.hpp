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

#ifndef __SHADOWMAP__
#define __SHADOWMAP__

#include <tucano/tucano.hpp>

namespace Tucano
{
namespace Effects
{

/**
 * Simple Shadow Mapping
 * Renders depth buffer from light position to compute the shadow map
**/
class ShadowMap: public Tucano::Effect {

protected:
    /// Framebuffer to store the shadow depth map, this is using multisampling
    Tucano::Framebuffer fbo;

    /// A auxiliary fbo to blit multisampled fbo
    Tucano::Framebuffer aa_fbo;

    /// Creates the shadowmap depth buffer by rendering from light pos 
    Tucano::Shader shadowbuffer_shader;

    /// Shader to render shadow map
    Tucano::Shader drawbuffer_shader;

    /// Quad for rendering shadow map, mostly for debug
    Tucano::Mesh quad;

    /// Shadowbuffer size
    Eigen::Vector2i viewport = Eigen::Vector2i(1024, 1024);

    int depth_tex_id = 0;

    int num_samples = 4;

    /// Flag to recreate buffer during next render
    bool recreate_fbo = true;

public:

    /**
     * @brief Default constructor.
     *
	**/
    ShadowMap (void)
    {
        quad.createQuad();
        glEnable(GL_MULTISAMPLE);
	}

    /**
     * @brief Initializes the ShadowMap effects,
     *
     */
    virtual void initialize (void)
    {
		loadShader(shadowbuffer_shader, "shadowbuffer");
        loadShader(drawbuffer_shader, "rendershadowmap");
    }

	void clearShadowBuffer (void)
	{
		fbo.clearAttachments();
	}

    /**
     * @brief Returns a pointer to the shadow map with single sampling
     * Note that if multisapling is used (num samples > 1) a blit operation
     * is performed, and the single sampled map is returned
     * @return Pointer to the shadow map with single sampling
     */
    Tucano::Framebuffer* getShadowMap (void)
	{
        if (num_samples == 1)
            return &fbo;
		return &aa_fbo;
	}

    /**
     * @brief Returns a pointer to the multisampled shadow map
     * @return Pointer to multisample shadowmap
     */
    Tucano::Framebuffer* getShadowMapMultisample (void)
    {
        return &fbo;   
    }

    /**
    * @brief Set the number of samples for multisampling
    * @param n Given number of samples
    */
    void setNumSamples (int n)
    {
        num_samples = n;
        recreate_fbo = true;
    }

    /**
    * @brief Set the shadow buffer size
    * @param n Given size as vector 2i
    */
    void setBufferSize (const Eigen::Vector2i & size)
    {
        viewport = size;
        recreate_fbo = true;
    }

    /**
     * @brief Creates FBOs, or recreates when size changes
     */
    void createBuffers (void)
    {
        aa_fbo.create(viewport[0], viewport[1], 1, 1); // single sampling
        fbo.create(viewport[0], viewport[1], 1, num_samples); // multi sampling (actually used for generatin map)
        fbo.getTexture(0)->setTexParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

        // if using sampler2DShadow in shaders, but usually we do the check manually
        fbo.getTexture(0)->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);   
        fbo.getTexture(0)->unbind();

        fbo.clearAttachments();
        aa_fbo.clearAttachments();
    }

    void renderBuffer (const Tucano::Camera& camera)
    {
        Eigen::Vector4f viewport = camera.getViewport();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        drawbuffer_shader.bind();
		if (num_samples == 1)
	        drawbuffer_shader.setUniform("shadowmap", fbo.bindAttachment(0));
		else
	        drawbuffer_shader.setUniform("shadowmap", aa_fbo.bindAttachment(0));
        quad.setAttributeLocation(drawbuffer_shader);
        quad.render();
        drawbuffer_shader.unbind();
        fbo.unbindAttachments();
        
    }

    /**
     * @brief Composes the shadow depth buffer
     * @param mesh Mesh to be rendered.
     * @param camera A pointer to the camera trackball object.
     * @param light A pointer to the light trackball object.
     */
    void render (Tucano::Mesh& mesh, const Tucano::Camera& camera, const Tucano::Camera & light, Eigen::Matrix4f cropMatrix = Eigen::Matrix4f::Identity())
	{
        if (recreate_fbo)
        {
            createBuffers();
            recreate_fbo = false;
        }

        glEnable(GL_DEPTH_TEST);
        
        glViewport(0, 0, viewport[0], viewport[1]);

        // Bind buffer to store coord
        fbo.bindRenderBuffer(depth_tex_id);

        shadowbuffer_shader.bind();
        shadowbuffer_shader.setUniform("projectionMatrix", light.getProjectionMatrix());
        shadowbuffer_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        shadowbuffer_shader.setUniform("viewMatrix", light.getViewMatrix());
        shadowbuffer_shader.setUniform("cropMatrix", cropMatrix);

        mesh.setAttributeLocation(shadowbuffer_shader);
        mesh.render();

        shadowbuffer_shader.unbind();
        fbo.unbind();

        if (num_samples > 1) // if multisampling prepare Anti Aliased single sampled buffer
            fbo.blitTo(aa_fbo);
    }

};
}
}
#endif
