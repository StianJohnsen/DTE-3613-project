

#ifndef FLATBOARDPIECE_H
#define FLATBOARDPIECE_H

#include "meshCreation.h"
#include "trackPiece.h"
#include <iostream>

class FlatBoardPiece : public TrackPiece {
public:
    float length;
    float slopeAngle;

    FlatBoardPiece(float lengthMeters, float slopeRadians)
        : length(lengthMeters), slopeAngle(slopeRadians)
    {}

protected:

    void doGenerateControlPoints(
        const glm::vec3 &attachPoint,
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

        // Smooth: p0 = attachPoint
        glm::vec3 p0 = attachPoint;

        // Move forward slightly to define start tangent consistently
        glm::vec3 p1 = attachPoint + tiltedForward * (L * 0.33f);

        // Next control points continue in same direction
        glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.66f);
        glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.0f);

        controlPoints.push_back(p0);
        controlPoints.push_back(p1);
        controlPoints.push_back(p2);
        controlPoints.push_back(p3);
    }


    // void doGenerateControlPoints(const glm::vec3 &attachPoint,
    //                              const glm::vec3 &attachTangent,
    //                              const glm::vec3 &attachRight,
    //                              const glm::vec3 &attachUp) override
    // {
    //     controlPoints.clear();

    //     glm::vec3 forward = glm::normalize(attachTangent);
    //     glm::vec3 right   = glm::normalize(attachRight);
    //     glm::vec3 up      = glm::normalize(attachUp);

    //     glm::vec3 tiltedForward =
    //         glm::normalize(glm::rotate(forward, slopeAngle, right));

    //     float L = length;

    //     glm::vec3 p0 = attachPoint - tiltedForward * (L * 0.25f);
    //     glm::vec3 p1 = attachPoint;
    //     glm::vec3 p2 = attachPoint + tiltedForward * (L * 0.5f);
    //     glm::vec3 p3 = attachPoint + tiltedForward * (L * 1.0f);

    //     controlPoints.push_back(p0);
    //     controlPoints.push_back(p1);
    //     controlPoints.push_back(p2);
    //     controlPoints.push_back(p3);
    // }

    void generateMesh() override {
        mesh = createFlatBoard(smoothedPath, frameT, frameN, frameB,
                               trackRadius * 2.0f);
    }

};

#endif // FLATBOARDPIECE_H
