#ifndef PROCESSVERTICES_H
#define PROCESSVERTICES_H

#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <cmath>
#include "hermiteClass.h"

void createUVSphere(float radius,
                    unsigned int sectorCount,
                    unsigned int stackCount,
                    std::vector<float>& vertices,         // out: interleaved floats
                    std::vector<unsigned int>& indices);


// std::vector<glm::vec3> generateSpiralControlPoints(
//     int numPoints,
//     float turns,            // how many full turns around
//     float radiusStart,
//     float radiusEnd,
//     float totalDrop,        // positive value (how much Y will decrease)
//     float verticalBumpAmp,  // amplitude of sine bumps (can be 0)
//     float verticalBumpFreq, // number of bumps per full turn (e.g. 4)
//     float jitterAmount,     // small random perturbation (world units)
//     unsigned int seed);
std::vector<glm::vec3> generateSpiralControlPoints(
    int count,
    float radiusStart,
    float radiusEnd,
    float heightDrop,
    float turns,
    float wobbleAmplitude,
    float wobbleFrequency,
    float verticalEase, // smooth descent curve: 0 = linear, 1 = eased
    unsigned seed
    );


void createUVSkybox(std::vector<float>& vertices);

void createUVTrack(
    const std::vector<glm::vec3>& controllPoints,
    const float& width,
    unsigned int resolution,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    );

void createUVHalfpipe(
    const HermiteTrack& track,
    float radius,
    unsigned int pathResolution,
    unsigned int arcSegments,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    );

void createUVHalfpipeFromHermite(
    const HermiteTrack& track,
    float width,
    float radius,
    unsigned int pathResolution,
    unsigned int arcSegments,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    );


void createUVHalfpipeWithRails(
    const std::vector<glm::vec3>& controlPoints,
    float radius,                 // radius of the halfpipe
    unsigned int pathResolution,  // samples along the spline
    unsigned int arcSegments,     // samples along semicircle cross-section
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices);

glm::vec3 catmullRom(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
    );

glm::vec3 getPointOnCatmullRomSpline(
    const std::vector<glm::vec3>& controlPoints,
    float t
    );


glm::vec3 getTangentOnCatmullRomSpline(
    const std::vector<glm::vec3>& controlPoints,
    float t
    );

#endif // PROCESSVERTICES_H
