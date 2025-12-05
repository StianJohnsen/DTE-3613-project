#include "renderer.h"

// WINDOWS
#define SHADER_PATH(base) ("../../DTE-3613_part_2/shaders/" base)
#define ASSETS_SKYBOX_PATH(base) ("../../DTE-3613_part_2/assets/skybox/" base)
#define ASSETS_TEXTURE_PATH(base) ("../../DTE-3613_part_2/assets/textures/" base)

Renderer::Renderer(int width, int height, const std::string& title)
    : width(width), height(height), title(title),
    lastX(width / 2.0f), lastY(height / 2.0f) {}

Renderer::~Renderer() {

    if (dynamicsWorld) {
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
    }

    for (btCollisionShape* shape : collisionShapes) {
        delete shape;
    }
    collisionShapes.clear();

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;

    glfwTerminate();
}

bool Renderer::init() {
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window,[](GLFWwindow* win, int width, int height){
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
        renderer->framebuffer_size_callback(win,width,height);
        renderer->width = width;
        renderer->height = height;
    }
                                   );

    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
        renderer->inputManager.updateKey(key, action);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
        renderer->inputManager.updateMouseButton(button, action);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !renderer->mouseCaptured) {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderer->firstMouse = true;
            renderer->mouseCaptured = true;
        }
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(win));

        if (!r->mouseCaptured)
            return;

        if (r->firstMouse) {
            r->lastX = xpos;
            r->lastY = ypos;
            r->firstMouse = false;
            return;
        }

        float dx = xpos - r->lastX;
        float dy = r->lastY - ypos;

        r->lastX = xpos;
        r->lastY = ypos;

        r->camera.ProcessMouseMovement(dx, dy);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
        renderer->inputManager.updateScroll(yoffset);
    });


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    shaderManager.load("sphere", SHADER_PATH("sphere.vert"), SHADER_PATH("sphere.frag"));
    shaderManager.load("skybox", SHADER_PATH("skybox.vert"), SHADER_PATH("skybox.frag"));

    trackTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("WoodFloor.png")));
    trackTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Road007.png")));
    trackTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric004.png")));
    trackTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Bricks094.png")));
    trackTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("WoodFloor.png")));

    sphereTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric010.png")));
    sphereTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric014.png")));
    sphereTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Plastic005.png")));
    sphereTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Plastic002.png")));

    obstacleTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Marble006.png")));
    obstacleTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Marble021.png")));
    obstacleTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Onyx013.png")));
    obstacleTextures.push_back(texutils::loadTexture(ASSETS_TEXTURE_PATH("Onyx015.png")));

    int   numBoards   = 6;
    float length      = 25.0f;
    float slopeDeg[2] = { +12.0f, -12.0f };

    glm::vec3 pos = glm::vec3(0, 20, 0);

    const int   nLanes       = 4;
    const float sphereRadius = 1.0f;
    const float leftMargin   = 0.5f;
    const float rightMargin  = 0.5f;
    const int   rowsPerBoard = 4;

    int currentObstacleType = 0;

    for (int i = 0; i < numBoards; ++i)
    {
        float slope = glm::radians(slopeDeg[i % 2]);
        bool slopesDown = (slope < 0.0f);
        float offset = 0.25f;

        TrackPiece* p = new FlatBoardPiece(length, slope);

        glm::vec3 fwd = glm::normalize(
            glm::rotate(glm::vec3(0,0,1), slope, glm::vec3(1,0,0))
            );

        p->generateControlPoints(pos, fwd);
        p->build();

        MeshInstance board = createFlatBoardLocal(p->smoothedPath,p->trackRadius * 2.f);

        Renderable r;
        r.mesh  = board.mesh;
        r.model = board.model;
        r.doubleSided = true;
        GLuint boardTexture = trackTextures[i % trackTextures.size()];
        r.textureID = boardTexture;
        r.useTexture = true;

        trackPieces.push_back(p);
        staticObjects.push_back(r);
        createStaticTriangleMeshFromMeshWithTransform(r.mesh,r.model);

        if(i == 0){

            {
                float t = 0.257f;

                glm::vec3 center, T, R, U;
                p->getPointAndFrame(t, center, T, R, U);

                glm::mat4 basis(1.0f);
                basis[0] = glm::vec4(R,0);
                basis[1] = glm::vec4(U,0);
                basis[2] = glm::vec4(T,0);

                float length     = 4.0f;
                float innerStart = 0.5f;
                float innerEnd   = p->trackRadius * 0.5f;
                float height     = 1.0f;

                Mesh guide = createGuideDeflectorLocal(length, innerStart, innerEnd, height);

                glm::vec3 leftWallInside  = center + R * p->trackRadius;
                glm::vec3 rightWallInside = center - R * p->trackRadius;


                {
                    glm::mat4 model =
                        glm::translate(glm::mat4(1), leftWallInside) *
                        basis;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    r.textureID = texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric004.png"));
                    r.useTexture = true;

                    staticObjects.push_back(r);

                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);
                }


                {
                    glm::mat4 mirror = glm::scale(glm::mat4(1), glm::vec3(-1,1,1));

                    glm::mat4 model =
                        glm::translate(glm::mat4(1), rightWallInside) *
                        basis *
                        mirror;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    r.textureID = texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric004.png"));
                    r.useTexture = true;
                    staticObjects.push_back(r);
                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);

                }
            }

            {
                glm::vec3 center, T, R, U;
                float t = 0.955f;
                p->getPointAndFrame(t, center, T, R, U);

                glm::mat4 basis(1.0f);
                basis[0] = glm::vec4(R,0);
                basis[1] = glm::vec4(U,0);
                basis[2] = glm::vec4(T,0);

                float length     = 4.0f;
                float innerStart = 0.5f;
                float innerEnd   = p->trackRadius * 0.5f;
                float height     = 1.0f;

                Mesh guide = createGuideDeflectorLocal(length, innerStart, innerEnd, height);

                glm::vec3 posLeft  = center;
                glm::vec3 posRight = center;

                {
                    glm::mat4 model =
                        glm::translate(glm::mat4(1), posLeft) *
                        basis;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    r.textureID = texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric004.png"));
                    r.useTexture = true;



                    // UNCOMMENT TO SEE AN ADDITIONAL OBSTACLE

                    // staticObjects.push_back(r);
                    // createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);



                }

                {
                    glm::mat4 mirror = glm::scale(glm::mat4(1), glm::vec3(-1,1,1));

                    glm::mat4 model =
                        glm::translate(glm::mat4(1), posRight) *
                        basis *
                        mirror;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    r.textureID = texutils::loadTexture(ASSETS_TEXTURE_PATH("Fabric004.png"));
                    r.useTexture = true;



                    // UNCOMMENT TO SEE AN ADDITIONAL OBSTACLE



                    // staticObjects.push_back(r);
                    // createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);





                }
            }
        }

        int boardType = currentObstacleType % 4;
        currentObstacleType++;

        if (i > 0 && i < numBoards - 1)
        {

            int M = p->smoothedPath.size();
            float halfW = p->trackRadius;

            std::vector<float> laneX(nLanes);
            float usable = (2.f * halfW) - leftMargin - rightMargin - 2.f * sphereRadius;

            for (int k = 0; k < nLanes; ++k) {
                float tLane = (nLanes == 1) ? 0.5f : float(k) / float(nLanes-1);
                laneX[k] = -halfW + leftMargin + sphereRadius + tLane * usable;
            }

            float laneSpacing = (nLanes > 1) ? (laneX[1] - laneX[0]) : 0.f;

            float upShift = 0.12f * length;

            for (int row = 0; row < rowsPerBoard; ++row){

                int idx = int(float(row + 1) / float(rowsPerBoard + 1) * M);


                idx = std::clamp(idx, 0, M - 1);

                glm::vec3 center = p->smoothedPath[idx];

                glm::vec3 T, R, U;
                getFrameAtIndex(p->smoothedPath, idx, T, R, U);

                glm::mat4 basis(1.0f);
                basis[0] = glm::vec4(R, 0);
                basis[1] = glm::vec4(U, 0);
                basis[2] = glm::vec4(T, 0);

                int obstacles = nLanes - 1;

                for (int k = 0; k < obstacles; ++k){
                    float xPos = 0.5f * (laneX[k] + laneX[k+1]);

                    float stagger = (row % 2 == 0) ? -0.25f * laneSpacing
                                                   : +0.25f * laneSpacing;
                    xPos += stagger;

                    Mesh mesh;
                    float height = 2.5f;

                    switch(boardType)
                    {
                    case 0: mesh = createWall(1.f, 2.5f, 0.25f); break;
                    case 1: mesh = createCylinder(0.5f, 2.5f, 16); break;
                    case 2: mesh = createPyramid(1.5f, 2.0f); height = 2.0f; break;
                    case 3: mesh = createTriangularPrism(1.5f, 2.5f, 1.0f); break;
                    }

                    float boardThickness = 0.05f;
                    float lift = height*0.5f + boardThickness;

                    glm::vec3 obsPos =
                        center +
                        R * xPos +
                        T * upShift +
                        U * lift;

                    glm::mat4 model =
                        glm::translate(glm::mat4(1.0f), obsPos) *
                        basis;

                    Renderable obstacle;
                    obstacle.mesh = mesh;
                    obstacle.model = model;
                    obstacle.doubleSided = true;
                    obstacle.textureID = obstacleTextures[
                        currentObstacleType % obstacleTextures.size()
                    ];

                    obstacle.useTexture = true;
                    staticObjects.push_back(obstacle);
                    createStaticTriangleMeshFromMeshWithTransform(obstacle.mesh, obstacle.model);
                }
            }
        }

        int last = p->smoothedPath.size() - 1;
        glm::vec3 T, R, U;
        getFrameAtIndex(p->smoothedPath, last, T, R, U);

        glm::vec3 wallPos =
            slopesDown
                ? p->smoothedPath[last] - T * offset
                : p->smoothedPath[0]    + T * offset;



        float wallWidth     = p->trackRadius * 2.f;
        float wallHeight    = 3.f;
        float wallThickness = 0.25f;

        glm::mat4 rot(1.f);
        rot[0] = glm::vec4(R,0);
        rot[1] = glm::vec4(U,0);
        rot[2] = glm::vec4(T,0);

        glm::mat4 wallModel =
            glm::translate(glm::mat4(1.0f), wallPos) *
            rot;


        Renderable wall;
        wall.mesh = createEndWall(wallWidth, wallHeight, wallThickness);
        wall.model = wallModel;
        wall.doubleSided = true;

        wall.textureID = boardTexture;
        wall.useTexture = true;
        staticObjects.push_back(wall);
        createStaticTriangleMeshFromMeshWithTransform(wall.mesh, wall.model);

        pos = p->smoothedPath.back();
        pos.y -= 15.f;

        pos.z = (i % 2 == 0) ? 15.f : 0.f;

    }

    const int nSpheres = nLanes;
    const float radius = sphereRadius;

    Mesh sphereMesh = createSphere(radius, 36, 18);

    TrackPiece* startBoard = trackPieces[0];
    int M = startBoard->smoothedPath.size();

    int idx0 = std::min(5, M - 1);
    glm::vec3 center = startBoard->smoothedPath[idx0];

    glm::vec3 T0, R0, U0;
    getFrameAtIndex(startBoard->smoothedPath, idx0, T0, R0, U0);

    std::vector<float> laneX(nLanes);
    float halfW   = startBoard->trackRadius;
    float usable  = (2.f * halfW) - leftMargin - rightMargin - 2.f * radius;

    for (int k = 0; k < nLanes; ++k) {
        float tLane = (nLanes == 1) ? 0.5f : float(k) / float(nLanes - 1);
        laneX[k] = -halfW + leftMargin + radius + tLane * usable;
    }

    for (int k = 0; k < nSpheres; ++k) {

        glm::vec3 pos =
            center +
            R0 * laneX[k] +
            U0 * (radius + 0.1f);

        glm::mat4 model = glm::translate(glm::mat4(1.f), pos);

        Renderable sphereObj;
        sphereObj.mesh = sphereMesh;
        sphereObj.model = model;
        sphereObj.doubleSided = true;
        sphereObj.textureID = sphereTextures[k % sphereTextures.size()];
        sphereObj.useTexture = true;

        dynamicObjects.push_back(sphereObj);
        rigidBodyToRenderable.push_back(dynamicObjects.size() - 1);

        btRigidBody* body = createSphereRigidBody(radius, model, 1.0f);
        physicsBodies.push_back(body);

    }

    Light light1;
    light1.position = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.7f)); // direction
    light1.ambient = glm::vec3(0.35);
    light1.diffuse  = glm::vec3(0.7f);
    light1.specular = glm::vec3(0.05f);

    Light light2;
    light2.position = glm::normalize(glm::vec3(0.5f, -0.3f, 0.9f));
    light2.ambient  = glm::vec3(0.25f);
    light2.diffuse  = glm::vec3(0.25f);
    light2.specular = glm::vec3(0.05f);
    light2.position = glm::vec3(-3.0f, 1.0f, -2.0f);
    light2.diffuse  = glm::vec3(0.2f, 0.6f, 1.0f);

    Light light3;
    light3.position = glm::vec3(3.0f, 1.5f, -1.0f);
    light3.diffuse  = glm::vec3(0.8f, 1.0f, 0.5f);

    lights.push_back(light1);
    lights.push_back(light2);
    lights.push_back(light3);


    std::vector<std::string> faces = {
        ASSETS_SKYBOX_PATH("right.png"),   // +X
        ASSETS_SKYBOX_PATH("left.png"),    // -X
        ASSETS_SKYBOX_PATH("top.png"),     // +Y
        ASSETS_SKYBOX_PATH("bottom.png"),  // -Y
        ASSETS_SKYBOX_PATH("front.png"),   // +Z
        ASSETS_SKYBOX_PATH("back.png")     // -Z
    };

    skybox.load(faces);

    Shader& skyboxShader = shaderManager.get("skybox");
    skyboxShader.use();

    skyboxShader.setInt("skybox",0);

    Shader& shader = shaderManager.get("sphere");
    shader.use();
    shader.setInt("texture_diffuse", 0);
    shader.setBool("useTexture",true);


    if (!physicsBodies.empty()) {


        // WHICH MARBLE DO YOU CHOOSE?
        int sphereIdx = playerSphereIndex;

        btTransform trans;
        physicsBodies[sphereIdx]->getMotionState()->getWorldTransform(trans);

        glm::mat4 sphereModel = btTransformToGlm(trans);
        glm::vec3 spherePos   = glm::vec3(sphereModel[3]);

        glm::vec3 camOffset(-6.0f, 3.0f, -10.0f);

        camera.Position = spherePos + camOffset;
        camera.Front    = glm::normalize(spherePos - camera.Position);
        camera.Right    = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
        camera.Up       = glm::normalize(glm::cross(camera.Right, camera.Front));

        followSphere = true;
        mouseCaptured = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true;
    }
    else {
        followSphere = false;
    }

    return true;
}

