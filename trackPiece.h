// // #ifndef TRACKPIECE_H
// // #define TRACKPIECE_H


// // #define GLM_ENABLE_EXPERIMENTAL

// // #include <vector>
// // #include <glm/glm.hpp>
// // #include <glm/gtx/rotate_vector.hpp>
// // #include "meshPipelineSetup.h"
// // #include <glm/gtx/norm.hpp>
// // // #include "meshCreation.h"

// // class TrackPiece{
// // public:

// //     std::vector<glm::vec3> controlPoints;

// //     std::vector<glm::vec3> smoothedPath;
// //     Mesh mesh;

// //     glm::vec3 startPoint, startTangent, startRight, startUp;
// //     glm::vec3 endPoint,   endTangent,   endRight,   endUp;

// //     float trackRadius = 4.0f;
// //     unsigned int pathResolution = 20;
// //     unsigned int arcSegments = 32;

// // public:

// //     // virtual void generateControlPoints(
// //     //     const glm::vec3& attachPoint,
// //     //     const glm::vec3& attachDir
// //     //     ) = 0;

// //     virtual void generateControlPoints(
// //         const glm::vec3 &attachPoint,
// //         const glm::vec3 &attachTangent,
// //         const glm::vec3 &attachRight,
// //         const glm::vec3 &attachUp) = 0;


// //     void generateControlPoints(const glm::vec3& attachPoint,
// //                                const glm::vec3& attachDir)
// //     {
// //         glm::vec3 forward = glm::normalize(attachDir);
// //         glm::vec3 worldUp(0,1,0);

// //         glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
// //         glm::vec3 up    = glm::cross(forward, right);

// //         generateControlPoints(attachPoint, forward, right, up);
// //     }



// //     void build() {
// //         if (controlPoints.size() < 2) return;

// //         // -----------------------------------------
// //         // 1) Build PT frames directly from controlPoints
// //         // -----------------------------------------
// //         size_t N = controlPoints.size();

// //         std::vector<glm::vec3> T(N);
// //         std::vector<glm::vec3> Nn(N);
// //         std::vector<glm::vec3> B(N);

// //         // --- Initial tangent ---
// //         T[0] = glm::normalize(controlPoints[1] - controlPoints[0]);

// //         // --- Choose stable initial normal ---
// //         glm::vec3 arbitrary =
// //             (fabs(T[0].y) < 0.9f ? glm::vec3(0,1,0) : glm::vec3(1,0,0));

// //         Nn[0] = glm::normalize(glm::cross(arbitrary, T[0]));
// //         B[0]  = glm::cross(T[0], Nn[0]);

// //         // --- Parallel Transport over control points ---
// //         for (size_t i = 1; i < N; i++) {
// //             glm::vec3 t_prev = T[i-1];
// //             glm::vec3 t      = glm::normalize(controlPoints[i] - controlPoints[i-1]);

// //             T[i] = t;

// //             glm::vec3 v = glm::cross(t_prev, t);
// //             float c = glm::dot(t_prev, t);

// //             if (glm::length(v) < 1e-6f) {
// //                 Nn[i] = Nn[i-1];
// //             } else {
// //                 float s = glm::length(v);
// //                 glm::vec3 axis = v / s;
// //                 float angle = atan2(s, c);

// //                 glm::mat3 K(
// //                     0, -axis.z, axis.y,
// //                     axis.z, 0, -axis.x,
// //                     -axis.y, axis.x, 0
// //                     );

// //                 glm::mat3 R =
// //                     glm::mat3(1.0f) * glm::cos(angle)
// //                     + (1.0f - glm::cos(angle)) * glm::outerProduct(axis, axis)
// //                     + glm::sin(angle) * K;

// //                 Nn[i] = glm::normalize(R * Nn[i-1]);
// //             }

// //             B[i] = glm::cross(T[i], Nn[i]);
// //         }

// //         // -----------------------------------------
// //         // 2) Store start and end frames using PT frames
// //         // -----------------------------------------
// //         // startPoint   = controlPoints.front();
// //         // startTangent = T[0];
// //         // startRight   = Nn[0];
// //         // startUp      = B[0];

