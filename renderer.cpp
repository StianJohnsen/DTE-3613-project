#include "renderer.h"

// #define SHADER_PATH(base) ("../../DTE-3613_part_2/shaders/" base)
// #define ASSETS_SKYBOX_PATH(base) ("../../DTE-3613_part_2/assets/skybox/" base)


// #define SHADER_PATH(base) ("../dte3611_project/shaders/" base)
// #define ASSETS_SKYBOX_PATH(base) ("../dte3611_project/assets/skybox" base)


// WINDOWS
#define SHADER_PATH(base) ("../../DTE-3613_part_2/shaders/" base)
#define ASSETS_SKYBOX_PATH(base) ("../../DTE-3613_part_2/assets/skybox/" base)
#define ASSETS_TEXTURE_PATH(base) ("../../DTE-3613_part_2/assets/textures/" base)



Renderer::Renderer(int width, int height, const std::string& title)
    : width(width), height(height), title(title),
    lastX(width / 2.0f), lastY(height / 2.0f) {}

Renderer::~Renderer() {

    // remove and delete rigid bodies
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

    // delete collision shapes
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


    // Bullet initialization (after GLEW/init)
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // set gravity
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

        // recapture mouse on click
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !renderer->mouseCaptured) {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderer->firstMouse = true;
            renderer->mouseCaptured = true;
        }
    });

    // glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
    //     auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
    //     renderer->inputManager.updateMousePosition(xpos, ypos);

    //     if (!renderer->mouseCaptured) return;

    //     // if(renderer->followSphere) return;

    //     if (renderer->firstMouse) {
    //         renderer->lastX = xpos;
    //         renderer->lastY = ypos;
    //         renderer->firstMouse = false;
    //     }

    //     float xoffset = xpos - renderer->lastX;
    //     float yoffset = renderer->lastY - ypos;
    //     renderer->lastX = xpos;
    //     renderer->lastY = ypos;

    //     renderer->camera.ProcessMouseMovement(xoffset, yoffset);
    // });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        Renderer* r = static_cast<Renderer*>(glfwGetWindowUserPointer(win));

        // If mouse is not captured → ignore movement
        if (!r->mouseCaptured)
            return;

        // Prevent jump on first frame
        if (r->firstMouse) {
            r->lastX = xpos;
            r->lastY = ypos;
            r->firstMouse = false;
            return;
        }

        float dx = xpos - r->lastX;
        float dy = r->lastY - ypos;  // inverted y

        r->lastX = xpos;
        r->lastY = ypos;

        // Allow mouse look in both follow mode AND free mode
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



    int   numBoards   = 6;      // how many flatboards you want
    float length      = 25.0f;
    float slopeDeg[2] = { +12.0f, -12.0f };   // alternate incline
            // vertical spacing

    glm::vec3 pos = glm::vec3(0, 20, 0);      // starting point
                 // toggles between +Z and -Z


    // ---------- SHARED GRID CONFIG (lanes & spheres) ----------
    const int   nLanes            = 4;      // number of lanes (spheres per row)
    const float sphereRadius = 1.0f;
    const float leftMargin   = 0.5f;
    const float rightMargin  = 0.5f;
    const int   rowsPerBoard = 4;  // rows of obstacles on each board (except first/last)


    int currentObstacleType = 0;

    for (int i = 0; i < numBoards; ++i)
    {
        float slope = glm::radians(slopeDeg[i % 2]);
        bool slopesDown = (slope < 0.0f);
        float offset = 0.25f;

        // -------- 1. CREATE TRACK PIECE --------
        TrackPiece* p = new FlatBoardPiece(length, slope);

        glm::vec3 fwd = glm::normalize(
            glm::rotate(glm::vec3(0,0,1), slope, glm::vec3(1,0,0))
            );

        glm::vec3 right = glm::vec3(1,0,0);
        glm::vec3 up    = glm::normalize(glm::cross(fwd, right));

        p->generateControlPoints(pos, fwd);
        p->build();

        // -------- 2. ADD TRACK TO RENDER + PHYSICS --------

        MeshInstance board = createFlatBoardLocal(p->smoothedPath,p->trackRadius * 2.f);

        Renderable r;
        r.mesh  = board.mesh;
        r.model = board.model;
        r.doubleSided = true;
        r.textureID = trackTextures[i % trackTextures.size()];
        r.useTexture = true;

        trackPieces.push_back(p);
        staticObjects.push_back(r);
        createStaticTriangleMeshFromMeshWithTransform(r.mesh,r.model);


        if(i == 0){



            // FIRST PAIR (attached to walls)
            {
                // int idx = p->smoothedPath.size() * 0.35f;
                // std::cout << "idx 1: " << idx << "\n";

                // glm::vec3 center = p->smoothedPath[idx];

                // float t = 0.25f;                        // PERFECT fine control
                // glm::vec3 center = getPointAtT(p->smoothedPath, t);

                // glm::vec3 T, R, U;
                // getFrameAtT(p->smoothedPath, t, T, R, U);
                // getFrameAtIndex(p->smoothedPath, idx, T, R, U);

                float t = 0.257f;

                std::cout << "\n\n\n\n\n";

                std::cout << "----- DEFLECTOR DEBUG -----\n";
                std::cout << "SmoothedPath size = " << p->smoothedPath.size() << "\n";

                std::cout << "Using t = " << t << "\n";

                float fidx = t * (p->smoothedPath.size() - 1);
                int i0 = int(fidx);
                int i1 = std::min(i0 + 1, int(p->smoothedPath.size() - 1));

                std::cout << "Computed fidx = " << fidx << "\n";
                std::cout << "i0 = " << i0 << "  i1 = " << i1 << "\n";

                glm::vec3 P0 = p->smoothedPath[i0];
                glm::vec3 P1 = p->smoothedPath[i1];

                std::cout << "P0 = (" << P0.x << ", " << P0.y << ", " << P0.z << ")\n";
                std::cout << "P1 = (" << P1.x << ", " << P1.y << ", " << P1.z << ")\n";

                glm::vec3 center, T, R, U;
                p->getPointAndFrame(t, center, T, R, U);

                std::cout << "CENTER = (" << center.x << ", " << center.y << ", " << center.z << ")\n";
                std::cout << "T = (" << T.x << ", " << T.y << ", " << T.z << ")\n";
                std::cout << "R = (" << R.x << ", " << R.y << ", " << R.z << ")\n";
                std::cout << "U = (" << U.x << ", " << U.y << ", " << U.z << ")\n";
                std::cout << "---------------------------\n";


                std::cout << "\n\n\n\n\n";



                // glm::vec3 center, T, R, U;
                // float t = 0.25f;   // EXACT control, smoothly moves up/down the board
                // p->getPointAndFrame(t, center, T, R, U);


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

                // LEFT
                {
                    glm::mat4 model =
                        glm::translate(glm::mat4(1), leftWallInside) *
                        basis;


                    // Renderable r;
                    // r.mesh  = board.mesh;
                    // r.model = board.model;
                    // r.doubleSided = true;
                    // r.mesh.textureId = checkersBoardTexture;
                    // r.useTexture = true;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    staticObjects.push_back(r);
                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);
                }

                // RIGHT
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
                    staticObjects.push_back(r);
                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);

                }
            }




            // SECOND PAIR (centered, not attached to walls)
            {
                // int idx = p->smoothedPath.size() * 0.95f;
                // std::cout << "idx 2: " << idx << "\n";

                // std::cout << "smoothedPath.size(): " << p->smoothedPath.size() << "\n";

                // glm::vec3 center = p->smoothedPath[idx];

                // glm::vec3 T, R, U;
                // getFrameAtIndex(p->smoothedPath, idx, T, R, U);
                // float t = 0.95f;                        // PERFECT fine control
                // glm::vec3 center = getPointAtT(p->smoothedPath, t);

                // glm::vec3 T, R, U;
                // getFrameAtT(p->smoothedPath, t, T, R, U);

                glm::vec3 center, T, R, U;
                float t = 0.955f;   // EXACT control, smoothly moves up/down the board
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

                // Here you said: “use the center instead”
                glm::vec3 posLeft  = center;     // NO wall offset
                glm::vec3 posRight = center;

                // LEFT (inner direction = -R)
                {
                    glm::mat4 model =
                        glm::translate(glm::mat4(1), posLeft) *
                        basis;

                    Renderable r;
                    r.mesh = guide;
                    r.model = model;
                    r.doubleSided = true;
                    staticObjects.push_back(r);
                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);
                }

                // RIGHT (mirror X axis)
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
                    staticObjects.push_back(r);
                    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);


                    // auto last = p->smoothedPath.back();
                    // std::cout << "PATH END = (" << last.x << ", " << last.y << ", " << last.z << ")\n";

                }
            }


            // int idx = p->smoothedPath.size() * 0.7f;

            // glm::vec3 center = p->smoothedPath[idx];

            // glm::vec3 T, R, U;
            // getFrameAtIndex(p->smoothedPath, idx, T, R, U);

            // glm::mat4 basis(1.0f);
            // basis[0] = glm::vec4(R,0);
            // basis[1] = glm::vec4(U,0);
            // basis[2] = glm::vec4(T,0);





            // float length     = 4.0f;
            // float innerStart = 0.5f;                  // small
            // float innerEnd   = p->trackRadius * 0.5f; // wide
            // float height     = 1.0f;

            // Mesh guide = createGuideDeflectorLocal(
            //     length,
            //     innerStart,
            //     innerEnd,
            //     height
            //     );

            // glm::vec3 leftWallInside  = center + R * p->trackRadius;
            // glm::vec3 rightWallInside = center - R * p->trackRadius;

            // // LEFT
            // {
            //     glm::mat4 model =
            //         glm::translate(glm::mat4(1), leftWallInside) *
            //         basis;

            //     Renderable g;
            //     g.mesh = guide;
            //     g.model = model;
            //     g.doubleSided = true;
            //     staticObjects.push_back(g);
            //     createStaticTriangleMeshFromMeshWithTransform(g.mesh, g.model);
            // }


            // // RIGHT (mirrored across X)
            // {
            //     glm::mat4 mirror = glm::scale(glm::mat4(1), glm::vec3(-1,1,1));

            //     glm::mat4 model =
            //         glm::translate(glm::mat4(1), rightWallInside) *
            //         basis *
            //         mirror;

            //     Renderable g;
            //     g.mesh = guide;
            //     g.model = model;
            //     g.doubleSided = true;
            //     staticObjects.push_back(g);
            //     createStaticTriangleMeshFromMeshWithTransform(g.mesh, g.model);
            // }
        }






        // ==================================================
        //  STAGGERED OBSTACLES FOR THIS BOARD
        //  (every board except first and last)
        // ==================================================


        // ==================================================
        // SINGLE OBSTACLE TYPE SELECTED PER BOARD
        // ==================================================
        int boardType = currentObstacleType % 4;
        currentObstacleType++;  // prepare next board’s type

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
                // int idx = int( float(row+1) / float(rowsPerBoard+1) * M );


                // int idx = int( float(row+1) / float(rowsPerBoard+1) * M );
                // idx = std::min(idx, M - 2);  // ← safest


                int idx = int(float(row + 1) / float(rowsPerBoard + 1) * M);

                // // BEFORE clamping — debug
                // std::cout << "[IDX] raw=" << idx
                //           << "  M=" << M
                //           << "  row=" << row << "\n";

                idx = std::clamp(idx, 0, M - 1);

                // AFTER clamping
                // std::cout << "      clamped=" << idx << "\n";


                glm::vec3 center = p->smoothedPath[idx];

                //--------------------------------------------------
                // Compute T, R, U at this path index
                //--------------------------------------------------
                glm::vec3 T, R, U;
                getFrameAtIndex(p->smoothedPath, idx, T, R, U);

                //--------------------------------------------------
                // Orientation basis matrix
                //--------------------------------------------------
                glm::mat4 basis(1.0f);
                basis[0] = glm::vec4(R, 0);
                basis[1] = glm::vec4(U, 0);
                basis[2] = glm::vec4(T, 0);

                float halfW = p->trackRadius;
                float deflectorHeight = 1.2f;     // you tune this
                float deflectorInward = 0.6f;     // you tune this
                float boardYoffset   = 0.05f;     // board thickness

                glm::vec3 leftBase  = center - R * halfW + U * boardYoffset;
                glm::vec3 rightBase = center + R * halfW + U * boardYoffset;

                // Mesh defMesh = createDeflectorRail(deflectorHeight, deflectorInward);

                // {

                //     glm::mat4 model = glm::translate(glm::mat4(1.f), leftBase) * basis;
                //     Renderable d;
                //     d.mesh  = defMesh;
                //     d.model = model;
                //     d.doubleSided = true;

                //     staticObjects.push_back(d);
                //     createStaticTriangleMeshFromMeshWithTransform(d.mesh, d.model);

                // }


                // // RIGHT SIDE → MIRROR: inward direction is NEGATIVE R
                // {
                //     // We flip X-scale to mirror the piece
                //     glm::mat4 mirror = glm::scale(glm::mat4(1.f), glm::vec3(-1,1,1));

                //     glm::mat4 model =
                //         glm::translate(glm::mat4(1.f), rightBase) *
                //         basis *
                //         mirror;

                //     Renderable d;
                //     d.mesh  = defMesh;
                //     d.model = model;
                //     d.doubleSided = true;

                //     staticObjects.push_back(d);
                //     createStaticTriangleMeshFromMeshWithTransform(d.mesh, d.model);
                // }

                //--------------------------------------------------
                // Spawn obstacles between lanes
                //--------------------------------------------------
                int obstacles = nLanes - 1;


                for (int k = 0; k < obstacles; ++k){
                    float xPos = 0.5f * (laneX[k] + laneX[k+1]);

                    // stagger: left/right
                    float stagger = (row % 2 == 0) ? -0.25f * laneSpacing
                                                   : +0.25f * laneSpacing;
                    xPos += stagger;

                    //--------------------------------------------------
                    // Select obstacle mesh
                    //--------------------------------------------------
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

                    //--------------------------------------------------
                    // Final position
                    //--------------------------------------------------
                    glm::vec3 obsPos =
                        center +
                        R * xPos +
                        T * upShift +
                        U * lift;

                    glm::mat4 model =
                        glm::translate(glm::mat4(1.0f), obsPos) *
                        basis;


                    //--------------------------------------------------
                    // Add obstacle
                    //--------------------------------------------------
                    Renderable obstacle;
                    obstacle.mesh = mesh;
                    obstacle.model = model;
                    obstacle.doubleSided = true;
                    obstacle.textureID = trackTextures[0];

                    obstacle.useTexture = true;
                    staticObjects.push_back(obstacle);
                    createStaticTriangleMeshFromMeshWithTransform(obstacle.mesh, obstacle.model);
                }
            }
        }


        //--------------------------------------------------------
        // 4. END-WALL FOR THIS BOARD
        //--------------------------------------------------------
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
        wall.textureID = trackTextures[0];

        staticObjects.push_back(wall);
        createStaticTriangleMeshFromMeshWithTransform(wall.mesh, wall.model);

        //--------------------------------------------------------
        // 5. ADVANCE pos TO START NEXT PIECE
        //--------------------------------------------------------
        pos = p->smoothedPath.back();
        pos.y -= 15.f;

        pos.z = (i % 2 == 0) ? 15.f : 0.f;

    }


    // =====================================================
    //  CREATE SPHERES (after track is fully built)
    // =====================================================

    // std::cout << "[SPHERES] Creating spheres...\n";

    const int nSpheres = nLanes;     // 1 sphere per lane
    const float radius = sphereRadius;

    Mesh sphereMesh = createSphere(radius, 36, 18);

    TrackPiece* startBoard = trackPieces[0];
    int M = startBoard->smoothedPath.size();

    // safe index somewhere near the start
    int idx0 = std::min(5, M - 1);
    glm::vec3 center = startBoard->smoothedPath[idx0];

    // compute frame at this index
    glm::vec3 T0, R0, U0;
    getFrameAtIndex(startBoard->smoothedPath, idx0, T0, R0, U0);

    // lane center positions
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
            U0 * (radius + 0.1f);   // lift above board

        glm::mat4 model = glm::translate(glm::mat4(1.f), pos);

        Renderable sphereObj;
        sphereObj.mesh = sphereMesh;
        sphereObj.model = model;
        sphereObj.doubleSided = true;
        sphereObj.textureID = trackTextures[0];
        sphereObj.useTexture = true;

        dynamicObjects.push_back(sphereObj);
        rigidBodyToRenderable.push_back(dynamicObjects.size() - 1);

        btRigidBody* body = createSphereRigidBody(radius, model, 1.0f);
        physicsBodies.push_back(body);

        if (k == playerSphereIndex) {
            std::cout << "[PLAYER] sphere is lane " << k << "\n";
        }
    }

    std::cout << "[SPHERES] Done. Count = " << physicsBodies.size() << "\n";

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
    // lights.push_back(light2);
    // lights.push_back(light3);


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

    // === FOLLOW CAMERA ===

    if (!physicsBodies.empty()) {

        // std::cout << "Not empty\n";
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
        // std::cout << "[WARNING] No spheres exist — skipping follow-camera init.\n";
        followSphere = false;
    }


    // debugDrawer = new BulletDebugDrawer();
    // dynamicsWorld->setDebugDrawer(debugDrawer);
    // debugDrawer->setDebugMode(
    //     btIDebugDraw::DBG_DrawWireframe |
    //     btIDebugDraw::DBG_DrawContactPoints |
    //     btIDebugDraw::DBG_DrawNormals
    //     );




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

    // -----------------------------------------------------------
    // Toggle physics (P)
    // -----------------------------------------------------------
    if (pPressed && !pWasPressed) {
        physicsEnabled = !physicsEnabled;
    }
    pWasPressed = pPressed;

    // -----------------------------------------------------------
    // Toggle followSphere mode (F)
    // -----------------------------------------------------------
    if (fPressed && !fWasPressed) {
        followSphere = !followSphere;

        // Lock cursor ALWAYS when F is toggled
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCaptured = true;

        // Avoid camera jump
        firstMouse = true;

        // If leaving follow mode, sync yaw/pitch from current camera direction
        if (!followSphere) {
            camera.Yaw   = glm::degrees(atan2(camera.Front.z, camera.Front.x));
            camera.Pitch = glm::degrees(asin(camera.Front.y));
        }
    }
    fWasPressed = fPressed;

    // -----------------------------------------------------------
    // Non-follow (free movement)
    // -----------------------------------------------------------
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

    // -----------------------------------------------------------
    // ESC → release mouse
    // -----------------------------------------------------------
    if (inputManager.isKeyPressed(GLFW_KEY_ESCAPE) && mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseCaptured = false;
    }

    // -----------------------------------------------------------
    // Scroll (zoom)
    // -----------------------------------------------------------
    if (inputManager.getScrollY() != 0.0) {
        camera.ProcessMouseScroll(inputManager.getScrollY());
        inputManager.resetScroll();
    }
}