void Renderer::run() {
    while (!glfwWindowShouldClose(window)) {
        updateDeltaTime();
        processInput();
        renderFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Renderer::updateDeltaTime() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Renderer::processInput() {

    static bool fWasPressed = false;
    static bool pWasPressed = false;

    bool fPressed = inputManager.isKeyPressed(GLFW_KEY_F);
    bool pPressed = inputManager.isKeyPressed(GLFW_KEY_P);

    if (pPressed && !pWasPressed) {
        physicsEnabled = !physicsEnabled;
    }
    pWasPressed = pPressed;

    if (fPressed && !fWasPressed) {
        followSphere = !followSphere;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCaptured = true;

        firstMouse = true;

        if (!followSphere) {
            camera.Yaw   = glm::degrees(atan2(camera.Front.z, camera.Front.x));
            camera.Pitch = glm::degrees(asin(camera.Front.y));
        }
    }
    fWasPressed = fPressed;

    if (!followSphere) {
        if (inputManager.isKeyPressed(GLFW_KEY_W))
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (inputManager.isKeyPressed(GLFW_KEY_S))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (inputManager.isKeyPressed(GLFW_KEY_A))
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (inputManager.isKeyPressed(GLFW_KEY_D))
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (inputManager.isKeyPressed(GLFW_KEY_SPACE))
            camera.ProcessKeyboard(UP, deltaTime);
        if (inputManager.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
            camera.ProcessKeyboard(DOWN, deltaTime);
    }


    if (inputManager.isKeyPressed(GLFW_KEY_ESCAPE) && mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseCaptured = false;
    }


    if (inputManager.getScrollY() != 0.0) {
        camera.ProcessMouseScroll(inputManager.getScrollY());
        inputManager.resetScroll();
    }
}

void Renderer::renderFrame() {
    glClearColor(0.2f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(followSphere && !physicsBodies.empty()){

        int sphereIdx = 0;

        btTransform trans;
        physicsBodies[sphereIdx]->getMotionState()->getWorldTransform(trans);
        glm::mat4 sphereModel = btTransformToGlm(trans);

        glm::vec3 spherePos = glm::vec3(sphereModel[3]);

        glm::vec3 camOffset(-6.0f, 3.0f, -10.0f);

        camera.Position = spherePos + camOffset;

        camera.Front = glm::normalize(spherePos - camera.Position);

        camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
        camera.Up    = glm::normalize(glm::cross(camera.Right, camera.Front));

    }

    if (dynamicsWorld) {

        if(physicsEnabled){

            for (auto* body : physicsBodies) {
                body->activate(true);
                body->setActivationState(ACTIVE_TAG);
            }

            dynamicsWorld->stepSimulation(deltaTime, 10);

            for (size_t i = 0; i < physicsBodies.size(); i++) {

                btRigidBody* body = physicsBodies[i];

                btTransform trans;
                body->getMotionState()->getWorldTransform(trans);

                int renderIndex = rigidBodyToRenderable[i];

                dynamicObjects[renderIndex].model = btTransformToGlm(trans);
            }

        }
        else{
            for (auto* body : physicsBodies)
                body->setActivationState(WANTS_DEACTIVATION);
        }

        glDisable(GL_CULL_FACE);
    }

    Shader& sphereShader = shaderManager.get("sphere");
    sphereShader.use();

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom),
        (float)width / (float)height,
        0.1f,
        100.0f
        );

    glm::mat4 view = camera.GetViewMatrix();

    sphereShader.setMat4("view", camera.GetViewMatrix());
    sphereShader.setMat4("projection", projection);
    sphereShader.setVec3("viewPos", camera.Position);

    sphereShader.setInt("numLights", lights.size());

    for(size_t i = 0; i < lights.size(); ++i){
        lights[i].apply(sphereShader,"lights",i);
    }

    glDisable(GL_CULL_FACE);

    for(auto& r: dynamicObjects){
        r.draw(sphereShader);
    }

    for(auto& r: staticObjects){
        r.draw(sphereShader);
    }

    glDepthFunc(GL_LEQUAL);

    Shader& skyboxShader = shaderManager.get("skybox");
    skybox.draw(skyboxShader,view,projection);

    glDepthFunc(GL_LESS);

}

glm::mat4 Renderer::btTransformToGlm(const btTransform& t) const {
    btScalar m[16];
    t.getOpenGLMatrix(m);
    glm::mat4 mat;
    for (int i=0;i<16;++i) mat[i/4][i%4] = (float)m[i];
    return mat;
}

btTransform Renderer::glmToBtTransform(const glm::mat4& m) const {
    glm::vec3 origin = glm::vec3(m[3][0], m[3][1], m[3][2]);
    btMatrix3x3 basis(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]
        );
    btTransform t(basis, btVector3(origin.x, origin.y, origin.z));
    return t;
}

btRigidBody* Renderer::createStaticTriangleMeshFromMesh(const Mesh& mesh) {
    if (mesh.indices.empty() || mesh.vertices.empty()) return nullptr;

    btTriangleMesh* triMesh = new btTriangleMesh();

    const size_t strideFloats = 11;
    const size_t vertexCount = mesh.vertices.size() / strideFloats;

    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
        unsigned int ia = mesh.indices[i + 0];
        unsigned int ib = mesh.indices[i + 1];
        unsigned int ic = mesh.indices[i + 2];

        if (ia >= vertexCount || ib >= vertexCount || ic >= vertexCount) continue;

        const float* a = &mesh.vertices[ia * strideFloats];
        const float* b = &mesh.vertices[ib * strideFloats];
        const float* c = &mesh.vertices[ic * strideFloats];

        btVector3 va(a[0], a[1], a[2]);
        btVector3 vb(b[0], b[1], b[2]);
        btVector3 vc(c[0], c[1], c[2]);

        triMesh->addTriangle(va, vb, vc);
    }

    btBvhTriangleMeshShape* triShape = new btBvhTriangleMeshShape(triMesh, true);
    triShape->setLocalScaling(btVector3(1,1,1));

    collisionShapes.push_back(triShape);
    collisionShapes.push_back(reinterpret_cast<btCollisionShape*>(triMesh)); // hack: keep pointer list for deletion

    btTransform identity;
    identity.setIdentity();
    btScalar mass(0.0f);
    btVector3 localInertia(0,0,0);

    btDefaultMotionState* motion = new btDefaultMotionState(identity);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, triShape, localInertia);
    btRigidBody* body = new btRigidBody(info);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

    dynamicsWorld->addRigidBody(body);
    return body;
}

