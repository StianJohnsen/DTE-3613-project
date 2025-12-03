
// #ifndef CLOTHOIDTRANSITIONPIECE_H
// #define CLOTHOIDTRANSITIONPIECE_H

// #include "trackPiece.h"
// #include "meshCreation.h"
// #include <iostream>

// class ClothoidTransitionPiece : public TrackPiece {
// public:
//     float length;       // total arc length of transition
//     float targetRadius; // desired loop radius
//     int   segments;     // how many control points





//     ClothoidTransitionPiece(float L, float R, int segs = 32)
//         : length(L), targetRadius(R), segments(segs)
//     {}




// protected:
//     // Dense path already, no Catmull–Rom needed

//     void computeSpline() override {
//         smoothedPath = controlPoints;
//     }

//     void doGenerateControlPoints(const glm::vec3 &attachPoint,
//                                  const glm::vec3 &attachTangent,
//                                  const glm::vec3 &attachRight,
//                                  const glm::vec3 &attachUp) override
//     {
//         controlPoints.clear();

//         glm::vec3 F0 = glm::normalize(attachTangent);
//         glm::vec3 R  = glm::normalize(attachRight);
//         glm::vec3 U0 = glm::normalize(attachUp);

//         // Tilt F by slopeAngle around R
//         glm::vec3 F = glm::normalize(glm::rotate(F0, slopeAngle, R));

//         // Recompute U to keep the frame orthonormal
//         glm::vec3 U = glm::normalize(glm::cross(F, R));


//         // glm::vec3 F = glm::normalize(attachTangent);
//         // glm::vec3 R = glm::normalize(attachRight);
//         // glm::vec3 U = glm::normalize(attachUp);

//         // glm::vec3 F = glm::normalize(attachTangent);

//         // // FORCE local axes to world-up frame
//         // glm::vec3 up(0,1,0);
//         // if (fabs(glm::dot(F, up)) > 0.9f)
//         //     up = glm::vec3(1,0,0);

//         // glm::vec3 R = glm::normalize(glm::cross(up, F));
//         // glm::vec3 U = glm::normalize(glm::cross(F, R));


//         float L     = length;
//         float Rloop = targetRadius;
//         float a2    = L * Rloop;
//         float shiftAmount = Rloop * 0.25f;


//         int sign_to_swing;
//         if(isEntrance){
//             sign_to_swing = 1;
//         }
//         else {
//             sign_to_swing = -1;
//         }

//         glm::vec2 pos(0.0f);
//         float s  = 0.0f;
//         float ds = L / float(segments);

//         for (int i = 0; i <= segments; ++i) {
//             float t = float(i) / float(segments);

//             // ------------------------------------
//             // 1. Clothoid curve in local 2D
//             // ------------------------------------
//             float theta = (s * s) / (2.0f * a2);
//             glm::vec2 dir(std::cos(theta), std::sin(theta));

//             if (i > 0)
//                 pos += dir * ds;

//             // sideways offset (smooth)
//             float ease    = t * t * (3.0f - 2.0f * t);
//             float xOffset = sign_to_swing * ease * shiftAmount;

//             // // initial world point
//             glm::vec3 world =
//                 attachPoint +
//                               F * pos.x +
//                               U * pos.y +
//                               R * xOffset;

//             //     F  * (sign_to_swing*pos.x) +
//             //     U * (sign_to_swing*pos.y) +
//             //     R *  (sign_to_swing*xOffset);


//             // ------------------------------------
//             // 2. APPLY TWIST: rotate around F
//             // ------------------------------------
//             // float roll = glm::mix(twistStart, twistEnd, ease);

//             // glm::mat4 Rtwist = glm::rotate(glm::mat4(1.0f), roll, F);

//             glm::vec3 local = world - attachPoint;
//             // local = glm::vec3(Rtwist * glm::vec4(local, 1.0f));
//             world = attachPoint + local;

//             controlPoints.push_back(world);

//             s += ds;
//         }
//     }



//     void generateMesh() override {
//         mesh = createFlatBoard(smoothedPath, frameT, frameN, frameB,
//                                trackRadius * 2.0f);
//     }


// };

// #endif // CLOTHOIDTRANSITIONPIECE_H