// //         // endPoint     = controlPoints.back();
// //         // endTangent   = T.back();
// //         // endRight     = Nn.back();
// //         // endUp        = B.back();


// //         startPoint = controlPoints.front();
// //         startTangent = glm::normalize(controlPoints[1] - controlPoints[0]);

// //         glm::vec3 arbitraryUp =
// //             (fabs(startTangent.y) < 0.9f)
// //                 ? glm::vec3(0,1,0)
// //                 : glm::vec3(1,0,0);

// //         startRight = glm::normalize(glm::cross(arbitraryUp, startTangent));
// //         startUp    = glm::normalize(glm::cross(startTangent, startRight));


// //         // -----------------------------------------
// //         // 3) Generate mesh AFTER we fix the frames
// //         // -----------------------------------------
// //         generateMesh();


// //         // ==============================
// //         // 3) Compute END frame
// //         // ==============================
// //         size_t last = controlPoints.size() - 1;

// //         endPoint   = controlPoints[last];
// //         endTangent = glm::normalize(controlPoints[last] - controlPoints[last - 1]);


// //         // choose an up that is not parallel to the new tangent
// //         glm::vec3 endArbitraryUp =
// //             (fabs(endTangent.y) < 0.9f)
// //                 ? glm::vec3(0,1,0)
// //                 : glm::vec3(1,0,0);

// //         endRight = glm::normalize(glm::cross(endArbitraryUp, endTangent));

// //         // If degenerate (rare), fall back to startRight
// //         if (glm::length2(endRight) < 1e-6f)
// //             endRight = startRight;

// //         endUp = glm::normalize(glm::cross(endTangent, endRight));
// //     }


// //     // void build() {
// //     //     if(controlPoints.empty()) return;

// //     //     // Store start frame
// //     //     startPoint = controlPoints.front();
// //     //     glm::vec3 tangentStart = glm::normalize(controlPoints[1] - controlPoints[0]);
// //     //     glm::vec3 arbitrary = (fabs(tangentStart.y) < 0.9f) ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
// //     //     startRight = glm::normalize(glm::cross(arbitrary, tangentStart));
// //     //     startUp = glm::normalize(glm::cross(tangentStart, startRight));
// //     //     startTangent = tangentStart;

// //     //     // Generate mesh
// //     //     generateMesh();

// //     //     // Store end frame using last segment of control points
// //     //     size_t last = controlPoints.size() - 1;
// //     //     glm::vec3 tangentEnd = glm::normalize(controlPoints[last] - controlPoints[last-1]);
// //     //     endPoint = controlPoints.back();
// //     //     endTangent = tangentEnd;

// //     //     glm::vec3 rightEnd = glm::normalize(glm::cross(arbitrary, tangentEnd));


// //     //     if(glm::length2(rightEnd) < 1e-6f)
// //     //         rightEnd = startRight; // fallback
// //     //     endRight = rightEnd;
// //     //     endUp = glm::normalize(glm::cross(tangentEnd, endRight));
// //     // }

// //     // void build(){
// //     //     computeSpline();
// //     //     computeTangents();
// //     //     generateMesh();
// //     // }


// // protected:


// //     glm::vec3 catmullRom(
// //         const glm::vec3& p0,
// //         const glm::vec3& p1,
// //         const glm::vec3& p2,
// //         const glm::vec3& p3,
// //         float t)
// //     {
// //         float t2 = t*t;
// //         float t3 = t2*t;

// //         return 0.5f * (
// //             (2.0f * p1) +
// //             (-p0 + p2) * t +
// //             (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
// //             (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
// //             );

// //     }


// //     void computeSpline(){
// //         smoothedPath.clear();

// //         if(controlPoints.size() < 4)
// //             return;

// //         const int SAMPLES_PER_SEGMENT = 20;

// //         for(int i = 0; i < controlPoints.size() - 3; ++i){

// //             glm::vec3 p0 = controlPoints[i + 0];
// //             glm::vec3 p1 = controlPoints[i + 1];
// //             glm::vec3 p2 = controlPoints[i + 2];
// //             glm::vec3 p3 = controlPoints[i + 3];