void Renderer::renderFrame() {
    glClearColor(0.2f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if(followSphere && !physicsBodies.empty()){
        // === FOLLOW CAMERA ===
        int sphereIdx = 0;

        // Get sphere transform
        btTransform trans;
        physicsBodies[sphereIdx]->getMotionState()->getWorldTransform(trans);
        glm::mat4 sphereModel = btTransformToGlm(trans);

        glm::vec3 spherePos = glm::vec3(sphereModel[3]);  // sphere world position

        // Camera offset relative to sphere
        glm::vec3 camOffset(-6.0f, 3.0f, -10.0f); // tune this

        // Set camera position
        camera.Position = spherePos + camOffset;

        // Compute new camera forward direction
        camera.Front = glm::normalize(spherePos - camera.Position);

        // Rebuild Right and Up manually
        camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
        camera.Up    = glm::normalize(glm::cross(camera.Right, camera.Front));

    }


    if (dynamicsWorld) {

        if(physicsEnabled){


            // WAKE UP ALL BODIES (important!)
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





        // debugDrawer->clear();
        // dynamicsWorld->debugDrawWorld();

        // sphereShader.use();
        glDisable(GL_CULL_FACE);
    }

    // if (!raceFinished && !physicsBodies.empty() && trackPieces.size() > 0)
    // {
    //     TrackPiece* lastBoard = trackPieces.back();

    //     glm::vec3 finishPoint = lastBoard->endPoint;
    //     glm::vec3 Tfinish     = glm::normalize(lastBoard->frameT[0]);

    //     for (int i = 0; i < physicsBodies.size(); ++i)
    //     {
    //         btTransform trans;
    //         physicsBodies[i]->getMotionState()->getWorldTransform(trans);

    //         glm::mat4 model = btTransformToGlm(trans);
    //         glm::vec3 spherePos = glm::vec3(model[3]);

    //         float dist = glm::dot(spherePos - finishPoint, Tfinish);

    //         if (dist > 0.0f)
    //         {
    //             raceFinished = true;
    //             winnerIndex = i;

    //             if(i == playerSphereIndex){
    //                 playerWon = true;
    //                 std::cout << "\n Congratulations! Your sphere won the race!\n";
    //             }
    //             else{
    //                 playerWon = false;
    //                 std::cout << "\n Too bad! Sphere " << i << " won.\n";
    //             }

    //             break;
    //         }
    //     }
    // }

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


// convert btTransform -> glm::mat4
glm::mat4 Renderer::btTransformToGlm(const btTransform& t) const {
    btScalar m[16];
    t.getOpenGLMatrix(m); // fills column-major OpenGL matrix
    // glm expects column-major as well; we can copy directly
    glm::mat4 mat;
    for (int i=0;i<16;++i) mat[i/4][i%4] = (float)m[i];
    return mat;
}

// convert glm::mat4 -> btTransform
btTransform Renderer::glmToBtTransform(const glm::mat4& m) const {
    // extract basis and origin
    glm::vec3 origin = glm::vec3(m[3][0], m[3][1], m[3][2]);
    // basis columns:
    btMatrix3x3 basis(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]
        );
    btTransform t(basis, btVector3(origin.x, origin.y, origin.z));
    return t;
}

// Create a static btRigidBody from a triangle mesh (your track)
btRigidBody* Renderer::createStaticTriangleMeshFromMesh(const Mesh& mesh) {
    if (mesh.indices.empty() || mesh.vertices.empty()) return nullptr;

    btTriangleMesh* triMesh = new btTriangleMesh();

    const size_t strideFloats = 11; // pos(3), normal(3), uv(2), color(3)
    const size_t vertexCount = mesh.vertices.size() / strideFloats;

    // extract positions
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
    // Keep triMesh alive — btBvhTriangleMeshShape stores pointer to it, so we should keep it too:
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

// Create a dynamic sphere rigid body and return pointer
btRigidBody* Renderer::createSphereRigidBody(float radius, const glm::mat4& startTransform, float mass) {
    btCollisionShape* sphereShape = new btSphereShape(radius);
    collisionShapes.push_back(sphereShape);

    btTransform start = glmToBtTransform(startTransform);

    btVector3 localInertia(0,0,0);
    if (mass > 0.0f) sphereShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(start);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, sphereShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    // configure physical behaviour a bit
    body->setFriction(0.5f);
    body->setRollingFriction(0.02f);
    body->setRestitution(0.1f);

    dynamicsWorld->addRigidBody(body);
    return body;
}


// === GLFW Callbacks ===
void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}



float Renderer::extractRoll(const glm::vec3& N, const glm::vec3& U, const glm::vec3& worldUp)
{
    float cosAngle = glm::dot(U, worldUp);
    float angle = acos(glm::clamp(cosAngle, -1.0f, 1.0f));

    // sign determined by N's world-up projection
    float sign = (glm::dot(N, worldUp) < 0.0f) ? -1.0f : +1.0f;

    return angle * sign;
}


float Renderer::computeRollBetween(
    const glm::vec3 &R0,
    const glm::vec3 &U0,
    const glm::vec3 &T0,
    const glm::vec3 &R1)
{
    // Project R0 and R1 to plane orthogonal to T0
    glm::vec3 R0p = glm::normalize(R0 - glm::dot(R0, T0) * T0);
    glm::vec3 R1p = glm::normalize(R1 - glm::dot(R1, T0) * T0);

    float c = glm::dot(R0p, R1p);
    c = glm::clamp(c, -1.0f, 1.0f);

    float s = glm::dot(glm::cross(R0p, R1p), T0);

    return atan2(s, c);   // signed roll
}


float Renderer::computeRoll(const glm::vec3& right,
                  const glm::vec3& up,
                  const glm::vec3& tangent)
{
    // Project "right" onto a horizontal plane perpendicular to tangent
    glm::vec3 worldUp(0,1,0);

    glm::vec3 refRight = glm::normalize(glm::cross(worldUp, tangent));
    if (glm::length(refRight) < 0.001f)
        refRight = glm::vec3(1,0,0); // fallback

    float dotv = glm::clamp(glm::dot(refRight, right), -1.0f, 1.0f);
    float angle = acos(dotv);

    float sign = glm::dot(glm::cross(refRight, right), tangent) < 0 ? -1.0f : 1.0f;
    return angle * sign;
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

        glm::vec3 n = glm::normalize(glm::cross(pb - pa, pc - pa));
        // std::cout << "Triangle normal = " << n.x << ", " << n.y << ", " << n.z << "\n";

        triMesh->addTriangle(
            btVector3(pa.x, pa.y, pa.z),
            btVector3(pb.x, pb.y, pb.z),
            btVector3(pc.x, pc.y, pc.z)
            );


        if (glm::length(pa - pb) < 0.0001f ||
            glm::length(pa - pc) < 0.0001f ||
            glm::length(pb - pc) < 0.0001f)
        {
            std::cout << "Degenerate triangle detected!\n";
        }
    }

    auto* shape = new btBvhTriangleMeshShape(triMesh, true);
    collisionShapes.push_back(shape);

    btTransform t;
    t.setIdentity();

    btDefaultMotionState* motion = new btDefaultMotionState(t);

    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion, shape);
    btRigidBody* body = new btRigidBody(info);

    // ⭐ THE IMPORTANT PART ⭐
    body->setCollisionFlags(
        body->getCollisionFlags() |
        btCollisionObject::CF_STATIC_OBJECT
        );

    body->setFriction(1.0f);
    body->setRollingFriction(0.0f);
    body->setRestitution(0.1f);

    // static objects should never deactivate
    body->setActivationState(DISABLE_DEACTIVATION);

    dynamicsWorld->addRigidBody(body);
    return body;
}

