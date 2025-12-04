cmake_minimum_required(VERSION 3.14)

project(dte3613_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Bullet CONFIG REQUIRED)


add_executable(dte3613_project
  main.cpp
  processShader.h
  processShader.cpp
  shaders/sphere.vert
  shaders/sphere.frag
  meshCreation.h
  meshPipelineSetup.h
  meshCreation.cpp
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
  trackPiece.h
  flatBoardPiece.h

)
target_link_libraries(dte3613_project
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