// //             for(int j = 0; j < SAMPLES_PER_SEGMENT; ++j){
// //                 float t = float(j) / float(SAMPLES_PER_SEGMENT - 1);
// //                 smoothedPath.push_back(catmullRom(p0,p1,p2,p3,t));
// //             }


// //         }
// //     }


// //     void computeTangents(){

// //         startPoint = smoothedPath.front();
// //         endPoint = smoothedPath.back();


// //         startTangent = glm::normalize(smoothedPath[1] - smoothedPath[0]);
// //         endTangent = glm::normalize(
// //             smoothedPath[smoothedPath.size() - 1] -
// //                 smoothedPath[smoothedPath.size() - 2]
// //             );

// //     }

// //     virtual void generateMesh() = 0;

// //     // virtual void generateMesh(){
// //     //     mesh = createHalfpipe(
// //     //         smoothedPath,
// //     //         trackRadius,
// //     //         pathResolution,
// //     //         arcSegments
// //     //         );
// //     // }
// // };


// // #endif // TRACKPIECE_H


// #ifndef TRACKPIECE_H
// #define TRACKPIECE_H

// #define GLM_ENABLE_EXPERIMENTAL

// #include <vector>
// #include <glm/glm.hpp>
// #include <glm/gtx/rotate_vector.hpp>
// #include <glm/gtx/norm.hpp>

// #include "meshPipelineSetup.h"

// class TrackPiece {
// public:
//     // Input: sparse path definition
//     std::vector<glm::vec3> controlPoints;

//     // Output: dense path along which we build the mesh
//     std::vector<glm::vec3> smoothedPath;

//     // Parallel transport frames along smoothedPath
//     std::vector<glm::vec3> frameT; // tangent
//     std::vector<glm::vec3> frameN; // right
//     std::vector<glm::vec3> frameB; // up

//     Mesh mesh;

//     glm::vec3 startPoint, startTangent, startRight, startUp;
//     glm::vec3 endPoint,   endTangent,   endRight,   endUp;

//     float        trackRadius    = 4.0f;
//     unsigned int pathResolution = 20;  // samples per segment for Catmull-Rom
//     unsigned int arcSegments    = 32;

// public:
//     virtual ~TrackPiece() = default;

//     // Full frame version – preferred
//     virtual void generateControlPoints(
//         const glm::vec3 &attachPoint,
//         const glm::vec3 &attachTangent,
//         const glm::vec3 &attachRight,
//         const glm::vec3 &attachUp) = 0;

//     // Convenience version: only point + direction
//     void generateControlPoints(const glm::vec3& attachPoint,
//                                const glm::vec3& attachDir)
//     {
//         glm::vec3 forward = glm::normalize(attachDir);
//         glm::vec3 worldUp(0,1,0);

//         glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
//         glm::vec3 up    = glm::normalize(glm::cross(forward, right));

//         generateControlPoints(attachPoint, forward, right, up);
//     }

//     // Main pipeline
//     void build() {
//         if (controlPoints.size() < 4)
//             return;

//         // 1) build dense Catmull–Rom path from control points
//         computeSpline();        // fills smoothedPath

//         if (smoothedPath.size() < 2)
//             return;

//         // 2) compute parallel–transport frames on the *same* path
//         computeFrames();        // fills frameT / frameN / frameB

//         // 3) set start/end frames from smoothedPath + PT frames
//         startPoint   = smoothedPath.front();
//         startTangent = frameT.front();
//         startRight   = frameN.front();
//         startUp      = frameB.front();

//         endPoint     = smoothedPath.back();
//         endTangent   = frameT.back();
//         endRight     = frameN.back();
//         endUp        = frameB.back();

//         // 4) finally build the actual mesh using smoothedPath + frames
//         generateMesh();
//     }

// protected:
//     // Catmull–Rom interpolation
//     glm::vec3 catmullRom(
//         const glm::vec3& p0,
//         const glm::vec3& p1,
//         const glm::vec3& p2,
//         const glm::vec3& p3,
//         float t) const
//     {
//         float t2 = t * t;
//         float t3 = t2 * t;

