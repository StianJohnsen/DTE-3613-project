// FOR SKYBOX: https://freestylized.com/all-skybox/

// ENDED UP USING THIS ONE https://freestylized.com/skybox/sky_20/


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <vector>

#include "meshCreation.h"
#include "meshPipelineSetup.h"
#include "processShader.h"
#include "processVertices.h"
#include "shader.h"
#include "camera.h"
// #include "particleSystem.h"

#include "renderer.h"

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <SOIL2/SOIL2.h>

// WINDOWS
#define SHADER_PATH(base) ("../../DTE-3613_part_2/shaders/" base)

// LINUX
// #define SHADER_PATH(base) ("../dte3611_project/shaders/" base)


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseCaptured = true;

Camera camera;


int main()
{

    Renderer renderer(800,600,"DTE-3613: Marble Game");
    if (!renderer.init())
        return -1;
    renderer.run();

    return 0;

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Don't process mouse movement when not captured
    if (!mouseCaptured)
        return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !mouseCaptured)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true;  // reset before re-enabling movement
        mouseCaptured = true;
    }
}

