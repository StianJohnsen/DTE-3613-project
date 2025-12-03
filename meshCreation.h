#ifndef MESHCREATION_H
#define MESHCREATION_H

#define GLM_ENABLE_EXPERIMENTAL

#include "meshPipelineSetup.h"
#include "hermiteClass.h"
#include "trackPiece.h"
#include <glm/glm.hpp>

#include "bulletDebugDrawer.h"


struct MeshInstance {
    Mesh mesh;
    glm::mat4 model;
};



Mesh createSphere(float radius, unsigned int sectorCount, unsigned int stackCount);

Mesh createSkybox();

Mesh createTrack(const std::vector<glm::vec3>& controllPoints, float width);

Mesh createHalfpipe(const std::vector<glm::vec3>& controlPoints,float radius, unsigned int pathResolution, unsigned int arcSegments);

// Mesh createHalfpipe(const HermiteTrack& hermiteTrack,float radius, unsigned int pathResolution, unsigned int arcSegments);


Mesh mergeMeshes(const std::vector<Mesh>& meshes);

// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& controlPoints,
//     float width,
//     unsigned int pathResolution);

Mesh createFlatBoard(
    const std::vector<glm::vec3>& path,
    const std::vector<glm::vec3>& T,
    const std::vector<glm::vec3>& N,
    const std::vector<glm::vec3>& B,
    float width);

Mesh createTube(
    const std::vector<glm::vec3>& controlPoints,
    float radius,
    unsigned int radialSegments);


Mesh createWall(float width, float height, float thickness);
Mesh createEndWall(float width, float height, float thickness);
Mesh createCylinder(float radius, float height, int segments);
Mesh createPyramid(float baseSize, float height);
Mesh createTriangularPrism(float width, float height, float depth);


Mesh createDebugBulletMesh(const std::vector<DebugLine>& lines);


MeshInstance createFlatBoardLocal(
    const std::vector<glm::vec3>& pathWorld,
    float width);

Mesh createDebugPathLine(const std::vector<glm::vec3>& path,
                         const glm::vec3& color);

Mesh createDebugFrames(const std::vector<glm::vec3>& path,
                       const std::vector<glm::vec3>& T,
                       const std::vector<glm::vec3>& N,
                       const std::vector<glm::vec3>& B,
                       float axisLength,
                       int   step);


Mesh createMorphingHalfpipe(
    const std::vector<glm::vec3>& controlPoints,
    float width,
    unsigned int pathResolution,
    unsigned int arcSegments,
    float curveToPipeWeight);


Mesh createLoopBoard(const std::vector<glm::vec3>& controlPoints, float width,unsigned int pathResolution);


// Mesh createBlendSegment(
//     const TrackPiece& fromPiece, // piece we’re leaving
//     const TrackPiece& toPiece,   // piece we’re entering
//     unsigned int pathResolution,  // samples along blend
//     float width);

// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& path,
//     float width,
//     unsigned int resolution);

// Mesh createTube(
//     const std::vector<glm::vec3>& path,
//     float radius,
//     unsigned int radialSegments);

#endif // MESHCREATION_H
