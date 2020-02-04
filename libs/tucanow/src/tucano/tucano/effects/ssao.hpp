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

#ifndef __SSAO__
#define __SSAO__

#include <tucano/effect.hpp>
#include <tucano/camera.hpp>
#include <tucano/framebuffer.hpp>
#include <tucano/utils/ppmIO.hpp>

namespace Tucano
{
namespace Effects
{

/**
 * Screen Space Ambient Occlusion effect class. Handles the pre-computations needed in order to use this effect, as well as handles the rendering with this effect. 
 *
 * Based on http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
**/
class SSAO: public Tucano::Effect {

protected:

    ///Noise texture
    Tucano::Texture noise_texture;

    ///Kernel radius. If the distance between a sample point and the point for which the occlusion is being computed is larger than radius, the occlusion for this sample will be neglected.
    float radius;

    /// Framebuffer to store coord/normal buffer
    Tucano::Framebuffer fbo;

    /// The per pixel AO computation shader
    Tucano::Shader ssao_shader;

    /// Save coord, normal and color to FBO
    Tucano::Shader deferred_shader;

    /// Join original render with SSAO (blur it first)
    Tucano::Shader ssao_final_shader;

    /// A quad mesh for framebuffer rendering
    Tucano::Mesh quad;

    /// Flag indicating wether blur shall be applied or not.
    bool apply_blur = true;

    /// Flag indicating if the mesh should be rendered only with ambient occlusion pass or with full illumination. If True, mesh will be rendered only with the ambient occlusion pass.
    bool displayAmbientPass = false;

    /// Number of neighbour pixels used in blurring. The blur will be applied to a blurRange x blurRange square around the current pixel. It's important to notice that blurRange must be an odd number.
    int blurRange = 3;

    /// The ID defining the color attachment to which the depth texture is bound in the framebuffer.
    int depthTextureID = 0;

    /// The ID defining the color attachment to which the normal texture is bound in the framebuffer.
    int normalTextureID = 1;

    /// The ID defining the color attachment to which the color texture is bound in the framebuffer.
    int colorTextureID = 2;

    /// The ID of the color attachment holding the SSAO result.
    int ssaoTextureID = 3;

    /// The ID defining the color attachment to which the blur texture is bound in the framebuffer.
    int blurTextureID = 4;

    /// Global intensity value.
    int intensity = 1;

    /// Global scale
    float global_scale = 1.0;


public:

    /**
     * @brief Default constructor.
     *
	 * @param rad The kernel radius. This is used to define the max distance between the current point and the samples that will be considered for occlusion computation.
	**/
    SSAO (float rad = 1.0)
    {
        radius = rad;
	}

    /**
     * @brief Initializes the SSAO effects,
     *
     * generating the sample kernel, creating and loading the shaders and generating the noise texture.
     */
    virtual void initialize (void)
    {
        initializeShaders();

        // @TODO put appropiate image path for random.ppm
        Tucano::ImageImporter::loadPPMImage("../samples/images/random.ppm", &noise_texture);
        noise_texture.setTexParameters( GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST );

        #ifdef TUCANODEBUG
        Tucano::Misc::errorCheckFunc(__FILE__, __LINE__);
        #endif

        quad.createQuad();
    }

    /**
     * @brief First pass of the SSAO, writes coords, normals and colors to a buffer.
     * @param mesh Mesh to be rendered.
     * @param camera_trackball A pointer to the camera trackball object.
     * @param light_trackball A pointer to the light trackball object.
     */
    void createViewSpaceBuffer (Tucano::Mesh& mesh, const Tucano::Camera& camera_trackball, const Tucano::Camera& light_trackball)
    {

        // Bind buffer to store coord, normal and color information
        fbo.clearAttachments();
        fbo.bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

        deferred_shader.bind();
        deferred_shader.setUniform("projectionMatrix", camera_trackball.getProjectionMatrix());
        deferred_shader.setUniform("modelMatrix", mesh.getShapeModelMatrix());
        deferred_shader.setUniform("viewMatrix", camera_trackball.getViewMatrix());
        deferred_shader.setUniform("lightViewMatrix", light_trackball.getViewMatrix());
        deferred_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));

        mesh.setAttributeLocation(deferred_shader);
        mesh.render();

        deferred_shader.unbind();
        fbo.unbind();
        fbo.clearDepth();

    }

