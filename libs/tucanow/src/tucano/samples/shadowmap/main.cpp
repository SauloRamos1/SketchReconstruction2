#include <iostream>
#include <tucano/tucano.hpp>
#include <simplewidget.hpp>
#include "GLFW/glfw3.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

ModelWidget* widget;

void initialize (void)
{
    Tucano::Misc::initGlew();
	widget = new ModelWidget();	
    
    widget->setShaderDir("../samples/shadowmap/shaders/");

	widget->initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    widget->openMeshFile("../samples/models/toy.ply");

    cout << endl << endl << " ************ usage ************** " << endl;
    cout << "Left mouse button : rotate trackball" << endl;
    cout << "Right mouse button : translate trackball" << endl;
    cout << "Midel mouse button : change light direction" << endl;
    cout << endl;
    cout << "R : reset trackball and light direction" << endl;
    cout << "K : toggle multisampling" << endl;
    cout << "P : set reference camera equal to observer camera" << endl;
    cout << endl;
    cout << "O : increase number of splits" << endl;
    cout << "I : decrease number of splits" << endl;
    cout << endl;
    cout << "X : focus on next split" << endl;
    cout << "Z : focus on previous split" << endl;
    cout << endl;
    cout << "M : show focus split's buffer" << endl;
    cout << "B : show focus split's frustrum" << endl;
    cout << "N : show focus split's bounding box" << endl;
    cout << " ***************** " << endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);	
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        widget->reset();
    }
    if (key == GLFW_KEY_O && action == GLFW_RELEASE)
    {
        widget->addSlice();
    }
    else if (key == GLFW_KEY_I && action == GLFW_RELEASE)
    {
        widget->removeSlice();
    }
    else if (key == GLFW_KEY_P && action == GLFW_RELEASE)
    {
        widget->setCamera();
    }
    else if (key == GLFW_KEY_X && action == GLFW_RELEASE)
    {
        widget->nextFocusSplit();
    }
    else if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
    {
        widget->previousFocusSplit();
    }
    else if (key == GLFW_KEY_K && action == GLFW_RELEASE)
    {
        widget->switchMultisampling();
    }
    else if (key == GLFW_KEY_M && action == GLFW_RELEASE)
    {
        widget->switchRenderBuffer();
    }    
    else if (key == GLFW_KEY_N && action == GLFW_RELEASE)
    {
        widget->switchRenderFrustrum();
    }    
    else if (key == GLFW_KEY_B && action == GLFW_RELEASE)
    {
        widget->switchRenderBBox();
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
    if (widget->getGUI()->cursorMove (xpos, ypos))
        return;

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
	main_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TUCANO :: Flythrough Camera", NULL, NULL);
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
