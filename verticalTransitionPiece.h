// #ifndef VERTICALTRANSITIONPIECE_H
// #define VERTICALTRANSITIONPIECE_H

// #include "trackPiece.h"
// #include "meshCreation.h"

// #include <glm/gtx/rotate_vector.hpp>

// class VerticalTransitionPiece : public TrackPiece {
// public:
//     float length;        // horizontal length of transition
//     float slopeOut;      // final slope in radians
//     int   segments;      // number of samples

//     VerticalTransitionPiece(float L, float slopeOutRad, int segs = 32)
//         : length(L), slopeOut(slopeOutRad), segments(segs)
//     {}

// protected:

//     // No Catmull-Rom; we produce dense path ourselves
//     void computeSpline() override {
//         smoothedPath = controlPoints;
//     }

//     void doGenerateControlPoints(const glm::vec3 &attachPoint,
//                                  const glm::vec3 &attachTangent,
//                                  const glm::vec3 &attachRight,
//                                  const glm::vec3 &attachUp) override
//     {
//         controlPoints.clear();

//         //
//         // 1) Extract slopeIn from attachTangent
//         //
//         glm::vec3 T  = glm::normalize(attachTangent);

//         // vertical component
//         float slopeIn = atan2(T.y, glm::length(glm::vec2(T.x, T.z)));
//         float slopeOutLocal = slopeOut;

//         //
//         // 2) Build a horizontal forward direction (flattened T)
//         //
//         glm::vec3 Fflat = glm::normalize(glm::vec3(T.x, 0.0f, T.z));
//         if (glm::length(Fflat) < 0.001f)
//             Fflat = glm::vec3(0,0,1); // fallback if straight up/down

//         glm::vec3 up = glm::vec3(0,1,0);

//         //
//         // 3) Hermite cubic for vertical displacement y(s)
//         //
//         auto hermite = [&](float t) {
//             float t2 = t*t;
//             float t3 = t2*t;

//             float m0 = tan(slopeIn);
//             float m1 = tan(slopeOutLocal);

//             // Hermite basis
//             float h00 =  2*t3 - 3*t2 + 1;
//             float h10 =      t3 - 2*t2 + t;
//             float h01 = -2*t3 + 3*t2;
//             float h11 =      t3 -    t2;

//             float y = h00*0.0f + h10*m0*length + h01*0.0f + h11*m1*length;
//             return y;
//         };

//         //
//         // 4) Generate world points
//         //
//         for (int i = 0; i <= segments; i++)
//         {
//             float t = float(i) / float(segments);

//             float x = length * t;
//             float y = hermite(t);

//             glm::vec3 world =
//                 attachPoint +
//                 Fflat * x +
//                 up    * y;

//             controlPoints.push_back(world);
//         }
//     }

//     void generateMesh() override {
//         mesh = createFlatBoard(
//             smoothedPath,
//             frameT, frameN, frameB,
//             trackRadius * 2.0f);
//     }
// };

// #endif // VERTICALTRANSITIONPIECE_H






#ifndef VERTICALTRANSITIONPIECE_H
#define VERTICALTRANSITIONPIECE_H

#include "trackPiece.h"
#include "meshCreation.h"

#include <glm/gtx/rotate_vector.hpp>


class VerticalTransitionPiece : public TrackPiece {
public:
    float length;
    float finalSlope;   // radians
    int   segments;

    VerticalTransitionPiece(float L, float slope, int segs=32)
        : length(L), finalSlope(slope), segments(segs)
    {}

protected:

    void computeSpline() override {
        smoothedPath = controlPoints;
    }

    void doGenerateControlPoints(const glm::vec3 &attachPoint,
                                 const glm::vec3 &attachTangent,
                                 const glm::vec3 &attachRight,
                                 const glm::vec3 &attachUp) override
    {
        controlPoints.clear();

        glm::vec3 F0 = glm::normalize(attachTangent);
        glm::vec3 R  = glm::normalize(attachRight);
        glm::vec3 U0 = glm::normalize(attachUp);

        float L      = length;
        float ds     = L / float(segments);
        float slope0 = 0.0f;              // start with current slope
        float slope1 = finalSlope;        // target slope (radians)

        // i = 0: exact attach point (stitching!)
        glm::vec3 pos = attachPoint;
        controlPoints.push_back(pos);

        for (int i = 1; i <= segments; ++i)
        {
            float t    = float(i) / float(segments);
            float ease = t * t * (3.0f - 2.0f * t); // smoothstep

            float slopeNow = glm::mix(slope0, slope1, ease);

            // rotate F0 by current slope around R (same style as CTP)
            glm::vec3 F = glm::normalize(glm::rotate(F0, slopeNow, R));
            glm::vec3 U = glm::normalize(glm::cross(F, R));

            pos += F * ds;        // advance along the tilted forward
            controlPoints.push_back(pos);
        }
    }







    // void doGenerateControlPoints(
    //     const glm::vec3& attachPoint,
    //     const glm::vec3& attachTangent,
    //     const glm::vec3& attachRight,
    //     const glm::vec3& attachUp) override
    // {
    //     controlPoints.clear();

    //     glm::vec3 F0 = glm::normalize(attachTangent);
    //     glm::vec3 R  = glm::normalize(attachRight);
    //     glm::vec3 U0 = glm::normalize(attachUp);

    //     // We rotate the forward vector from 0 → finalSlope gradually
    //     float L = length;
    //     float ds = L / float(segments);

    //     float slope0 = 0.0f;
    //     float slope1 = finalSlope;

    //     glm::vec3 pos = attachPoint;

    //     for (int i = 0; i <= segments; i++)
    //     {
    //         float t = float(i) / float(segments);
    //         float ease = t*t*(3-2*t);

    //         float slopeNow = glm::mix(slope0, slope1, ease);

    //         // rotate F0 by slopeNow around R
    //         glm::vec3 F = glm::normalize(glm::rotate(F0, slopeNow, R));

    //         // recompute U to stay orthonormal
    //         glm::vec3 U = glm::normalize(glm::cross(F, R));

    //         // advance along local axes
    //         pos += F * ds;

    //         controlPoints.push_back(pos);
    //     }
    // }

    void generateMesh() override {
        mesh = createFlatBoard(smoothedPath, frameT, frameN, frameB,
                               trackRadius * 2.0f);
    }
};
#endif // VERTICALTRANSITIONPIECE_H
