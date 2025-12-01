// #ifndef SPIRALPIECE_H
// #define SPIRALPIECE_H

// #include "trackPiece.h"
// #include "meshCreation.h"
// #include "processVertices.h"

// class SpiralPiece : public TrackPiece {

// public:
//     float radius;
//     float heightPerTurn;
//     int turns;
//     int resolution;

// SpiralPiece(float r = 8.0f,
//             float height = 5.0f,
//             int t = 2,
//             int res = 40
//             ) : radius(r), heightPerTurn(height),
//         turns(t), resolution(res)
//     {}



//     void generateControlPoints(const glm::vec3 &attachPoint, const glm::vec3 &attachDir)
//         override {


//         controlPoints.clear();

//         controlPoints = generateSpiralControlPoints(
//         120,    // smoother!
//         6.0f,   // start radius
//         12.0f,   // end radius
//         40.0f,  // MUCH more height drop
//         3.0f,   // full spiral loops
//         0.1f,  // gentle wobble
//         2.0f,   // wobble frequency
//         0.6f,   // slightly eased vertical drop
//         42
//         );

//         glm::vec3 offset = attachPoint - controlPoints.front();
//         for(auto& p: controlPoints) p += offset;

//     }

//     // TrackPiece interface
// protected:
//     void generateMesh() override {
//         mesh = createHalfpipe(controlPoints,radius,resolution,arcSegments);
//     };
// };

// #endif // SPIRALPIECE_H
