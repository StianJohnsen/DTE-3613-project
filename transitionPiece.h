// #ifndef TRANSITIONPIECE_H
// #define TRANSITIONPIECE_H
// #include "trackPiece.h"
// #include "meshCreation.h"
// #include <glm/gtx/rotate_vector.hpp>


// class TransitionPiece : public TrackPiece {
// public:
//     float length;       // how far along the forward dir the transition goes
//     float lateral;      // how far to move sideways (to meet loop center)
//     float vertical;     // optional vertical lift/drop
//     int   turnDir;      // +1 = right turn, -1 = left turn
//     float entryPitch;   // how much to pitch the end tangent upward (radians)

//     unsigned int pathResolution;
//     unsigned int arcSegments;
//     float curveToPipeWeight;

//     TransitionPiece(float lengthMeters = 6.0f,
//                     float lateralMeters = 6.0f,
//                     int turnDirection = +1,
//                     float verticalMeters = 0.0f,
//                     float entryPitchRadians = glm::radians(20.0f),
//                     unsigned int pathRes = 40,
//                     unsigned int arcSegs = 24,
//                     float morphWeight = 1.0f)
//         : length(lengthMeters),
//         lateral(lateralMeters),
//         vertical(verticalMeters),
//         turnDir(turnDirection),
//         entryPitch(entryPitchRadians),
//         pathResolution(pathRes),
//         arcSegments(arcSegs),
//         curveToPipeWeight(morphWeight)
//     {}

//     void generateControlPoints(const glm::vec3 &attachPoint,
//                                const glm::vec3 &attachDir) override
//     {
//         controlPoints.clear();

//         // stable local frame from incoming direction
//         glm::vec3 forward = glm::normalize(attachDir);
//         glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
//         glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
//         if (glm::length2(right) < 1e-6f) { // degenerate case
//             right = glm::vec3(1,0,0);
//         }
//         glm::vec3 up = glm::normalize(glm::cross(right, forward));

//         // Build target end point (p3):
//         // move forward by roughly 'length', move sideways by 'turnDir * lateral',
//         // and optionally lift/drop by 'vertical'
//         glm::vec3 p0 = attachPoint;

//         glm::vec3 p3 = p0
//                        + forward * length           // forward advance
//                        + right   * (turnDir * lateral) // sideways toward loop center
//                        + up      * vertical;        // small vertical offset if desired

//         // Build end tangent: rotate forward a bit upward (entryPitch) and slightly toward loop center.
//         glm::vec3 rotated = glm::normalize(glm::rotate(forward, entryPitch, right));
//         // Also bias end tangent toward direction from p3 back to p0 to make it wrap
//         glm::vec3 towards = glm::normalize(glm::mix(rotated, glm::normalize(p3 - p0), 0.35f));
//         glm::vec3 t1 = towards;

//         // p1/p2 as control handles (Hermite style).
//         // p1 follows incoming tangent; p2 approaches p3 along t1.
//         float handle0 = glm::clamp(length * 0.35f, 0.01f, length * 0.6f);
//         float handle1 = glm::clamp(length * 0.35f, 0.01f, length * 0.6f);

//         glm::vec3 p1 = p0 + forward * handle0;
//         glm::vec3 p2 = p3 - t1 * handle1;

//         controlPoints.push_back(p0);
//         controlPoints.push_back(p1);
//         controlPoints.push_back(p2);
//         controlPoints.push_back(p3);

//         // store endPoint and endTangent for others to attach
//         endPoint   = p3;
//         endTangent = t1;
//     }

// protected:
//     void generateMesh() override {
//         // Use morphing halfpipe so entrance can gradually become tubular
//         mesh = createMorphingHalfpipe(controlPoints,
//                                       trackRadius,    // width (same as other pieces)
//                                       pathResolution,
//                                       arcSegments,
//                                       curveToPipeWeight);
//     }
// };

// #endif // TRANSITIONPIECE_H
