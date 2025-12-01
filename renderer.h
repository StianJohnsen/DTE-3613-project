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
#include "hermiteClass.h"
#include "trackPiece.h"
#include "flatBoardPiece.h"
#include "loopPiece.h"
#include "spiralPiece.h"
#include "loopEntrancePiece.h"
#include "transitionPiece.h"
#include "clothoidTransitionPiece.h"
#include "verticalTransitionPiece.h"
#include "bulletDebugDrawer.h"

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


    // Shader* shader = nullptr;
    ShaderManager shaderManager;
    Skybox skybox;
    Mesh sphere;
    Renderable track;
    // Renderable debugIDontKnow;
    Camera camera;

    std::vector<TrackPiece*> trackPieces;
    Renderable combinedTrack;

    std::vector<Renderable> debugRenderables;

    std::vector<Renderable> tracks_Testing;


    // Light light;
    Material material;
    std::vector<Renderable> staticObjects;
    std::vector<Renderable> dynamicObjects;


    int winnerIndex = -1;

    int playerSphereIndex = 0;
    bool playerWon = false;

    bool raceFinished = false;
    std::vector<Light> lights;

    HermiteTrack hermiteTrack;

    // Physics: Bullet members
    btBroadphaseInterface* broadphase = nullptr;
    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    BulletDebugDrawer* debugDrawer = nullptr;

    // keep track of allocated collision shapes & bodies to cleanup
    std::vector<btCollisionShape*> collisionShapes;
    std::vector<btRigidBody*> physicsBodies; // corresponds to dynamicObjects indices
    std::vector<unsigned int> rigidBodyToRenderable;

    glm::vec3 extractPosition(const glm::mat4& m);



    // helper functions
    btRigidBody* createStaticTriangleMeshFromMesh(const Mesh& mesh);
    btRigidBody* createSphereRigidBody(float radius, const glm::mat4& startTransform, float mass = 1.0f);
    btRigidBody* createStaticTriangleMeshFromMeshWithTransform(
        const Mesh& mesh, const glm::mat4& modelMatrix);

    glm::mat4 btTransformToGlm(const btTransform& t) const;
    btTransform glmToBtTransform(const glm::mat4& m) const;

    void addObstacle(
        const Mesh& mesh,
        const glm::vec3& position,
        const glm::mat4& basis,
        const glm::vec3& scale);

    glm::mat4 buildBasis(const glm::vec3& N,
                                   const glm::vec3& B,
                                   const glm::vec3& T);



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

    float extractRoll(const glm::vec3& N, const glm::vec3& U, const glm::vec3& worldUp);
    float computeRollBetween(const glm::vec3& R0,
                                       const glm::vec3& U0,
                                       const glm::vec3& T,
                             const glm::vec3& R1);


    float computeRoll(const glm::vec3& right,
                      const glm::vec3& up,
                      const glm::vec3& tangent);


    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

#endif // RENDERER_H
