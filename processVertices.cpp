#include "processVertices.h"

#include "hermiteClass.h"
#include "renderer.h"


glm::vec3 catmullRom(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
    ) {
    float t2= t * t;
    float t3 = t2*t;

    return 0.5f * (

               (2.0f * p1) +
               (-p0 + p2) * t +
               (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
               (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
               );

}

glm::vec3 getPointOnCatmullRomSpline(const std::vector<glm::vec3>& controlPoints, float t) {
    if (controlPoints.size() < 4)
        return glm::vec3(0.0f);

    t = glm::clamp(t, 0.0f, 1.0f);

    float segmentFloat = t * (controlPoints.size() - 3);
    int segment = static_cast<int>(segmentFloat);
    float localT = segmentFloat - segment;

    segment = glm::clamp(segment, 0, (int)controlPoints.size() - 4);

    const glm::vec3& p0 = controlPoints[segment];
    const glm::vec3& p1 = controlPoints[segment + 1];
    const glm::vec3& p2 = controlPoints[segment + 2];
    const glm::vec3& p3 = controlPoints[segment + 3];

    return catmullRom(p0, p1, p2, p3, localT);
}


glm::vec3 getTangentOnCatmullRomSpline(const std::vector<glm::vec3>& controlPoints, float t) {
    const float delta = 0.001f;
    glm::vec3 p1 = getPointOnCatmullRomSpline(controlPoints, t);
    glm::vec3 p2 = getPointOnCatmullRomSpline(controlPoints, glm::min(1.0f, t + delta));
    return glm::normalize(p2 - p1);
}






void createUVSphere(float radius,
                    unsigned int sectorCount,
                    unsigned int stackCount,
                    std::vector<float>& vertices,         // out: interleaved floats
                    std::vector<unsigned int>& indices){

    vertices.clear();
    indices.clear();

    const float PI = acos(-1.0f);

    float x, y, z, xy; // Vertices
    float nx, ny, nz; // Normals
    float s, t; // Textures Coordinates

    for(unsigned int i = 0; i <= stackCount; ++i){

        float stackAngle = PI / 2.0f - i * PI / (float)stackCount;

        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(unsigned int j = 0; j <= sectorCount; ++j){
            float sectorAngle = j * 2.0f * PI / (float)sectorCount;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            nx = x / radius;
            ny = y / radius;
            nz = z / radius;

            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            s = (float)j / (float)sectorCount;
            t = (float)i / (float)sectorCount;

            vertices.push_back(s);
            vertices.push_back(t);

            // SO FAR JUST BLUE COLOR !!!
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);


        }
    }

    for(unsigned int i = 0; i < stackCount; ++i){
        unsigned int k1 = i * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;

        for(unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2){
            if(i != 0){
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1)){
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);

            }
        }
    }

}

void createUVSkybox(std::vector<float>& vertices){

    vertices = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };


}


