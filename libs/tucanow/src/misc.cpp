#include <iostream>

#include <GL/glew.h>

#include "tucanow/misc.hpp"


namespace tucanow {
namespace misc {


void initGlew()
{
    glewExperimental = true;
    GLenum glewInitResult = glewInit();
    if (GLEW_OK != glewInitResult)
    {
        std::cerr << "\nError: " << glewGetErrorString(glewInitResult) << std::endl << std::flush;
        exit(EXIT_FAILURE);
    }
}

}
}