//         return 0.5f * (
//                    (2.0f * p1) +
//                    (-p0 + p2) * t +
//                    (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
//                    (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
//                    );
//     }

//     // Build smoothedPath from controlPoints using Catmull–Rom
//     virtual void computeSpline() {
//         smoothedPath.clear();

//         if (controlPoints.size() < 4)
//             return;

//         const int SAMPLES_PER_SEGMENT = pathResolution;

//         const int n = static_cast<int>(controlPoints.size());
//         for (int i = 0; i < n - 3; ++i) {
//             glm::vec3 p0 = controlPoints[i + 0];
//             glm::vec3 p1 = controlPoints[i + 1];
//             glm::vec3 p2 = controlPoints[i + 2];
//             glm::vec3 p3 = controlPoints[i + 3];

//             for (int j = 0; j < SAMPLES_PER_SEGMENT; ++j) {
//                 float t = float(j) / float(SAMPLES_PER_SEGMENT - 1);
//                 smoothedPath.push_back(catmullRom(p0, p1, p2, p3, t));
//             }
//         }

//         // Ensure the exact last control point is included
//         smoothedPath.push_back(controlPoints.back());
//     }

//     // Build PT frame along smoothedPath
//     void computeFrames() {
//         frameT.clear();
//         frameN.clear();
//         frameB.clear();

//         const size_t N = smoothedPath.size();
//         if (N < 2)
//             return;

//         frameT.resize(N);
//         frameN.resize(N);
//         frameB.resize(N);

//         // Initial tangent
//         frameT[0] = glm::normalize(smoothedPath[1] - smoothedPath[0]);

//         // Choose stable initial up
//         glm::vec3 arbitraryUp(0, 1, 0);
//         if (fabs(glm::dot(frameT[0], arbitraryUp)) > 0.9f)
//             arbitraryUp = glm::vec3(1, 0, 0);

//         glm::vec3 right0 = glm::normalize(glm::cross(arbitraryUp, frameT[0]));
//         glm::vec3 up0    = glm::normalize(glm::cross(frameT[0], right0));

//         frameN[0] = right0;
//         frameB[0] = up0;

//         // Parallel transport
//         for (size_t i = 1; i < N; ++i) {
//             glm::vec3 t_prev = frameT[i - 1];
//             glm::vec3 t      = glm::normalize(smoothedPath[i] - smoothedPath[i - 1]);

//             frameT[i] = t;

//             glm::vec3 v = glm::cross(t_prev, t);
//             float      c = glm::dot(t_prev, t);

//             if (glm::length(v) < 1e-6f) {
//                 // no rotation
//                 frameN[i] = frameN[i - 1];
//             } else {
//                 float s = glm::length(v);
//                 glm::vec3 axis = v / s;
//                 float angle = atan2(s, c);

//                 glm::mat3 K(
//                     0,        -axis.z,  axis.y,
//                     axis.z,    0,      -axis.x,
//                     -axis.y,   axis.x,  0
//                     );

//                 glm::mat3 R =
//                     glm::mat3(1.0f) * glm::cos(angle)
//                     + (1.0f - glm::cos(angle)) * glm::outerProduct(axis, axis)
//                     + glm::sin(angle) * K;

//                 frameN[i] = glm::normalize(R * frameN[i - 1]);
//             }

//             frameB[i] = glm::cross(frameT[i], frameN[i]);
//         }
//     }

//     // Implemented by child
//     virtual void generateMesh() = 0;
// };

// #endif // TRACKPIECE_H

#ifndef TRACKPIECE_H
#define TRACKPIECE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#include "meshPipelineSetup.h"

class TrackPiece {
public:
    // Input: sparse path definition
    std::vector<glm::vec3> controlPoints;

    // Output: dense path along which we build the mesh
    std::vector<glm::vec3> smoothedPath;

    // Parallel transport frames along smoothedPath
    std::vector<glm::vec3> frameT; // tangent
    std::vector<glm::vec3> frameN; // right
    std::vector<glm::vec3> frameB; // up

    Mesh mesh;