void createUVTrack(
    const std::vector<glm::vec3>& controllPoints,
    const float& width,
    unsigned int resolution,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    )
{
    vertices.clear();
    indices.clear();


    if(controllPoints.size() < 2) return;

    const float halfWidth = width * 0.5f;
    const float railHeight = width * 0.1f;
    const float railThickness = width * 0.05f;

    std::vector<glm::vec3> path;

    for(size_t i = 0; i < controllPoints.size() - 1; ++i){
        for(size_t j = 0; j < resolution; ++j){
            float t = static_cast<float>(j) / resolution;
            // glm::vec3 p = glm::mix(controllPoints[i], controllPoints[i + 1], t);
            // path.push_back(p);
            glm::vec3 p0 = (i==0) ? controllPoints[i] : controllPoints[i - 1];
            glm::vec3 p1 = controllPoints[i];
            glm::vec3 p2 = controllPoints[i + 1];
            glm::vec3 p3 = (i + 2 < controllPoints.size()) ? controllPoints[i + 2] : controllPoints[i + 1];

            glm::vec3 p = catmullRom(p0,p1,p2,p3,t);
            path.push_back(p);
        }
    }

    path.push_back(controllPoints.back());

    size_t baseVertexCount = 0;

    for(size_t i = 0; i < path.size(); ++i){
        glm::vec3 forward;
        if(i < path.size() - 1) forward = glm::normalize(path[i + 1] - path[i]);
        else forward = glm::normalize(path[i] - path[i - 1]);


        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward,up));

        glm::vec3 normal = glm::normalize(glm::cross(right,forward));

        glm::vec3 leftPos = path[i] - right * halfWidth;
        glm::vec3 rightPos = path[i] + right * halfWidth;

        glm::vec3 leftRailBottom = leftPos;
        glm::vec3 leftRailTop = leftPos + normal * railHeight;

        glm::vec3 rightRailBottom = rightPos;
        glm::vec3 rightRailTop = rightPos + normal * railHeight;

        float texV = static_cast<float>(i) / (path.size() - 1);

        float c = 0.5f + 0.5f * sinf(i * 0.1f);

        vertices.insert(vertices.end(), {
            leftPos.x,leftPos.y,leftPos.z,
            normal.x,normal.y,normal.z,
            0.0f,texV,
            c,0.7f,0.7f
        });

        vertices.insert(vertices.end(),{
            rightPos.x,rightPos.y,rightPos.z,
            normal.x,normal.y,normal.z,
            1.0f,texV,
            c,0.7f,0.7f
        });

        vertices.insert(vertices.end(),{
            leftRailBottom.x, leftRailBottom.y, leftRailBottom.z,
            normal.x, normal.y, normal.z,
            0.0f,texV,
            0.7f,0.7f,0.7f
        });

        vertices.insert(vertices.end(),{
            leftRailTop.x, leftRailTop.y, leftRailTop.z,
            normal.x, normal.y, normal.z,
            0.0f,texV,
            0.9f,0.9f,0.9f
        });


        vertices.insert(vertices.end(),{
            rightRailBottom.x, rightRailBottom.y, rightRailBottom.z,
            normal.x, normal.y, normal.z,
            1.0f,texV,
            0.7f,0.7f,0.7f
            });


        vertices.insert(vertices.end(),{
            rightRailTop.x, rightRailTop.y, rightRailTop.z,
            normal.x, normal.y, normal.z,
            1.0f,texV,
            0.9f,0.9f,0.9f
        });

    };

    for(size_t i = 0; i< path.size() - 1; ++i){

        unsigned int base = static_cast<unsigned int>(i * 6);

        // unsigned int k = static_cast<unsigned int>(i * 2);
        unsigned int k = baseVertexCount + i * 6;

        indices.insert(indices.end(),{
            k+0,k+1,k+6,
            k+1,k+7,k+6
        });

        indices.insert(indices.end(),{
            k+2,k+3,k+8,
            k+3,k+9,k+8
        });

        indices.insert(indices.end(),{
            k+4,k+5,k+10,
            k+5,k+11,k+10
        });
    }
}


void createUVHalfpipe(
    const HermiteTrack& track,
    float radius,
    unsigned int pathResolution,
    unsigned int arcSegments,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    ){

    vertices.clear();
    indices.clear();

    const unsigned int ringCount = arcSegments + 1;

    std::vector<glm::vec3> path;
    std::vector<glm::vec3> tangents;

    for(unsigned int i = 0; i <= pathResolution; ++i){
        float t = float(i) / float(pathResolution);
        path.push_back(track.evaluate(t));
        tangents.push_back(glm::normalize(track.tangent(t)));
    }


    for(unsigned int i = 0; i <= pathResolution; ++i){

        glm::vec3 pos = path[i];
        glm::vec3 forward = tangents[i];

        glm::vec3 worldUp(0,1,0);
        glm::vec3 right = glm::normalize(glm::cross(worldUp,forward));
        glm::vec3 up = glm::normalize(glm::cross(forward,right));

        float v = float(i) / float(pathResolution);

        for(unsigned int a = 0; a <= arcSegments; ++a){
            float arcT = float(a) / float(arcSegments);
            float angle = -glm::half_pi<float>() + arcT * glm::pi<float>();

            float x = radius * std::sin(angle);
            float y = radius * ( 1- std::cos(angle));

            glm::vec3 worldPos = pos + right * x - up * y;

            glm::vec3 localNormal = glm::normalize(glm::vec3(
                std::sin(angle), -std::cos(angle),0.0f));

            glm::vec3 normal = glm::normalize(right * localNormal.x - up * localNormal.y);

            vertices.push_back(worldPos.x);
            vertices.push_back(worldPos.y);
            vertices.push_back(worldPos.z);

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            vertices.push_back(arcT);
            vertices.push_back(v);


        }
    }

    for(unsigned int i = 0; i < pathResolution; ++i){

        unsigned int base = i * ringCount;
        unsigned int next = (i + 1) * ringCount;

        for(unsigned int a = 0; a < ringCount; ++a){
            indices.push_back(base + a);
            indices.push_back(next + a);
        }

    }


}



