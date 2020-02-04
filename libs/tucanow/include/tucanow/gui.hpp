#ifndef __TUCANOW_GUI__
#define __TUCANOW_GUI__

/** @file gui.hpp tucanow/gui.hpp
 * */


#include <memory>
#include <string>


namespace Tucano {
namespace GUI {
    class Base;
}
}

namespace tucanow {


struct GuiImpl;
class Scene;


class Gui 
{
    public: 
        /**
         * @brief Constructor implements the pimpl idiom
         */
        Gui(Scene&);

        /**
         * @brief Default destructor
         */
        virtual ~Gui();


        /**
         * @brief Deleted copy constructor
         */
        Gui(Gui &) = delete;

        /**
         * @brief Deleted copy assignment
         */
        Gui& operator=(Gui &) = delete;

        /**
         * @brief Deleted move constructor
         */
        Gui(Gui &&) = delete;

        /**
         * @brief Deleted move assignment
         */
        Gui& operator=(Gui&&) = delete;

        /**
         * @brief Initialize the gui and shaders
         *
         * Must be called after Glew has been initialized
         *
         * @param width Viewport width 
         * @param height Viewport height 
         * @param assets_dir Path to dir where gui's assets can be found
         */
        virtual void initialize(int width, int height, std::string assets_dir = "./samples/assets/");

        /**
         * @brief Set gui's scene viewport
         *
         * @param width Viewport width
         * @param height Viewport height
         */
        virtual bool setViewport(int width, int height);

        /**
         * @brief Render gui.
         **/
        virtual void render();

        /**
         * @brief Handles mouse left click callback for all elements
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         *
         * @return True if any gui element was clicked, false otherwise
         */
        virtual bool leftButtonPressed(float xpos, float ypos);

        /**
         * @brief Handles mouse left release callback for last clicked element
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         *
         * @return True if any element is selected, false otherwise
         */
        virtual bool leftButtonReleased(float xpos, float ypos);

        /**
         * @brief Handles mouse movement
         *
         * @param xpos Mouse x position
         * @param ypos Mouse y position
         *
         * @return True if any element is selected, false otherwise
         */
        virtual bool cursorMove(float xpos, float ypos);

    protected:
        /**
         * @brief Get a pointer to Tucano's gui base class
         *
         * @return Pointer to Tucano::GUI::Base
         */
        virtual Tucano::GUI::Base* getTucanoGui();

        Scene& scene; ///<-- Reference to Scene object
        std::unique_ptr<GuiImpl> pimpl; ///<-- Tucano data
};


} // namespace tucanow


#endif
