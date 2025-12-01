// #ifndef LOOPENTRANCEPIECE_H
// #define LOOPENTRANCEPIECE_H
// #include "trackPiece.h"
// #include "meshCreation.h"


// #include "trackPiece.h"
// #include "meshCreation.h"
// #include "processVertices.h"

// class LoopEntrancePiece : public TrackPiece {
// public:

//     // float radius;    // loop radius
//     // float length;    // how long the entrance transition is

//     // LoopEntrancePiece(float r, float len)
//     //     : radius(r), length(len) {}

//     float length;
//     float pipeClosure;        // 0 = flat, 1 = full pipe
//     unsigned int pathResolution;
//     unsigned int arcSegments;

//     float loopRadius;

//     LoopEntrancePiece(
//         float lengthMeters,
//         float loopRadiusMeters,
//         float pipeClosureAmount,     // 0 → flatboard, 1 → loop tube
//         unsigned int pathRes = 40,
//         unsigned int arcSegs = 24)
//         :
//         length(lengthMeters),
//         pipeClosure(pipeClosureAmount),
//         pathResolution(pathRes),
//         arcSegments(arcSegs),
//         loopRadius(loopRadiusMeters)
//     {}



//     void generateControlPoints(
//         const glm::vec3 &attachPoint,
//         const glm::vec3 &attachDir) override
//     {
//         controlPoints.clear();

//         glm::vec3 forward = glm::normalize(attachDir);
//         glm::vec3 worldUp(0,1,0);
//         glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
//         glm::vec3 up    = glm::normalize(glm::cross(right, forward));

//         // Bottom of the loop relative to the loop's center line
//         glm::vec3 loopBottom = attachPoint + up * (-loopRadius);

//         glm::vec3 endTangent = forward;

//         glm::vec3 p0 = attachPoint;
//         glm::vec3 p3 = loopBottom;

//         float L = glm::distance(p0, p3);

//         glm::vec3 p1 = p0 + forward * (0.4f * L);
//         glm::vec3 p2 = p3 - endTangent * (0.4f * L);

//         controlPoints.push_back(p0);
//         controlPoints.push_back(p1);
//         controlPoints.push_back(p2);
//         controlPoints.push_back(p3);

//         endPoint   = p3;
//         endTangent = endTangent;
//     }


// protected:
//     void generateMesh() override {

//         mesh = createMorphingHalfpipe(
//             controlPoints,
//             trackRadius,     // width of the half-pipe
//             pathResolution,  // path sampling resolution
//             24,              // arcSegments: resolution of the circular cross-section
//             1.0f             // curveToPipeWeight: 0 = flatboard, 1 = full halfpipe
//             );
//     }
// };

// #endif // LOOPENTRANCEPIECE_H