    glm::vec3 startPoint, startTangent, startRight, startUp;
    glm::vec3 endPoint,   endTangent,   endRight,   endUp;

    float        trackRadius    = 10.0f;
    unsigned int pathResolution = 20;  // samples per segment for Catmull-Rom
    unsigned int arcSegments    = 32;

    float twistStart = 0.0f;
    float twistEnd = 0.0f;

    bool isEntrance = true;

    float slopeAngle = 0.0f;

    TrackPiece() = default;
    virtual ~TrackPiece() = default;

    virtual void setTwist(float a, float b) {
        twistStart = a;
        twistEnd = b;
    }

    void setIsEntrance(bool isTrue){
        isEntrance = isTrue;
    }

    void setSlope(float angleRad) {
        slopeAngle = angleRad;
    }


    // ---------------------------
    // PUBLIC API for pieces
    // ---------------------------

    // 1) Attach with full frame
    void generateControlPoints(const glm::vec3 &attachPoint,
                               const glm::vec3 &attachTangent,
                               const glm::vec3 &attachRight,
                               const glm::vec3 &attachUp)
    {
        incomingT = glm::normalize(attachTangent);
        incomingR = glm::normalize(attachRight);
        incomingU = glm::normalize(attachUp);
        hasIncoming = true;

        doGenerateControlPoints(attachPoint, incomingT, incomingR, incomingU);
    }

    // 2) Attach with just point + direction
    void generateControlPoints(const glm::vec3& attachPoint,
                               const glm::vec3& attachDir)
    {
        glm::vec3 forward = glm::normalize(attachDir);
        glm::vec3 worldUp(0,1,0);
        if (fabs(glm::dot(forward, worldUp)) > 0.9f)
            worldUp = glm::vec3(1,0,0);

        glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
        glm::vec3 up    = glm::normalize(glm::cross(forward, right));

        generateControlPoints(attachPoint, forward, right, up);
    }

    // Main pipeline
    void build() {
        if (controlPoints.size() < 2)
            return;

        // 1) build dense path
        computeSpline();        // fills smoothedPath

        if (smoothedPath.size() < 2)
            return;

        // 2) compute PT frames
        computeFrames();        // fills frameT / frameN / frameB

        // 3) set start/end frames from smoothedPath + PT frames
        startPoint   = smoothedPath.front();
        startTangent = frameT.front();
        startRight   = frameN.front();
        startUp      = frameB.front();

        endPoint     = smoothedPath.back();
        endTangent   = frameT.back();
        endRight     = frameN.back();
        endUp        = frameB.back();

        // 4) build the actual mesh
        generateMesh();
    }

protected:
    // incoming frame from previous piece
    glm::vec3 incomingT{0,0,1};
    glm::vec3 incomingR{1,0,0};
    glm::vec3 incomingU{0,1,0};
    bool      hasIncoming = false;


    // virtual void setTwist(float startRad, float endRad);


    // Children override THIS instead of generateControlPoints
    virtual void doGenerateControlPoints(
        const glm::vec3 &attachPoint,
        const glm::vec3 &attachTangent,
        const glm::vec3 &attachRight,
        const glm::vec3 &attachUp) = 0;

    // Catmull–Rom interpolation
    glm::vec3 catmullRom(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        float t) const
    {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
                   (2.0f * p1) +
                   (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
                   );
    }

    // Build smoothedPath from controlPoints using Catmull–Rom
    // (children can override this if they already provide a dense path)
    virtual void computeSpline() {
        smoothedPath.clear();

        if (controlPoints.size() < 4) {
            // if too few, just copy them
            smoothedPath = controlPoints;
            return;
        }

        const int SAMPLES_PER_SEGMENT = pathResolution;
        const int n = static_cast<int>(controlPoints.size());

        for (int i = 0; i < n - 3; ++i) {
            glm::vec3 p0 = controlPoints[i + 0];
            glm::vec3 p1 = controlPoints[i + 1];
            glm::vec3 p2 = controlPoints[i + 2];
            glm::vec3 p3 = controlPoints[i + 3];

            for (int j = 0; j < SAMPLES_PER_SEGMENT; ++j) {
                float t = float(j) / float(SAMPLES_PER_SEGMENT - 1);
                smoothedPath.push_back(catmullRom(p0, p1, p2, p3, t));
            }
        }

        smoothedPath.push_back(controlPoints.back());
    }


