#include <iostream>
#include <tucano/tucano.hpp>
#include <texturewidget.hpp>
#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 335

TextureWidget* widget;

void initialize (void)
{
    Tucano::Misc::initGlew();
	widget = new TextureWidget();	
	widget->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);	
}

static void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    widget->resize(width, height);
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
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (widget->getGUI()->cursorMove (xpos, ypos))
        return;
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
	main_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TUCANO :: Basic Image Processing", NULL, NULL);
	if (!main_window)
	{
		std::cerr << "Failed to create the GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(main_window);
	glfwSetKeyCallback(main_window, keyCallback);
    glfwSetWindowSizeCallback(main_window, windowSizeCallback);
	glfwSetMouseButtonCallback(main_window, mouseButtonCallback);
	glfwSetCursorPosCallback(main_window, cursorPosCallback);
 
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
