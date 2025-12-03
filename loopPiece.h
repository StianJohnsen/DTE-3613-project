
// #ifndef LOOPPIECE_H
// #define LOOPPIECE_H

// #include "trackPiece.h"
// #include "meshCreation.h"
// #include <glm/gtx/rotate_vector.hpp>

// class LoopPiece : public TrackPiece {
// public:
//     float radius;     // loop radius (height = 2R)
//     int   segments;   // number of samples along loop

//     LoopPiece(float r = 10.0f, int segs = 96)
//         : radius(r), segments(segs)
//     {}

// protected:

//     // ------------------------------------------------------------
//     //  Clean implementation:
//     //  1. Build a pure teardrop loop in local coordinates
//     //  2. Transform into world-space using (attachRight, attachUp, attachTangent)
//     //  3. Apply sideways & forward offsets *in world-space*, not local
//     // ------------------------------------------------------------
//     void doGenerateControlPoints(
//         const glm::vec3 &attachPoint,
//         const glm::vec3 &attachTangent,
//         const glm::vec3 &attachRight,
//         const glm::vec3 &attachUp) override
//     {
//         controlPoints.clear();

//         float R = radius;
//         int   N = segments;

//         // -----------------------------
//         // 1. Generate pure loop (local)
//         // -----------------------------
//         std::vector<glm::vec3> local;
//         local.reserve(N + 1);

//         local.push_back(glm::vec3(0,0,0));   // attach point

//         for (int i = 1; i <= N; i++) {
//             float t = float(i) / float(N);

//             // smooth t for teardrop shape
//             float smoothT = t * t * (3 - 2*t);

//             float theta = 2.0f * glm::pi<float>() * smoothT;

//             float y = R * (1 - cos(theta));
//             float z = R * sin(theta);

//             local.push_back(glm::vec3(0, y, z));
//         }

//         // normalize height to exactly 0..2R
//         float minY = 1e9f, maxY = -1e9f;
//         for (auto &p : local) { minY = glm::min(minY, p.y); maxY = glm::max(maxY, p.y); }
//         float scaleY = (2.0f * R) / (maxY - minY);

//         for (auto &p : local)
//             p.y = (p.y - minY) * scaleY;

//         // -----------------------------------------
//         // 2. Rotate local so its forward matches +Z
//         // -----------------------------------------
//         glm::vec3 T0 = glm::normalize(local[1] - local[0]);
//         glm::vec3 desired(0,0,1);

//         float dotv = glm::clamp(glm::dot(T0, desired), -1.0f, 1.0f);
//         float angle = acos(dotv);

//         glm::vec3 axis = glm::cross(T0, desired);
//         if (glm::length(axis) < 1e-6f)
//             axis = glm::vec3(0,1,0);

//         for (auto &p : local)
//             p = glm::rotate(p, angle, glm::normalize(axis));

//         // -----------------------------------------
//         // 3. Map into world-space immediately
//         // -----------------------------------------
//         glm::vec3 F = glm::normalize(attachTangent);
//         glm::vec3 Rv = glm::normalize(attachRight);
//         glm::vec3 Uv = glm::normalize(attachUp);

//         std::vector<glm::vec3> world;
//         world.reserve(N + 1);

//         for (auto &p : local) {
//             glm::vec3 w = attachPoint +
//                           Rv * p.x +
//                           Uv * p.y +
//                           F  * p.z;
//             world.push_back(w);
//         }

//         // ----------------------------------------------------
//         // 4. Apply sideways / forward offsets *in world-space*
//         // ----------------------------------------------------
//         float leftShift  = -R * 0.30f;
//         float rightShift =  R * 0.80f;
//         float forwardBulge = R * 0.65f;

//         for (int i = 0; i < (int)world.size(); i++)
//         {
//             float t = float(i) / float(world.size() - 1);

//             // sideways S-curve in world-space
//             float halfT = (t < 0.5f) ? (t*2.0f) : ((t-0.5f)*2.0f);
//             float S = halfT*halfT*(3 - 2*halfT);

//             float xShift =
//                 (t < 0.5f)
//                     ? glm::mix(0.0f, leftShift, S)
//                     : glm::mix(leftShift, rightShift, S);

//             // forward bulge (accelerates exit region)
//             float u = glm::clamp(t / 0.40f, 0.0f, 1.0f);
//             float s = u*u*(3 - 2*u);

//             glm::vec3 offset =
//                 Rv * xShift +
//                 F  * (s * forwardBulge);

//             world[i] += offset;
//         }

//         // ------------------------------
//         // 5. Save final world coordinates
//         // ------------------------------



//         // --------------------------------------------
//         // 5. Apply world-space banking (Option A: symmetric)
//         // --------------------------------------------

//         // float maxBankDeg = 35.0f;   // tweak freely

//         // for (int i = 0; i < (int)world.size(); i++)
//         // {
//         //     float t = float(i) / float(world.size() - 1);

//         //     float bankAngle = computeBankAngle(t, maxBankDeg);

//         //     // Rotate the frame around the forward (tangent) direction
//         //     glm::vec3 w = world[i] - attachPoint;

//         //     glm::mat4 Rbank = glm::rotate(glm::mat4(1.0f), bankAngle, F);

//         //     w = glm::vec3(Rbank * glm::vec4(w, 1.0f));

//         //     world[i] = attachPoint + w;
//         // }








//         controlPoints = world;
//     }

//     // No spline: points already dense & smooth
//     void computeSpline() override {
//         smoothedPath = controlPoints;
//     }

//     // Build flat-board mesh along PT frames
//     void generateMesh() override {
//         mesh = createFlatBoard(smoothedPath,
//                                frameT, frameN, frameB,
//                                trackRadius * 2.0f);
//     }




//     float computeBankAngle(float t, float maxBankDeg)
//     {
//         float maxBank = glm::radians(maxBankDeg);

//         float phase = cos(t * glm::pi<float>() * 2.0f);
//         float shaped = phase * (1.0f - abs(phase) * 0.3f);

//         return shaped * maxBank;
//     }

// };

// #endif // LOOPPIECE_H
