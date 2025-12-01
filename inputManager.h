#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
    void updateKey(int key, int action);
    void updateMouseButton(int button, int action);
    void updateMousePosition(double xpos, double ypos);
    void updateScroll(double yoffset);

    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;

    double getMouseX() const { return mouseX; }
    double getMouseY() const { return mouseY; }
    double getScrollY() const { return scrollY; }

    void resetScroll() { scrollY = 0.0; }

private:
    std::unordered_map<int, bool> keyState;
    std::unordered_map<int, bool> mouseButtonState;
    double mouseX = 0.0, mouseY = 0.0;
    double scrollY = 0.0;
};

#endif // INPUTMANAGER_H
