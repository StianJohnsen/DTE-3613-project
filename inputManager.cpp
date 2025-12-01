#include "inputManager.h"

void InputManager::updateKey(int key, int action) {
    if (action == GLFW_PRESS)
        keyState[key] = true;
    else if (action == GLFW_RELEASE)
        keyState[key] = false;
}

void InputManager::updateMouseButton(int button, int action) {
    if (action == GLFW_PRESS)
        mouseButtonState[button] = true;
    else if (action == GLFW_RELEASE)
        mouseButtonState[button] = false;
}

void InputManager::updateMousePosition(double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

void InputManager::updateScroll(double yoffset) {
    scrollY = yoffset;
}

bool InputManager::isKeyPressed(int key) const {
    auto it = keyState.find(key);
    return it != keyState.end() && it->second;
}

bool InputManager::isMouseButtonPressed(int button) const {
    auto it = mouseButtonState.find(button);
    return it != mouseButtonState.end() && it->second;
}