    /**
     * @brief Compute the Ambient Occlusion factor for each pixel.
     */
    void computeSSAO (const Tucano::Camera& camera)
    {

        fbo.bindRenderBuffer(ssaoTextureID);

        ssao_shader.bind();

        ssao_shader.setUniform("coordsTexture", fbo.bindAttachment(depthTextureID));
        ssao_shader.setUniform("normalTexture", fbo.bindAttachment(normalTextureID));
        ssao_shader.setUniform("noise_texture", noise_texture.bind());
        ssao_shader.setUniform("viewport", (GLfloat)fbo.getWidth(), (GLfloat)fbo.getHeight());
        ssao_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        ssao_shader.setUniform("radius", radius);
        ssao_shader.setUniform("global_scale", global_scale);
        ssao_shader.setUniform("intensity", (GLfloat)intensity);

        quad.setAttributeLocation(ssao_shader);
        quad.render();

        ssao_shader.unbind();
        noise_texture.unbind();
        fbo.unbind();
        fbo.clearDepth();
    }


    /**
     * @brief Blur SSAO result and mix with original render
     * @param light_trackball A pointer to the light trackball object.
     */
    void applySSAO (const Tucano::Camera& light_trackball)
    {
        ssao_final_shader.bind();

        ssao_final_shader.setUniform("lightViewMatrix", light_trackball.getViewMatrix());

        ssao_final_shader.setUniform("coordsTexture", fbo.bindAttachment(depthTextureID));
        ssao_final_shader.setUniform("normalTexture", fbo.bindAttachment(normalTextureID));
        ssao_final_shader.setUniform("colorTexture", fbo.bindAttachment(colorTextureID));
        ssao_final_shader.setUniform("ssaoTexture", fbo.bindAttachment(ssaoTextureID));
        ssao_final_shader.setUniform("blurRange", blurRange);

        quad.setAttributeLocation(ssao_final_shader);
        quad.render();

        ssao_final_shader.unbind();
        fbo.unbind();
    }

	/**
     * @brief Renders the mesh with the desired effect.
     *
     * The algorithm has three passes:
     * 1. compute buffer with coords, normals and color per pixel
     * 2. compute AO per pixel
     * 3. blur the final result
     * An option to pass an output buffer is available in case of offline rendering.
     * For example, when taking snapshots of the current result.
	 * @param mesh Mesh to be rendered.
	 * @param camera_trackball A pointer to the camera trackball object.
	 * @param light_trackball A pointer to the light trackball object.
     */
    virtual void render(Tucano::Mesh& mesh, const Tucano::Camera& camera_trackball, const Tucano::Camera& light_trackball)
    {
        Eigen::Vector4f viewport = camera_trackball.getViewport();
        Eigen::Vector2i viewport_size = camera_trackball.getViewportSize();

        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // check if viewport was modified, if so, regenerate fbo
        if (fbo.getWidth() != viewport_size[0] || fbo.getHeight() != viewport_size[1])
        {
            fbo.create(viewport_size[0], viewport_size[1], 5);
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // first pass
        createViewSpaceBuffer (mesh, camera_trackball, light_trackball);

        // second pass
        computeSSAO(camera_trackball);

        // final pass, blur SSAO and join with original render
        applySSAO(light_trackball);
    }

    /**
     * @brief Get intensity value.
     * @return Intensity value.
     */
    int getIntensity (void)
    {
        return intensity;
    }

    /**
     * @brief Set intensity value.
     * @param value New intensity value.
     */
    void setIntensity (int value)
    {
        intensity = value;
    }

	/**
     * @brief Get radius value.
     * @return Radius value.
     */
    int getRadius (void)
    {
        return radius;
    }


    /**
     * @brief Set radius value.
     * @param value New radius value.
     */
    void setRadius (int value)
    {
        radius = value;
    }

    /**
     * @brief Set global scale value.
     * @param value New scale value.
     */
    void setScale (int value)
    {
        global_scale = value;
    }


    /**
     * Increases blur range.
     */
    void setBlurRange (int value)
    {
        blurRange = value;
	}

    /**
     * @brief Toggles the displayAmbientPass flag.
     */
    void toggleAmbientPass (void)
    {
        displayAmbientPass = !displayAmbientPass;
    }


private:

    /**
     * @brief Creates and loads all shaders.
     */
    void initializeShaders (void)
    {
		loadShader(ssao_shader, "ssao");
		loadShader(deferred_shader, "viewspacebuffer");
		loadShader(ssao_final_shader, "ssaofinal");
    }

};
}
}

#endif
