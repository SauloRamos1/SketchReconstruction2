#include <iostream>
#include <tucano/tucano.hpp>
#include <glwidget.hpp>
#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

ModelWidget* widget;

void initialize (void)
{
    Tucano::Misc::initGlew();
	widget = new ModelWidget();	
	widget->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    widget->openMeshFile("../samples/models/toy.ply");

    cout << endl << endl << " ************ usage ************** " << endl;
    cout << "Left mouse button : rotate trackball" << endl;
    cout << "Right mouse button : translate trackball" << endl;
    cout << "Midel mouse button : change light direction" << endl;
    cout << "R : reset trackball and light direction" << endl;
    cout << " ***************** " << endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);	
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
		widget->getCamera()->reset();
        widget->getLight()->reset();
    }
}

static void mouseButtonCallback (GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (widget->getGUI()->leftButtonPressed (xpos, ypos))
            return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        widget->getGUI()->leftButtonReleased (xpos, ypos);


    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		widget->getCamera()->rotateCamera( Eigen::Vector2f (xpos, ypos) );
	}
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        widget->getCamera()->endRotation();
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        widget->getCamera()->translateCamera(Eigen::Vector2f(xpos, ypos));
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        widget->getCamera()->endTranslation();
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS)
        {
            widget->getLight()->rotateCamera( Eigen::Vector2f (xpos, ypos) );
        }
        else if (action == GLFW_RELEASE)
        {
            widget->getLight()->endRotation();
        }
    }


}
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

    if ( widget->getGUI()->cursorMove (xpos, ypos) )
	{
        return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		widget->getCamera()->rotateCamera(Eigen::Vector2f(xpos, ypos));
	}

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        widget->getCamera()->translateCamera(Eigen::Vector2f(xpos, ypos));
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
    {
        widget->getLight()->rotateCamera(Eigen::Vector2f(xpos, ypos));
    }

}

static void mouseWheelCallback (GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        widget->getCamera()->increaseZoom(1.05);
    }
    else if (yoffset < 0)
    {
        widget->getCamera()->increaseZoom(1.0/1.05);
    }
}


int main(int argc, char *argv[])
{
	GLFWwindow* main_window;

	if (!glfwInit()) 
	{
    	std::cerr << "Failed to init glfw" << std::endl;
		return 1;
	}

	// double x dimension for splitview and add margin
	main_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TUCANO :: Rendering Examples", NULL, NULL);
	if (!main_window)
	{
		std::cerr << "Failed to create the GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(main_window);
	glfwSetKeyCallback(main_window, keyCallback);
	glfwSetMouseButtonCallback(main_window, mouseButtonCallback);
	glfwSetCursorPosCallback(main_window, cursorPosCallback);
    glfwSetScrollCallback(main_window, mouseWheelCallback);

	glfwSetInputMode(main_window, GLFW_STICKY_KEYS, true);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	initialize();


	while (!glfwWindowShouldClose(main_window))
	{
		glfwMakeContextCurrent(main_window);
		widget->render();
		glfwSwapBuffers( main_window );

		glfwPollEvents();
	}

	glfwDestroyWindow(main_window);
	glfwTerminate();
	return 0;
}