glm::vec3 Renderer::extractPosition(const glm::mat4& m) {
    return glm::vec3(m[3]); // x,y,z translation
}



glm::mat4 Renderer::buildBasis(const glm::vec3& N,
                     const glm::vec3& B,
                     const glm::vec3& T)
{
    glm::mat4 rot(1.0f);
    rot[0] = glm::vec4(N, 0.0f);
    rot[1] = glm::vec4(B, 0.0f);
    rot[2] = glm::vec4(T, 0.0f);
    return rot;
}



void Renderer::addObstacle(
    const Mesh& mesh,
    const glm::vec3& position,
    const glm::mat4& basis,
    const glm::vec3& scale)
{
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), position)
        * basis
        * glm::scale(glm::mat4(1.0f), scale);

    Renderable r;
    r.mesh = mesh;
    r.model = model;
    r.doubleSided = true;

    staticObjects.push_back(r);
    createStaticTriangleMeshFromMeshWithTransform(r.mesh, r.model);
}


// void Renderer::getFrameAtIndex(
//     const std::vector<glm::vec3> &path,
//     int i,
//     glm::vec3 &T,
//     glm::vec3 &R,
//     glm::vec3 &U)
// {
//     int N = path.size();

//     if (i == 0)
//         T = glm::normalize(path[1] - path[0]);
//     else if (i == N - 1)
//         T = glm::normalize(path[N - 1] - path[N - 2]);
//     else
//         T = glm::normalize(path[i + 1] - path[i - 1]);