btRigidBody* Renderer::createSphereRigidBody(float radius, const glm::mat4& startTransform, float mass) {
    btCollisionShape* sphereShape = new btSphereShape(radius);
    collisionShapes.push_back(sphereShape);

    btTransform start = glmToBtTransform(startTransform);

    btVector3 localInertia(0,0,0);
    if (mass > 0.0f) sphereShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(start);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, sphereShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setFriction(0.5f);
    body->setRollingFriction(0.02f);
    body->setRestitution(0.1f);

    dynamicsWorld->addRigidBody(body);
    return body;
}


void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

btRigidBody* Renderer::createStaticTriangleMeshFromMeshWithTransform(
    const Mesh& mesh, const glm::mat4& modelMatrix)
{
    if (mesh.indices.empty() || mesh.vertices.empty()) return nullptr;

    btTriangleMesh* triMesh = new btTriangleMesh();
    const size_t strideFloats = 11;

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        unsigned ia = mesh.indices[i+0];
        unsigned ib = mesh.indices[i+1];
        unsigned ic = mesh.indices[i+2];

        const float* a = &mesh.vertices[ia * strideFloats];
        const float* b = &mesh.vertices[ib * strideFloats];
        const float* c = &mesh.vertices[ic * strideFloats];

        glm::vec3 pa = glm::vec3(modelMatrix * glm::vec4(a[0],a[1],a[2],1));
        glm::vec3 pb = glm::vec3(modelMatrix * glm::vec4(b[0],b[1],b[2],1));
        glm::vec3 pc = glm::vec3(modelMatrix * glm::vec4(c[0],c[1],c[2],1));

        triMesh->addTriangle(
            btVector3(pa.x, pa.y, pa.z),
            btVector3(pb.x, pb.y, pb.z),
            btVector3(pc.x, pc.y, pc.z)
            );

    }

    auto* shape = new btBvhTriangleMeshShape(triMesh, true);
    collisionShapes.push_back(shape);

    btTransform t;
    t.setIdentity();

    btDefaultMotionState* motion = new btDefaultMotionState(t);

    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion, shape);
    btRigidBody* body = new btRigidBody(info);

    body->setCollisionFlags(
        body->getCollisionFlags() |
        btCollisionObject::CF_STATIC_OBJECT
        );

    body->setFriction(1.0f);
    body->setRollingFriction(0.0f);
    body->setRestitution(0.1f);

    body->setActivationState(DISABLE_DEACTIVATION);

    dynamicsWorld->addRigidBody(body);
    return body;
}

