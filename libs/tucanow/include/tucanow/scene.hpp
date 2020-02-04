#ifndef __TUCANOW_SCENE__
#define __TUCANOW_SCENE__

/** @file scene.hpp tucanow/scene.hpp
 * */


#include<memory>
#include<string>
#include<vector>


namespace tucanow {


struct SceneImpl;
class Gui;


class Scene 
{
    public:
        /**
         * @brief Constructor implements the pimpl idiom
         */
        Scene();

        /**
         * @brief Default destructor
         */
        virtual ~Scene();

        /**
         * @brief Deleted copy constructor
         */
        Scene(const Scene &) = delete;

        /**
         * @brief Deleted copy assignment
         */
        Scene& operator=(const Scene &) = delete;

        /**
         * @brief Default move constructor
         */
        Scene(Scene &&);

        /**
         * @brief Default move assignment
         */
        Scene& operator=(Scene &&);

        /**
         * @brief Initializes the scene and shaders
         *
         * Must be called after Glew has been initialized
         *
         * @param width Viewport width 
         * @param height Viewport height 
         */
        virtual void initialize(int width, int height);

        /**
         * @brief Set scene viewport
         *
         * @param width Viewport width in pixels
         * @param height Viewport height in pixels 
         */
        virtual bool setViewport(int width, int height);

        /** @brief Set screen scale for HighDPI screens
         *
         * In HighDPI mode (e.g., macOS) the framebuffer size and the viewport
         * size may differ.  Set the screen scale to the ratio between both
         * sizes so that actions based on the screen position behave correctly
         *
         * @param scale_width Ratio (framebuffer width)/(logical window width) 
         * @param scale_height Ratio (framebuffer length)/(logical window length)
         */
        virtual bool setScreenScale(float scale_width, float scale_height);

        /** @brief Get screen scale for HighDPI screens
         *
         * In HighDPI mode (e.g., macOS) the framebuffer size and the viewport
         * size may differ.  Use the screen scale to fix actions that depend on
         * screen coordinates
         *
         * @param scale_width Ratio (framebuffer width)/(logical window width) 
         * @param scale_height Ratio (framebuffer length)/(logical window length)
         */
        virtual void getScreenScale(float& scale_width, float& scale_height);

        /**
         * @brief Set OpenGL clear color.
         *
         * @param r Intensity of red
         * @param g Intensity of green
         * @param b Intensity of blue
         * @param a Intensity of alpha (usually opacity)
         */
        virtual void setClearColor(float r, float g, float b, float a = 1.f);

        /**
         * @brief Set OpenGL clear color.
         *
         * Overloaded method
         *
         * @param r Intensity of red
         * @param g Intensity of green
         * @param b Intensity of blue
         * @param a Intensity of alpha (usually opacity)
         */
        virtual void setClearColor(int r, int g, int b, int a = 255);

        /**
         * @brief Repaints screen buffer.
         **/
        virtual void render();

        /**
         * @brief Render model with a single pass wireframe shader
         *
         * Triangles are shaded with flat colours.
         *
         * @param render_wireframe Set true to render wireframe, false for default shading
         */
        void renderWireframe(bool render_wireframe);

        /**
         * @brief Alternate between wireframe and default shading
         */
        void toggleRenderWireframe();
        /**
         * @brief Load a mesh to visualize
         *
         * @param vertices Mesh vertices (must be non-empty)
         * @param indices Mesh triangles (vector of indices on the vertices' list)
         * @param vertex_normals Normals per vertex
         *
         * @return True if mesh was set correctly
         */
        bool setMesh(
                const std::vector<float> &vertices, 
                const std::vector<unsigned int> &indices = {}, 
                const std::vector<float> &vertex_normals = {}
                );

        /**
         * @brief Set mesh's single colour -- does not affect mesh loaded from a ply file
         *
         * @param r Intensity of red
         * @param g Intensity of green
         * @param b Intensity of blue
         * @param a Intensity of alpha (usually opacity)
         */
        void setMeshColor(float r, float g, float b, float a = 1.f);

        /**
         * @brief Set mesh's single colour -- does not affect mesh loaded from a ply file
         *
         * Overloaded method
         *
         * @param r Intensity of red
         * @param g Intensity of green
         * @param b Intensity of blue
         * @param a Intensity of alpha (usually opacity)
         */
        void setMeshColor(int r, int g, int b, int a = 255);

        /**
         * @brief Set mesh's RGB colours per vertex -- does not affect mesh loaded from a ply file
         *
         * @param colors Vector of RGB colours per vertex
         *
         * @return True if colours vector size is a multiple of 3
         */
        bool setMeshColorsRGB(std::vector<float> &colors);

        /**
         * @brief Set mesh's RGB colours per vertex -- does not affect mesh loaded from a ply file
         *
         * @param colors Vector of RGBA colours per vertex
         *
         * @return True if colours vector size is a multiple of 4
         */
        bool setMeshColorsRGBA(std::vector<float> &colors);

        /**
         * @brief Set texture coordinates (u,v) as a vertex attribute.
         *
         * @param texture Texture coordinates array.
         *
         * @return True if texture vector is multiple of 2, false otherwise.
         */
        bool setMeshTexCoords(std::vector<float> &texture);

        /**
         * @brief Open a Ply mesh file
         *
         * @param filename Name of file to open
         *
         * @return True if mesh was loaded successfully
         */
        bool loadMeshFromPLY(std::string filename);

        /**
         * @brief Sets a texture for the model
         *
         * Mesh must have tex coords to work properly
         * @param tex_file Texture filename
         *
         * @return True if texture file was loaded correctly
         */
        bool setModelTexture(std::string tex_file);

        /**
         * @brief Place light source direction at the trackball camera direction
         *
         * @param headlight Set true for headlight, false for fixed light
         */
        void setHeadlight(bool headlight);

        /**
         * @brief Alternate between a headlight and a fixed light
         */
        void toggleHeadlight();
        // toggle

        /**
         * @brief Reset camera
         */
        void resetCamera();

        /**
         * @brief Increase zoom level
         */
        void increaseCameraZoom();

        /**
         * @brief Decrease zoom level
         */
        void decreaseCameraZoom();

        /**
         * @brief Accumulate sequence of increments to rotate camera 
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         */
        void rotateCamera(float xpos, float ypos);

        /**
         * @brief Rotate camera and stop rotateCamera() mode
         */
        void stopRotateCamera();

        /**
         * @brief Accumulate sequence of increments to translate camera 
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         */
        void translateCamera(float xpos, float ypos);

        /**
         * @brief Translate camera and stop translateCamera() mode
         */
        void stopTranslateCamera();

        /**
         * @brief Accumulate sequence of increments to rotate light 
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         */
        void rotateLight(float xpos, float ypos);

        /**
         * @brief Rotate light and stop rotateLight() mode
         */
        void stopRotateLight();

    protected:
        std::unique_ptr<SceneImpl> pimpl; ///<-- Tucano data

        /** @relates tucanow::Gui
         * */
        friend class Gui;

    private:
        bool render_wireframe = false; ///<-- Controls whether wireframe or default rendering is used

        bool headlight_camera = true; ///<-- Controls whether a headlight or a fixed light is used

        float scale_width = 1.0f; ///<-- Ratio framebuffer_width/window_width
        float scale_height = 1.0f; ///<-- Ratio framebuffer_height/window_height
};


} // namespace tucanow


#endif // MODELWIDGET
