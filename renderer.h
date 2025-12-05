#ifndef RENDERER_H
#define RENDERER_H

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

#include <vector>
#include "shader.h"
#include "meshCreation.h"
#include "camera.h"
#include "inputManager.h"
#include "light.h"
#include "material.h"
#include "renderable.h"
#include "shaderManager.h"
#include "skybox.h"
#include "trackPiece.h"
#include "flatBoardPiece.h"
#include "texture_utils.h"

class Renderer {
public:
    Renderer(int width, int height, const std::string& title);
    ~Renderer();

    bool init();
    void run();

    GLFWwindow* getWindow() const { return window; }

private:
    int width, height;
    std::string title;
    GLFWwindow* window = nullptr;

    InputManager inputManager;

    bool physicsEnabled = true;

    bool followSphere = false;

    std::vector<GLuint> sphereTextures;
    std::vector<GLuint> trackTextures;
    std::vector<GLuint> obstacleTextures;

    ShaderManager shaderManager;
    Skybox skybox;
    Mesh sphere;
    Camera camera;

    std::vector<TrackPiece*> trackPieces;

    std::vector<Renderable> staticObjects;
    std::vector<Renderable> dynamicObjects;

    int winnerIndex = -1;

    int playerSphereIndex = 0;
    bool playerWon = false;

    bool raceFinished = false;
    std::vector<Light> lights;


    btBroadphaseInterface* broadphase = nullptr;
    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

    std::vector<btCollisionShape*> collisionShapes;
    std::vector<btRigidBody*> physicsBodies;
    std::vector<unsigned int> rigidBodyToRenderable;


    btRigidBody* createStaticTriangleMeshFromMesh(const Mesh& mesh);
    btRigidBody* createSphereRigidBody(float radius, const glm::mat4& startTransform, float mass = 1.0f);
    btRigidBody* createStaticTriangleMeshFromMeshWithTransform(
        const Mesh& mesh, const glm::mat4& modelMatrix);

    glm::mat4 btTransformToGlm(const btTransform& t) const;
    btTransform glmToBtTransform(const glm::mat4& m) const;

    void getFrameAtIndex(
        const std::vector<glm::vec3> &path,
        int i,
        glm::vec3 &T,
        glm::vec3 &R,
        glm::vec3 &U);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    float trackProgress = 0.0f;
    float trackSpeed = 0.05f;

    bool mouseCaptured = true;
    bool firstMouse = true;
    float lastX, lastY;

    void processInput();
    void updateDeltaTime();
    void renderFrame();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

#endif // RENDERER_H