void createUVHalfpipeFromHermite(
    const HermiteTrack& track,
    float width,
    float radius,
    unsigned int pathResolution,
    unsigned int arcSegments,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
    ){


    vertices.clear();
    indices.clear();

    for(unsigned int i = 0; i < pathResolution; ++i){

        float t = float(i) / float(pathResolution - 1);

        glm::vec3 pos = track.evaluate(t);
        glm::vec3 tangent = track.tangent(t);

        glm::vec3 globalUp(0,1,0);
        glm::vec3 right = glm::normalize(glm::cross(globalUp,tangent));
        glm::vec3 up = glm::normalize(glm::cross(right,tangent));

        float v = t;

        for(unsigned int a = 0; a <= arcSegments; ++a){
            float arcT = float(a) / float(arcSegments);
            float angle = -glm::half_pi<float>() + arcT * glm::pi<float>();

            float x = width * (arcT - 0.5f);
            float y = radius * (1.0f - cos(angle));

            glm::vec3 vertexPos = pos + right * x - up * y;

            glm::vec3 localNormal = glm::normalize(glm::vec3(std::sin(angle), -std::cos(angle),0.0f));
            glm::vec3 normal = glm::normalize(right * localNormal.x - up * localNormal.y);

            float u = arcT;

            vertices.push_back(vertexPos.x);
            vertices.push_back(vertexPos.y);
            vertices.push_back(vertexPos.z);

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

        unsigned int ringVertexCount = arcSegments + 1;

    for (size_t i = 0; i < pathResolution - 1; ++i) {
        unsigned int base = i * ringVertexCount;
        unsigned int next = (i + 1) * ringVertexCount;

        for (unsigned int a = 0; a < arcSegments; ++a) {
            // First triangle
            indices.push_back(base + a);
            indices.push_back(next + a);
            indices.push_back(base + a + 1);

            // Second triangle
            indices.push_back(base + a + 1);
            indices.push_back(next + a);
            indices.push_back(next + a + 1);
        }
    }
}



// void createUVHalfpipeWithRails(
//     const std::vector<glm::vec3>& controlPoints,
//     float radius,                 // radius of the halfpipe
//     unsigned int pathResolution,  // samples along the spline
//     unsigned int arcSegments,     // samples along semicircle cross-section
//     std::vector<float>& vertices,
//     std::vector<unsigned int>& indices)
// {
//     vertices.clear();
//     indices.clear();

//     if(controlPoints.size() < 2) return;

//     // -------------------------------------------
//     // 1. Generate spline path (Catmull–Rom)
//     // -------------------------------------------
//     std::vector<glm::vec3> path;

//     glm::vec3 prevUp = glm::vec3(0,1,0); // initial up vector


//     for(size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for(unsigned int j = 0; j < pathResolution; ++j) {
//             float t = float(j)/float(pathResolution);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i-1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i+1];
//             glm::vec3 p3 = (i+2 < controlPoints.size()) ? controlPoints[i+2] : controlPoints[i+1];

//             glm::vec3 p = catmullRom(p0,p1,p2,p3,t);
//             path.push_back(p);
//         }
//     }
//     path.push_back(controlPoints.back());

//     size_t ringVertexCount = arcSegments + 1;

//     // -------------------------------------------
//     // 2. Generate vertices
//     // -------------------------------------------
//     for(size_t i = 0; i < path.size(); ++i) {
//         glm::vec3 forward;
//         if(i < path.size()-1) forward = glm::normalize(path[i+1]-path[i]);
//         else forward = glm::normalize(path[i]-path[i-1]);

//         glm::vec3 worldUp(0,1,0);
//         glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
//         glm::vec3 up = glm::normalize(glm::cross(right, forward));

//         float v = float(i)/float(path.size()-1);

//         // Generate semicircle cross-section
//         for(unsigned int a = 0; a <= arcSegments; ++a) {
//             float arcT = float(a)/float(arcSegments);
//             float angle = -glm::half_pi<float>() + arcT * glm::pi<float>(); // -90° to +90°

//             float x = radius * std::sin(angle);        // horizontal offset
//             float y = radius * (1.0f - std::cos(angle)); // vertical offset (bottom = 0)

//             glm::vec3 pos = path[i] + right*x - up*y;

//             glm::vec3 localNormal = glm::normalize(glm::vec3(std::sin(angle), -std::cos(angle), 0.0f));
//             glm::vec3 normal = glm::normalize(right*localNormal.x - up*localNormal.y);

//             float u = arcT;

//             vertices.push_back(pos.x);
//             vertices.push_back(pos.y);
//             vertices.push_back(pos.z);

//             vertices.push_back(normal.x);
//             vertices.push_back(normal.y);
//             vertices.push_back(normal.z);

//             vertices.push_back(u);
//             vertices.push_back(v);
//         }

//         // Optional: add rails at edges
//         glm::vec3 leftRailTop = path[i] - right*radius + up*0.1f*radius;
//         glm::vec3 rightRailTop = path[i] + right*radius + up*0.1f*radius;
//         glm::vec3 leftRailBottom = path[i] - right*radius;
//         glm::vec3 rightRailBottom = path[i] + right*radius;

//         glm::vec3 railNormal(0,1,0); // simple vertical normal

//         // Left rail
//         vertices.insert(vertices.end(), { leftRailBottom.x, leftRailBottom.y, leftRailBottom.z,
//                                          railNormal.x, railNormal.y, railNormal.z,
//                                          0.0f, v });
//         vertices.insert(vertices.end(), { leftRailTop.x, leftRailTop.y, leftRailTop.z,
//                                          railNormal.x, railNormal.y, railNormal.z,
//                                          0.0f, v });

//         // Right rail
//         vertices.insert(vertices.end(), { rightRailBottom.x, rightRailBottom.y, rightRailBottom.z,
//                                          railNormal.x, railNormal.y, railNormal.z,
//                                          1.0f, v });
//         vertices.insert(vertices.end(), { rightRailTop.x, rightRailTop.y, rightRailTop.z,
//                                          railNormal.x, railNormal.y, railNormal.z,
//                                          1.0f, v });
//     }

//     // -------------------------------------------
//     // 3. Generate triangle strip indices
//     // -------------------------------------------
//     for(size_t i = 0; i < path.size()-1; ++i) {
//         unsigned int base = i * (ringVertexCount);
//         unsigned int next = (i+1) * (ringVertexCount);

//         for(unsigned int a = 0; a < ringVertexCount-1; ++a) {
//             indices.push_back(base+a);
//             indices.push_back(next+a);
//             indices.push_back(base+a+1);

//             indices.push_back(base+a+1);
//             indices.push_back(next+a);
//             indices.push_back(next+a+1);
//         }
//     }

//     // Optional: connect rails if desired (could be added similarly)
// }


// void createUVHalfpipeWithRails(
//     const std::vector<glm::vec3>& controlPoints,
//     float radius,                 // radius of the halfpipe
//     unsigned int pathResolution,  // samples along the spline between control points
//     unsigned int arcSegments,     // samples across semicircle (>=3)
//     std::vector<float>& vertices, // OUT: interleaved floats (pos(3), normal(3), uv(2), color(3))
//     std::vector<unsigned int>& indices) // OUT
// {
//     vertices.clear();
//     indices.clear();

//     if (controlPoints.size() < 2) return;
//     if (arcSegments < 2) arcSegments = 2;

//     // 1) sample Catmull-Rom path (same as your working code)
//     std::vector<glm::vec3> path;
//     for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for (unsigned int j = 0; j < pathResolution; ++j) {
//             float t = static_cast<float>(j) / static_cast<float>(pathResolution);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i + 1];
//             glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

//             glm::vec3 p = catmullRom(p0, p1, p2, p3, t);
//             path.push_back(p);
//         }
//     }
//     path.push_back(controlPoints.back());

//     const unsigned int ringVerts = arcSegments + 1; // vertices for half-circle
//     const unsigned int railsPerRing = 4;            // left bottom/top, right bottom/top
//     const unsigned int vertsPerRing = ringVerts + railsPerRing;

//     // parameters for rails
//     const float railHeight = radius * 0.12f;
//     const glm::vec3 baseColor(0.6f, 0.6f, 0.6f);
//     const glm::vec3 railColor(0.9f, 0.9f, 0.9f);

//     // 2) build ring vertices (uses same stable up = (0,1,0) approach that worked for you)
//     for (size_t i = 0; i < path.size(); ++i) {
//         // forward
//         glm::vec3 forward;
//         if (i < path.size() - 1) forward = glm::normalize(path[i + 1] - path[i]);
//         else forward = glm::normalize(path[i] - path[i - 1]);

//         // keep same method as your working function
//         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//         glm::vec3 right = glm::normalize(glm::cross(forward, up));

//         // normal pointing "out" of halfpipe surface (computed from right & up)
//         // when building per-arc vertex we compute the proper normal below

//         float v = static_cast<float>(i) / static_cast<float>(path.size() - 1);

//         // semicircle from -90° to +90°
//         for (unsigned int a = 0; a <= arcSegments; ++a) {
//             float arcT = static_cast<float>(a) / static_cast<float>(arcSegments);
//             float angle = -glm::half_pi<float>() + arcT * glm::pi<float>(); // -pi/2 -> +pi/2

//             // local cross-section coords:
//             float x = radius * std::sin(angle);                    // lateral offset along right
//             float y = radius * (1.0f - std::cos(angle));           // vertical offset from rim downwards

//             glm::vec3 pos = path[i] + right * x + up * y;

//             // local normal in cross-section (points outward from pipe center)
//             glm::vec3 localNormal = glm::normalize(glm::vec3(std::sin(angle), std::cos(angle), 0.0f));
//             glm::vec3 normal = glm::normalize(right * localNormal.x + up * localNormal.y);

//             float u = arcT;

//             // push 11 floats: pos(3), normal(3), tex(2), color(3)
//             vertices.push_back(pos.x);
//             vertices.push_back(pos.y);
//             vertices.push_back(pos.z);

//             vertices.push_back(normal.x);
//             vertices.push_back(normal.y);
//             vertices.push_back(normal.z);

//             vertices.push_back(u);
//             vertices.push_back(v);

//             vertices.push_back(baseColor.r);
//             vertices.push_back(baseColor.g);
//             vertices.push_back(baseColor.b);
//         }

//         // add rails: left bottom, left top, right bottom, right top (4 verts)
//         glm::vec3 leftBottom = path[i] - right * radius;
//         glm::vec3 rightBottom = path[i] + right * radius;
//         glm::vec3 leftTop = leftBottom + up * railHeight;
//         glm::vec3 rightTop = rightBottom + up * railHeight;

//         // left bottom
//         vertices.push_back(leftBottom.x);
//         vertices.push_back(leftBottom.y);
//         vertices.push_back(leftBottom.z);

//         vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z); // normal
//         vertices.push_back(0.0f); vertices.push_back(v);
//         vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

//         // left top
//         vertices.push_back(leftTop.x);
//         vertices.push_back(leftTop.y);
//         vertices.push_back(leftTop.z);

//         vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
//         vertices.push_back(0.0f); vertices.push_back(v);
//         vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

//         // right bottom
//         vertices.push_back(rightBottom.x);
//         vertices.push_back(rightBottom.y);
//         vertices.push_back(rightBottom.z);

//         vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
//         vertices.push_back(1.0f); vertices.push_back(v);
//         vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

//         // right top
//         vertices.push_back(rightTop.x);
//         vertices.push_back(rightTop.y);
//         vertices.push_back(rightTop.z);

//         vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
//         vertices.push_back(1.0f); vertices.push_back(v);
//         vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);
//     }

//     // 3) indices: stitch rings -> quads (two triangles each)
//     // each ring has `ringVerts` halfpipe verts then 4 rail verts
//     for (size_t i = 0; i < path.size() - 1; ++i) {
//         unsigned int base = static_cast<unsigned int>(i * vertsPerRing);
//         unsigned int next = static_cast<unsigned int>((i + 1) * vertsPerRing);

//         // halfpipe surface
//         for (unsigned int a = 0; a < ringVerts - 1; ++a) {
//             unsigned int b0 = base + a;
//             unsigned int b1 = base + a + 1;
//             unsigned int n0 = next + a;
//             unsigned int n1 = next + a + 1;

//             // two triangles: (b0, n0, b1) and (b1, n0, n1)
//             indices.push_back(b0);
//             indices.push_back(n0);
//             indices.push_back(b1);

//             indices.push_back(b1);
//             indices.push_back(n0);
//             indices.push_back(n1);
//         }

//         // // rails: left (two verts per ring: L0=Lbase+ringVerts, L1=Lbase+ringVerts+1)
//         // unsigned int L0 = base + ringVerts;
//         // unsigned int L1 = base + ringVerts + 1;
//         // unsigned int R0 = base + ringVerts + 2;
//         // unsigned int R1 = base + ringVerts + 3;

//         // unsigned int NL0 = next + ringVerts;
//         // unsigned int NL1 = next + ringVerts + 1;
//         // unsigned int NR0 = next + ringVerts + 2;
//         // unsigned int NR1 = next + ringVerts + 3;

//         // // left rail quad -> two tris
//         // indices.push_back(L0); indices.push_back(NL0); indices.push_back(L1);
//         // indices.push_back(L1); indices.push_back(NL0); indices.push_back(NL1);

//         // // right rail quad -> two tris
//         // indices.push_back(R0); indices.push_back(NR0); indices.push_back(R1);
//         // indices.push_back(R1); indices.push_back(NR0); indices.push_back(NR1);
//     }
// }

void createUVHalfpipeWithRails(
    const std::vector<glm::vec3>& controlPoints,
    float radius,                 // radius of the halfpipe
    unsigned int pathResolution,  // samples along the spline between control points
    unsigned int arcSegments,     // samples across semicircle (>=3)
    std::vector<float>& vertices, // OUT: interleaved floats (pos(3), normal(3), uv(2), color(3))
    std::vector<unsigned int>& indices) // OUT
{
    vertices.clear();
    indices.clear();

    if (controlPoints.size() < 2) return;
    if (arcSegments < 2) arcSegments = 2;

    // 1) sample Catmull-Rom path (same as your working code)
    std::vector<glm::vec3> path;
    for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
        for (unsigned int j = 0; j < pathResolution; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(pathResolution);

            glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
            glm::vec3 p1 = controlPoints[i];
            glm::vec3 p2 = controlPoints[i + 1];
            glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

            glm::vec3 p = catmullRom(p0, p1, p2, p3, t);
            path.push_back(p);
        }
    }
    path.push_back(controlPoints.back());

    const unsigned int ringVerts = arcSegments + 1; // vertices for half-circle
    const unsigned int railsPerRing = 4;            // left bottom/top, right bottom/top
    const unsigned int vertsPerRing = ringVerts + railsPerRing;

    // parameters for rails and colors
    const float railHeight = radius * 0.12f;
    const glm::vec3 baseColor(0.6f, 0.6f, 0.6f);
    const glm::vec3 railColor(0.9f, 0.9f, 0.9f);

    // --- Build Parallel-Transport frames along the sampled path ---
    size_t N = path.size();
    std::vector<glm::vec3> T(N), Nn(N), B(N);

    // initial tangent
    T[0] = glm::normalize(path[1] - path[0]);

    // pick an arbitrary stable initial normal (prefer world-up unless nearly parallel)
    glm::vec3 arbitrary = (fabs(T[0].y) < 0.9f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    Nn[0] = glm::normalize(glm::cross(arbitrary, T[0])); // lateral/right axis
    B[0]  = glm::normalize(glm::cross(T[0], Nn[0]));     // up/binormal

    for (size_t i = 1; i < N; ++i) {
        glm::vec3 t_prev = T[i - 1];
        glm::vec3 t = glm::normalize(path[i] - path[i - 1]);
        T[i] = t;

        glm::vec3 v = glm::cross(t_prev, t);
        float c = glm::dot(t_prev, t);

        if (glm::length(v) < 1e-6f) {
            // nearly colinear - reuse previous normal
            Nn[i] = Nn[i - 1];
        } else {
            float s = glm::length(v);
            glm::vec3 axis = v / s;
            float angle = atan2(s, c);

            // Rodrigues rotation matrix for rotating the previous lateral (Nn) by angle around axis
            glm::mat3 K(
                0.0f,      -axis.z,  axis.y,
                axis.z,     0.0f,   -axis.x,
                -axis.y,     axis.x,  0.0f
                );

            glm::mat3 R =
                glm::mat3(1.0f) * glm::cos(angle)
                + (1.0f - glm::cos(angle)) * glm::outerProduct(axis, axis)
                + glm::sin(angle) * K;

            Nn[i] = glm::normalize(R * Nn[i - 1]);
        }
        B[i] = glm::normalize(glm::cross(T[i], Nn[i]));
    }

    // 2) build ring vertices using PT frames (Nn = right/lateral, B = up)
    for (size_t i = 0; i < path.size(); ++i) {
        // per-sample v coordinate
        float v = static_cast<float>(i) / static_cast<float>(path.size() - 1);

        // glm::vec3 right = Nn[i]; // lateral axis
        // glm::vec3 up = B[i];     // local up/binormal

        glm::vec3 worldUp(0,1,0);

        // Project worldUp onto plane perpendicular to T[i]:
        glm::vec3 up = worldUp - glm::dot(worldUp, T[i]) * T[i];
        if (glm::length2(up) < 1e-6f)
            up = B[i];      // fallback when forward almost parallel to world-up
        else
            up = glm::normalize(up);

        // Right is orthogonal cross product
        glm::vec3 right = glm::normalize(glm::cross(T[i], up));






        // semicircle from -90° -> +90°
        for (unsigned int a = 0; a <= arcSegments; ++a) {
            float arcT = static_cast<float>(a) / static_cast<float>(arcSegments);
            float angle = -glm::half_pi<float>() + arcT * glm::pi<float>(); // -pi/2 -> +pi/2

            // local cross-section coords (x along right, y along up)
            float x = radius * std::sin(angle);                    // lateral offset along right
            float y = radius * (1.0f - std::cos(angle));           // vertical offset from rim downwards

            glm::vec3 pos = path[i] + right * x + up * y;

            // normal computed in world space from local cross-section normal (sin, cos)
            glm::vec3 localNormal = glm::normalize(glm::vec3(std::sin(angle), std::cos(angle), 0.0f));
            glm::vec3 normal = glm::normalize(right * localNormal.x + up * localNormal.y);

            float u = arcT;

            // push 11 floats: pos(3), normal(3), tex(2), color(3)
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            vertices.push_back(u);
            vertices.push_back(v);

            vertices.push_back(baseColor.r);
            vertices.push_back(baseColor.g);
            vertices.push_back(baseColor.b);
        }

        // add rails (left bottom, left top, right bottom, right top)
        glm::vec3 leftBottom = path[i] - right * radius;
        glm::vec3 rightBottom = path[i] + right * radius;
        glm::vec3 leftTop = leftBottom + up * railHeight;
        glm::vec3 rightTop = rightBottom + up * railHeight;

        // left bottom
        vertices.push_back(leftBottom.x);
        vertices.push_back(leftBottom.y);
        vertices.push_back(leftBottom.z);
        vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
        vertices.push_back(0.0f); vertices.push_back(v);
        vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

        // left top
        vertices.push_back(leftTop.x);
        vertices.push_back(leftTop.y);
        vertices.push_back(leftTop.z);
        vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
        vertices.push_back(0.0f); vertices.push_back(v);
        vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

        // right bottom
        vertices.push_back(rightBottom.x);
        vertices.push_back(rightBottom.y);
        vertices.push_back(rightBottom.z);
        vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
        vertices.push_back(1.0f); vertices.push_back(v);
        vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);

        // right top
        vertices.push_back(rightTop.x);
        vertices.push_back(rightTop.y);
        vertices.push_back(rightTop.z);
        vertices.push_back(up.x); vertices.push_back(up.y); vertices.push_back(up.z);
        vertices.push_back(1.0f); vertices.push_back(v);
        vertices.push_back(railColor.r); vertices.push_back(railColor.g); vertices.push_back(railColor.b);
    }

    // 3) indices: stitch rings -> quads (two triangles each)
    for (size_t i = 0; i < path.size() - 1; ++i) {
        unsigned int base = static_cast<unsigned int>(i * vertsPerRing);
        unsigned int next = static_cast<unsigned int>((i + 1) * vertsPerRing);

        // halfpipe surface (ringVerts vertices)
        for (unsigned int a = 0; a < ringVerts - 1; ++a) {
            unsigned int b0 = base + a;
            unsigned int b1 = base + a + 1;
            unsigned int n0 = next + a;
            unsigned int n1 = next + a + 1;

            indices.push_back(b0);
            indices.push_back(n0);
            indices.push_back(b1);

            indices.push_back(b1);
            indices.push_back(n0);
            indices.push_back(n1);
        }
    }
}


// std::vector<glm::vec3> generateSpiralControlPoints(
//     int numPoints,
//     float turns,            // how many full turns around
//     float radiusStart,
//     float radiusEnd,
//     float totalDrop,        // positive value (how much Y will decrease)
//     float verticalBumpAmp,  // amplitude of sine bumps (can be 0)
//     float verticalBumpFreq, // number of bumps per full turn (e.g. 4)
//     float jitterAmount,     // small random perturbation (world units)
//     unsigned int seed = 1337u)
// {
//     std::vector<glm::vec3> pts;
//     if (numPoints < 2) return pts;

//     std::mt19937 rng(seed);
//     std::uniform_real_distribution<float> rnd(-1.0f, 1.0f);

//     pts.reserve(numPoints);
//     for (int i = 0; i < numPoints; ++i) {
//         float t = float(i) / float(numPoints - 1); // 0..1 along path
//         float angle = t * turns * glm::two_pi<float>(); // radians
//         float radius = glm::mix(radiusStart, radiusEnd, t);

//         float x = radius * std::cos(angle);
//         float z = radius * std::sin(angle);

//         // vertical position = linear drop + sinusoidal bump
//         float y_linear = -t * totalDrop; // drop from 0 downwards
//         float y_bump = verticalBumpAmp * std::sin(angle * verticalBumpFreq);
//         float y = y_linear + y_bump;

//         // small random jitter in all axes
//         float jx = jitterAmount * rnd(rng);
//         float jy = jitterAmount * rnd(rng) * 0.5f; // less vertical jitter
//         float jz = jitterAmount * rnd(rng);

//         pts.emplace_back(x + jx, y + jy, z + jz);
//     }
//     return pts;
// }


// std::vector<glm::vec3> generateSpiralControlPoints(
//     int count,
//     float radiusStart,
//     float radiusEnd,
//     float heightDrop,
//     float turns,
//     float wobbleAmplitude,
//     float wobbleFrequency,
//     float verticalEase, // smooth descent curve: 0 = linear, 1 = eased
//     unsigned seed
//     ){
//     std::vector<glm::vec3> points;
//     points.reserve(count);

//     std::mt19937 rng(seed);
//     std::uniform_real_distribution<float> noise(-1.0f, 1.0f);

//     for(int i=0; i<count; ++i)
//     {
//         float t = (float)i / (float)(count-1);

//         // radius interpolation
//         float r = glm::mix(radiusStart, radiusEnd, t);

//         // smooth height drop (ease)
//         float h = -heightDrop * glm::pow(t, verticalEase);

//         // angle
//         float angle = turns * glm::two_pi<float>() * t;

//         // optional wobble (adds natural curvature)
//         // float wobble = wobbleAmplitude * std::sin(angle * wobbleFrequency + noise(rng)*0.1f);

//         float wobbleScale = glm::smoothstep(0.2f, 1.0f, t);
//         float wobble = wobbleAmplitude * wobbleScale
//                        * sin(angle * wobbleFrequency);

//         float x = (r + wobble) * std::cos(angle);
//         float z = (r + wobble) * std::sin(angle);
//         float y = h;

//         points.emplace_back(x, y, z);
//     }

//     return points;
// }


std::vector<glm::vec3> generateSpiralControlPoints(
    int count,
    float radiusStart,
    float radiusEnd,
    float heightDrop,
    float turns,
    float wobbleAmplitude,
    float wobbleFrequency,
    float verticalEase,
    unsigned seed
    ){
    std::vector<glm::vec3> points;
    points.reserve(count);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> noise(-1.0f, 1.0f);

    for(int i = 0; i < count; ++i)
    {
        float t = (float)i / (float)(count - 1);

        // -----------------------------
        // 1. Radius interpolation (eased)
        // -----------------------------
        // Before: r = mix(start, end, t)
        // Tightened too fast → curvature blowup at top.
        //
        // Now eased: slower shrink at top, smoother curvature
        float radiusEase = glm::smoothstep(0.0f, 1.0f, t); // smooth monotonic
        float r = glm::mix(radiusStart, radiusEnd, glm::pow(radiusEase, 0.7f));

        // -----------------------------
        // 2. Smooth vertical descent
        // -----------------------------
        // Before: pow(t, verticalEase) → too steep near the end.
        //
        // Now: Hermite smoothstep = 3t² − 2t³
        float th = t * t * (3.0f - 2.0f * t);
        float h = -heightDrop * glm::mix(t, th, verticalEase);

        // -----------------------------
        // 3. Angle (unchanged)
        // -----------------------------
        float angle = turns * glm::two_pi<float>() * t;

        // -----------------------------
        // 4. Wobble (fixed)
        // -----------------------------
        // Before: wobble strongest at top → frame twisting.
        //
        // Now:
        // - ramp up gently
        // - fade OUT near the top
        // - never destabilizes orientation
        float wobbleIn  = glm::smoothstep(0.10f, 0.40f, t);   // ramp in
        float wobbleOut = glm::smoothstep(1.00f, 0.80f, t);   // fade out at the top
        float wobbleScale = wobbleIn * wobbleOut;

        float wobble =
            wobbleAmplitude *
            wobbleScale *
            sin(angle * wobbleFrequency + noise(rng) * 0.05f);

        // -----------------------------
        // 5. Final coordinates
        // -----------------------------
        float x = (r + wobble) * cos(angle);
        float z = (r + wobble) * sin(angle);
        float y = h;

        points.emplace_back(x, y, z);
    }

    return points;
}