//     glm::vec3 worldUp(0,1,0);
//     if (fabs(glm::dot(worldUp, T)) > 0.9f)
//         worldUp = glm::vec3(1,0,0);

//     R = glm::normalize(glm::cross(worldUp, T));
//     U = glm::normalize(glm::cross(T, R));
// }


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

    if (i < 0 || i >= N) {
        std::cout << "[FRAME ERROR] index " << i
                  << " is out of range (0.." << (N-1) << ")\n";
        i = std::clamp(i, 0, N - 1);
    }

    std::cout << "[FRAME] path size = " << N << "  index = " << i << "\n";

    // ==== SAFE TANGENT COMPUTATION ====
    if (i == 0) {
        std::cout << "  Using forward diff: path[1] - path[0]\n";
        T = glm::normalize(path[1] - path[0]);
    }
    else if (i == N - 1) {
        std::cout << "  Using backward diff: path[N-1] - path[N-2]\n";
        T = glm::normalize(path[N - 1] - path[N - 2]);
    }
    else {
        std::cout << "  Using central diff: path[i+1] - path[i-1]\n";
        T = glm::normalize(path[i + 1] - path[i - 1]);
    }

    glm::vec3 worldUp(0,1,0);
    if (fabs(glm::dot(worldUp, T)) > 0.9f) {
        std::cout << "  WARNING: worldUp parallel to T — using fallback axis\n";
        worldUp = glm::vec3(1,0,0);
    }

    R = glm::normalize(glm::cross(worldUp, T));
    U = glm::normalize(glm::cross(T, R));
}

glm::vec3 Renderer::getPointAtT(const std::vector<glm::vec3>& pts, float t)
{
    if (pts.size() < 2) return pts.front();

    float total = pts.size() - 1;

    float f = t * total;
    int i = floor(f);
    float frac = f - i;

    if (i >= pts.size() - 1)
        return pts.back();

    return glm::mix(pts[i], pts[i+1], frac);
}



void Renderer::getFrameAtT(const std::vector<glm::vec3>& pts, float t,
                 glm::vec3& T, glm::vec3& R, glm::vec3& U)
{
    glm::vec3 p = getPointAtT(pts, t);

    float eps = 1.0f / float(pts.size());
    glm::vec3 pNext = getPointAtT(pts, std::min(t+eps, 1.0f));

    T = glm::normalize(pNext - p);
    R = glm::normalize(glm::cross(T, glm::vec3(0,1,0))); // world up fallback
    U = glm::normalize(glm::cross(R, T));
}
