#ifndef TRACKPIECE_H
#define TRACKPIECE_H

#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "meshPipelineSetup.h"

class TrackPiece {
public:

    std::vector<glm::vec3> controlPoints;

    std::vector<glm::vec3> smoothedPath;

    Mesh mesh;

    float trackRadius    = 10.0f;
    unsigned int pathResolution = 80;

    TrackPiece() = default;
    virtual ~TrackPiece() = default;


    void generateControlPoints(const glm::vec3& attachPoint,
                               const glm::vec3& attachDir)
    {
        glm::vec3 forward = glm::normalize(attachDir);
        glm::vec3 worldUp(0,1,0);

        if (fabs(glm::dot(forward, worldUp)) > 0.9f)
            worldUp = glm::vec3(1,0,0);

        glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
        glm::vec3 up    = glm::normalize(glm::cross(forward, right));

        doGenerateControlPoints(attachPoint, forward, right, up);
    }


    void build() {
        if (controlPoints.size() < 2)
            return;

        computeSpline();
        if (smoothedPath.size() < 2)
            return;

        generateMesh();
    }


    void getPointAndFrame(float t, glm::vec3& P, glm::vec3& T, glm::vec3& R, glm::vec3& U)
    {
        if (smoothedPath.empty()) return;

        float fidx = t * (smoothedPath.size() - 1);
        int i0 = int(fidx);
        float frac = fidx - i0;

        int i1 = std::min(i0 + 1, int(smoothedPath.size() - 1));

        P = glm::mix(smoothedPath[i0], smoothedPath[i1], frac);

        glm::vec3 d = smoothedPath[i1] - smoothedPath[i0];
        if (glm::length(d) < 0.0001f)
            d = glm::vec3(0,0,1);

        T = glm::normalize(d);

        glm::vec3 worldUp = glm::vec3(0,1,0);
        if (fabs(glm::dot(worldUp, T)) > 0.9f)
            worldUp = glm::vec3(1,0,0);

        R = glm::normalize(glm::cross(worldUp, T));
        U = glm::normalize(glm::cross(T, R));
    }

protected:


    virtual void doGenerateControlPoints(const glm::vec3& p,
                                         const glm::vec3& forward,
                                         const glm::vec3& right,
                                         const glm::vec3& up) = 0;

    virtual void generateMesh() = 0;


    glm::vec3 catmullRom(const glm::vec3& p0,
                         const glm::vec3& p1,
                         const glm::vec3& p2,
                         const glm::vec3& p3,
                         float t) const
    {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
                   (2.f * p1) +
                   (-p0 + p2) * t +
                   (2.f*p0 - 5.f*p1 + 4.f*p2 - p3) * t2 +
                   (-p0 + 3.f*p1 - 3.f*p2 + p3) * t3
                   );
    }

    void computeSpline() {
        smoothedPath.clear();

        const size_t n = controlPoints.size();
        if (n < 4) {
            smoothedPath = controlPoints;
            return;
        }

        const int S = pathResolution;

        for (int i = 0; i < n - 3; i++) {
            glm::vec3 p0 = controlPoints[i + 0];
            glm::vec3 p1 = controlPoints[i + 1];
            glm::vec3 p2 = controlPoints[i + 2];
            glm::vec3 p3 = controlPoints[i + 3];

            for (int j = 0; j < S; j++) {
                float t = float(j) / float(S - 1);
                smoothedPath.push_back(catmullRom(p0, p1, p2, p3, t));
            }
        }

        smoothedPath.push_back(controlPoints.back());
    }
};

#endif // TRACKPIECE_H
