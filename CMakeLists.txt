cmake_minimum_required(VERSION 3.14)

project(dte3611_project LANGUAGES CXX)




set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Bullet CONFIG REQUIRED)

# set(TS_FILES dte3611_project_en_US.ts)

add_executable(dte3611_project
  main.cpp
  # ${TS_FILES}
  processShader.h
  processShader.cpp
  shaders/sphere.vert
  shaders/sphere.frag
  meshCreation.h
  meshPipelineSetup.h
  meshCreation.cpp
  processVertices.h
  processVertices.cpp

  shader.h
  camera.h
  camera.cpp
  renderer.h
  renderer.cpp
  inputManager.h
  inputManager.cpp
  material.h
  light.h
  renderable.h
  skybox.h
  shaders/skybox.vert
  shaders/skybox.frag
  shaderManager.h
  texture_utils.h
  BezierClass.h
  bezierClass.cpp
  hermiteClass.h
  hermiteClass.cpp
  trackPiece.h
  flatBoardPiece.h
  loopPiece.h
  spiralPiece.h
  loopEntrancePiece.h
  transitionPiece.h
  clothoidTransitionPiece.h
  verticalTransitionPiece.h
  bulletDebugDrawer.h


)
target_link_libraries(dte3611_project
    PRIVATE
    glfw
    GLEW::GLEW
    glm::glm
    soil2
    OpenGL::GL
    LinearMath
    BulletCollision
    BulletDynamics
    BulletSoftBody
)


