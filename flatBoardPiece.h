

// #ifndef FLATBOARDPIECE_H
// #define FLATBOARDPIECE_H

// #include "meshCreation.h"
// #include "trackPiece.h"
// #include <iostream>

// class FlatBoardPiece : public TrackPiece {
// public:
//     float length;
//     float slopeAngle;

//     FlatBoardPiece(float lengthMeters, float slopeRadians)
//         : length(lengthMeters), slopeAngle(slopeRadians)
//     {}

// protected:

//     void doGenerateControlPoints(
//         const glm::vec3 &attachPoint,
//         const glm::vec3 &attachTangent,
//         const glm::vec3 &attachRight,
//         const glm::vec3 &attachUp) override
//     {
//         controlPoints.clear();

//         glm::vec3 forward = glm::normalize(attachTangent);
//         glm::vec3 right   = glm::normalize(attachRight);
//         glm::vec3 up      = glm::normalize(attachUp);

//         glm::vec3 tiltedForward =
//             glm::normalize(glm::rotate(forward, slopeAngle, right));

//         float L = length;

//         // Smooth: p0 = attachPoint
//         glm::vec3 p0 = attachPoint;

//         // Move forward slightly to define start tangent consistently
//         glm::vec3 p1 = attachPoint + tiltedForward * (L * 0.33f);

//         // Next control points continue in same direction
//         glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.66f);
//         glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.0f);

//         controlPoints.push_back(p0);
//         controlPoints.push_back(p1);
//         controlPoints.push_back(p2);
//         controlPoints.push_back(p3);
//     }


//     // void doGenerateControlPoints(const glm::vec3 &attachPoint,
//     //                              const glm::vec3 &attachTangent,
//     //                              const glm::vec3 &attachRight,
//     //                              const glm::vec3 &attachUp) override
//     // {
//     //     controlPoints.clear();

//     //     glm::vec3 forward = glm::normalize(attachTangent);
//     //     glm::vec3 right   = glm::normalize(attachRight);
//     //     glm::vec3 up      = glm::normalize(attachUp);

//     //     glm::vec3 tiltedForward =
//     //         glm::normalize(glm::rotate(forward, slopeAngle, right));

//     //     float L = length;

//     //     glm::vec3 p0 = attachPoint - tiltedForward * (L * 0.25f);
//     //     glm::vec3 p1 = attachPoint;
//     //     glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.5f);
//     //     glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.0f);

//     //     controlPoints.push_back(p0);
//     //     controlPoints.push_back(p1);
//     //     controlPoints.push_back(p2);
//     //     controlPoints.push_back(p3);
//     // }

//     void generateMesh() override {
//         mesh = createFlatBoard(smoothedPath, frameT, frameN, frameB,
//                                trackRadius * 2.0f);
//     }

// };

// #endif // FLATBOARDPIECE_H



#ifndef FLATBOARDPIECE_H
#define FLATBOARDPIECE_H

#include "meshCreation.h"
#include "trackPiece.h"
#include <glm/gtx/rotate_vector.hpp>

class FlatBoardPiece : public TrackPiece {
public:
    float length;      // length of this board along its local forward
    float slopeAngle;  // tilt applied around the local right axis

    FlatBoardPiece(float lengthMeters, float slopeRadians)
        : length(lengthMeters), slopeAngle(slopeRadians)
    {}

protected:
    // --------------------------------------------------------
    // Build 4 Catmull–Rom control points for a single flat board
    // attachPoint  : where this piece starts
    // attachTangent: forward direction coming into this piece
    // attachRight  : local right
    // attachUp     : local up
    // --------------------------------------------------------
    // void doGenerateControlPoints(const glm::vec3 &attachPoint,
    //                              const glm::vec3 &attachTangent,
    //                              const glm::vec3 &attachRight,
    //                              const glm::vec3 &attachUp) override
    // {
    //     controlPoints.clear();

    //     glm::vec3 forward = glm::normalize(attachTangent);
    //     glm::vec3 right   = glm::normalize(attachRight);
    //     glm::vec3 up      = glm::normalize(attachUp);

    //     // Apply the board’s slope around its local right axis
    //     glm::vec3 tiltedForward =
    //         glm::normalize(glm::rotate(forward, slopeAngle, right));

    //     float L = length;

    //     // Simple 4-point curve along tiltedForward
    //     glm::vec3 p0 = attachPoint - tiltedForward * (L * 0.25f);
    //     glm::vec3 p1 = attachPoint;
    //     glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.5f);
    //     glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.0f);

    //     controlPoints.push_back(p0);
    //     controlPoints.push_back(p1);
    //     controlPoints.push_back(p2);
    //     controlPoints.push_back(p3);
    // }


    void doGenerateControlPoints(const glm::vec3 &attachPoint,
                                 const glm::vec3 &attachTangent,
                                 const glm::vec3 &attachRight,
                                 const glm::vec3 &attachUp) override
    {
        controlPoints.clear();

        glm::vec3 forward = glm::normalize(attachTangent);
        glm::vec3 right   = glm::normalize(attachRight);
        glm::vec3 up      = glm::normalize(attachUp);

        glm::vec3 tiltedForward =
            glm::normalize(glm::rotate(forward, slopeAngle, right));

        float L = length;

        // NEW: Spline exactly spans the board length
        glm::vec3 p0 = attachPoint;
        glm::vec3 p1 = attachPoint + tiltedForward * (L * 0.33f);
        glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.66f);
        glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.00f);

        controlPoints.push_back(p0);
        controlPoints.push_back(p1);
        controlPoints.push_back(p2);
        controlPoints.push_back(p3);
    }


    // --------------------------------------------------------
    // Generate mesh along smoothedPath.
    // Our working version of createFlatBoard ignores T/R/U and
    // recomputes its own simple frames from the path, so we just
    // pass the ones from TrackPiece to match the signature.
    // --------------------------------------------------------
    void generateMesh() override {
        auto board = createFlatBoardLocal(smoothedPath, trackRadius * 2.0f);
        mesh = board.mesh;
    }
};

#endif // FLATBOARDPIECE_H

