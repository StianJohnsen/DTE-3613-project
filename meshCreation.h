#ifndef MESHCREATION_H
#define MESHCREATION_H

#define GLM_ENABLE_EXPERIMENTAL

#include "meshPipelineSetup.h"
#include <glm/glm.hpp>


struct MeshInstance {
    Mesh mesh;
    glm::mat4 model;
};

Mesh createSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
Mesh createSkybox();

Mesh createWall(float width, float height, float thickness);
Mesh createEndWall(float width, float height, float thickness);
Mesh createCylinder(float radius, float height, int segments);
Mesh createPyramid(float baseSize, float height);
Mesh createTriangularPrism(float width, float height, float depth);

Mesh createGuideDeflectorLocal(float length, float widthInside, float widthAtWall, float height);

MeshInstance createFlatBoardLocal(
    const std::vector<glm::vec3>& pathWorld,
    float width);

#endif // MESHCREATION_H