void Renderer::getFrameAtIndex(
    const std::vector<glm::vec3> &path,
    int i,
    glm::vec3 &T,
    glm::vec3 &R,
    glm::vec3 &U)
{
    int N = path.size();

    if (N < 2) {
        std::cout << "[FRAME ERROR] path.size() = " << N
                  << " — cannot compute frame\n";
        T = glm::vec3(0,0,1);
        R = glm::vec3(1,0,0);
        U = glm::vec3(0,1,0);
        return;
    }



    if (i == 0) {
        // std::cout << "  Using forward diff: path[1] - path[0]\n";
        T = glm::normalize(path[1] - path[0]);
    }
    else if (i == N - 1) {
        // std::cout << "  Using backward diff: path[N-1] - path[N-2]\n";
        T = glm::normalize(path[N - 1] - path[N - 2]);
    }
    else {
        // std::cout << "  Using central diff: path[i+1] - path[i-1]\n";
        T = glm::normalize(path[i + 1] - path[i - 1]);
    }

    glm::vec3 worldUp(0,1,0);
    if (fabs(glm::dot(worldUp, T)) > 0.9f) {
        // std::cout << "  WARNING: worldUp parallel to T — using fallback axis\n";
        worldUp = glm::vec3(1,0,0);
    }

    R = glm::normalize(glm::cross(worldUp, T));
    U = glm::normalize(glm::cross(T, R));
}