    void computeFrames() {
        frameT.clear();
        frameN.clear();
        frameB.clear();

        const size_t N = smoothedPath.size();
        if (N < 2)
            return;

        frameT.resize(N);
        frameN.resize(N);
        frameB.resize(N);

        if (hasIncoming) {
            // --- CONTINUOUS STITCHING CASE ---
            // Use the EXACT same frame as the previous piece at the seam.
            frameT[0] = glm::normalize(incomingT);
            frameN[0] = glm::normalize(incomingR);
            frameB[0] = glm::normalize(incomingU);
        } else {
            // --- FIRST PIECE / NO PARENT ---
            glm::vec3 T0 = glm::normalize(smoothedPath[1] - smoothedPath[0]);
            frameT[0]    = T0;

            glm::vec3 arbitraryUp(0,1,0);
            if (fabs(glm::dot(T0, arbitraryUp)) > 0.9f)
                arbitraryUp = glm::vec3(1,0,0);

            glm::vec3 R0 = glm::normalize(glm::cross(arbitraryUp, T0));
            glm::vec3 U0 = glm::normalize(glm::cross(T0, R0));

            frameN[0] = R0;
            frameB[0] = U0;
        }

        // --- Parallel transport along this piece ---
        for (size_t i = 1; i < N; ++i) {
            glm::vec3 t_prev = frameT[i - 1];
            glm::vec3 t      = glm::normalize(smoothedPath[i] - smoothedPath[i - 1]);

            frameT[i] = t;

            glm::vec3 v = glm::cross(t_prev, t);
            float      c = glm::dot(t_prev, t);

            if (glm::length(v) < 1e-6f) {
                // no rotation
                frameN[i] = frameN[i - 1];
            } else {
                float s = glm::length(v);
                glm::vec3 axis = v / s;
                float angle = atan2(s, c);

                glm::mat3 K(
                    0,        -axis.z,  axis.y,
                    axis.z,    0,      -axis.x,
                    -axis.y,   axis.x,  0
                    );

                glm::mat3 R =
                    glm::mat3(1.0f) * glm::cos(angle)
                    + (1.0f - glm::cos(angle)) * glm::outerProduct(axis, axis)
                    + glm::sin(angle) * K;

                frameN[i] = glm::normalize(R * frameN[i - 1]);
            }

            frameB[i] = glm::cross(frameT[i], frameN[i]);


            // Apply transition twist if this piece requests it
            // if (twistStart != twistEnd) {
            //     float t = float(i) / float(N - 1);
            //     float roll = glm::mix(twistStart, twistEnd, t * t * (3 - 2 * t)); // smoothstep

            //     // Rotate N and B around T
            //     glm::mat4 Rtwist = glm::rotate(glm::mat4(1.0f), roll, frameT[i]);

            //     frameN[i] = glm::vec3(Rtwist * glm::vec4(frameN[i], 1.0f));
            //     frameB[i] = glm::vec3(Rtwist * glm::vec4(frameB[i], 1.0f));
            // }

            // ------------------------------------------------------
            // APPLY OPTIONAL TWIST (roll interpolation along piece)
            // ------------------------------------------------------
            if (twistStart != 0.0f || twistEnd != 0.0f)
            {
                float u = float(i) / float(N - 1);
                float roll = glm::mix(twistStart, twistEnd, u * u * (3 - 2*u)); // smoothstep

                glm::mat4 Rtwist = glm::rotate(glm::mat4(1.0f), roll, frameT[i]);

                frameN[i] = glm::normalize(glm::vec3(Rtwist * glm::vec4(frameN[i], 0.0f)));
                frameB[i] = glm::normalize(glm::vec3(Rtwist * glm::vec4(frameB[i], 0.0f)));
            }


        }
    }


    // Implemented by child
    virtual void generateMesh()=0;





};

#endif // TRACKPIECE_H
