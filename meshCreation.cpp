#include "meshCreation.h"
#include "processVertices.h"

Mesh createSphere(float radius, unsigned int sectorCount, unsigned int stackCount){

    Mesh mesh;

    std::vector<float> verts;

    std::vector<unsigned int> inds;

    createUVSphere(radius, sectorCount, stackCount, verts, inds);

    mesh.vertices = std::move(verts);
    mesh.indices = std::move(inds);

    GLsizei stride = 11 * sizeof(float);

    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},                   // POSITION
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},   // NORMAL
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},   // TEXCOORD
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)}    // COLOR
    };

    mesh.setup();

    return mesh;
}

Mesh createSkybox(){
    Mesh mesh;
    std::vector<float> verts;
    createUVSkybox(verts);

    mesh.vertices = std::move(verts);
    mesh.indices = {};  // Skybox drawn without index buffer

    GLsizei stride = 3 * sizeof(float);  // only positions
    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0}  // POSITION only
    };

    mesh.setup();
    return mesh;
}

Mesh createTrack(const std::vector<glm::vec3>& controllPoints, float width){

    Mesh mesh;

    std::vector<float> verts;

    std::vector<unsigned int> inds;

    createUVTrack(controllPoints,width,10,verts,inds);

    mesh.vertices = std::move(verts);
    mesh.indices = std::move(inds);

    GLsizei stride = 11 * sizeof(float);

    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},                   // POSITION
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},   // NORMAL
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},   // TEXCOORD
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)}    // COLOR
    };

    mesh.setup();

    return mesh;
}


// void createUVHalfpipe(
//     const HermiteTrack& track,
//     float radius,
//     unsigned int pathResolution,
//     unsigned int arcSegments,
//     std::vector<float>& vertices,
//     std::vector<unsigned int>& indices
//     )

Mesh createHalfpipe(const std::vector<glm::vec3>& controlPoints,float radius, unsigned int pathResolution, unsigned int arcSegments){
    Mesh mesh;

    std::vector<float> verts;

    std::vector<unsigned int> inds;


    createUVHalfpipeWithRails(controlPoints,
                              radius,     // radius
                              pathResolution,       // path resolution
                              arcSegments,        // semicircle segments
                             verts,
                              inds);
    // createUVHalfpipe(hermiteTrack,radius,pathResolution,arcSegments,verts,inds);


    mesh.vertices = std::move(verts);
    mesh.indices = std::move(inds);

    GLsizei stride = 11 * sizeof(float);

    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},                   // POSITION
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},   // NORMAL
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},   // TEXCOORD
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)}    // COLOR
    };

    mesh.setup();

    return mesh;
}


Mesh mergeMeshes(const std::vector<Mesh>& meshes)
{
    Mesh result;
    std::vector<float> verts;
    std::vector<unsigned int> inds;

    unsigned int indexOffset = 0;

    for (const Mesh& m : meshes)
    {
        // Append vertices
        verts.insert(verts.end(), m.vertices.begin(), m.vertices.end());

        // Append indices with offset
        for (unsigned int idx : m.indices)
        {
            inds.push_back(idx + indexOffset);
        }

        // Each vertex is 11 floats (pos, normal, uv, color)
        indexOffset += m.vertices.size() / 11;
    }

    result.vertices = std::move(verts);
    result.indices  = std::move(inds);

    GLsizei stride = 11 * sizeof(float);

    result.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)}
    };

    result.setup();
    return result;
}


// Mesh createLoopBoard(const std::vector<glm::vec3>& controlPoints, float width) {
//     Mesh mesh;
//     std::vector<float> vertices;
//     std::vector<unsigned int> indices;

//     if (controlPoints.size() < 2) return mesh;

//     float halfW = width * 0.5f;
//     glm::vec3 color(1.0f);

//     // Precompute tangent/right/up per control point
//     std::vector<glm::vec3> tangent(controlPoints.size());
//     std::vector<glm::vec3> right(controlPoints.size());
//     std::vector<glm::vec3> up(controlPoints.size());

//     for (size_t i = 0; i < controlPoints.size(); ++i) {
//         glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i-1];
//         glm::vec3 p1 = controlPoints[i];
//         glm::vec3 p2 = (i+1 < controlPoints.size()) ? controlPoints[i+1] : controlPoints[i];

//         tangent[i] = glm::normalize(p2 - p1);

//         // Choose stable up vector
//         glm::vec3 arbitraryUp = glm::vec3(0,1,0);
//         if (fabs(glm::dot(tangent[i], arbitraryUp)) > 0.99f)
//             arbitraryUp = glm::vec3(1,0,0);

//         right[i] = glm::normalize(glm::cross(arbitraryUp, tangent[i]));
//         up[i] = glm::normalize(glm::cross(tangent[i], right[i]));
//     }

//     auto pushVert = [&](const glm::vec3& pos, const glm::vec3& normal, float u, float v){
//         vertices.insert(vertices.end(), { pos.x, pos.y, pos.z,
//                                          normal.x, normal.y, normal.z,
//                                          u, v, color.r, color.g, color.b });
//     };

//     // Build vertices
//     for (size_t i = 0; i < controlPoints.size(); ++i) {
//         glm::vec3 center = controlPoints[i];
//         glm::vec3 r = right[i];
//         glm::vec3 u = up[i];

//         glm::vec3 leftEdge  = center - r * halfW;
//         glm::vec3 rightEdge = center + r * halfW;

//         float v = float(i) / float(controlPoints.size()-1);

//         pushVert(leftEdge, u, 0.0f, v);
//         pushVert(rightEdge, u, 1.0f, v);
//     }

//     // Build simple quad indices
//     for (size_t i = 0; i < controlPoints.size()-1; ++i) {
//         unsigned int a = i*2;
//         unsigned int b = a+2;

//         indices.push_back(a); indices.push_back(b); indices.push_back(a+1);
//         indices.push_back(a+1); indices.push_back(b); indices.push_back(b+1);
//     }

//     mesh.vertices = std::move(vertices);
//     mesh.indices = std::move(indices);

//     GLsizei stride = 11 * sizeof(float);
//     mesh.attributes = {
//                        {0,3,GL_FLOAT,GL_FALSE,stride,0},
//                        {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
//                        {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
//                        {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)},
//                        };
//     mesh.setup();
//     return mesh;
// }


Mesh createLoopBoard(const std::vector<glm::vec3>& controlPoints, float width, unsigned int pathResolution) {
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (controlPoints.size() < 2) return mesh;

    float halfW = width * 0.5f;
    glm::vec3 color(1.0f);

    // --- 1) Sample Catmull-Rom path ---
    std::vector<glm::vec3> path;
    for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
        for (unsigned int j = 0; j < pathResolution; ++j) {
            float t = float(j) / float(pathResolution);

            glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i-1];
            glm::vec3 p1 = controlPoints[i];
            glm::vec3 p2 = controlPoints[i+1];
            glm::vec3 p3 = (i+2 < controlPoints.size()) ? controlPoints[i+2] : controlPoints[i+1];

            path.push_back(catmullRom(p0, p1, p2, p3, t));
        }
    }
    path.push_back(controlPoints.back());

    size_t N = path.size();

    // --- 2) Build tangent/right/up frames ---
    std::vector<glm::vec3> tangent(N);
    std::vector<glm::vec3> right(N);
    std::vector<glm::vec3> up(N);

    for (size_t i = 0; i < N; ++i) {
        glm::vec3 pPrev = (i == 0) ? path[i] : path[i-1];
        glm::vec3 pNext = (i+1 < N) ? path[i+1] : path[i];

        tangent[i] = glm::normalize(pNext - pPrev);

        glm::vec3 arbitraryUp = glm::vec3(0,1,0);
        if (fabs(glm::dot(tangent[i], arbitraryUp)) > 0.99f)
            arbitraryUp = glm::vec3(1,0,0);

        right[i] = glm::normalize(glm::cross(arbitraryUp, tangent[i]));
        up[i] = glm::normalize(glm::cross(tangent[i], right[i]));
    }

    auto pushVert = [&](const glm::vec3& pos, const glm::vec3& normal, float u, float v) {
        vertices.insert(vertices.end(), { pos.x, pos.y, pos.z,
                                         normal.x, normal.y, normal.z,
                                         u, v, color.r, color.g, color.b });
    };

    // --- 3) Build vertices ---
    for (size_t i = 0; i < N; ++i) {
        glm::vec3 c = path[i];
        glm::vec3 r = right[i];
        glm::vec3 u = up[i];

        glm::vec3 leftEdge  = c - r * halfW;
        glm::vec3 rightEdge = c + r * halfW;

        float vCoord = float(i) / float(N-1);

        pushVert(leftEdge, u, 0.0f, vCoord);
        pushVert(rightEdge, u, 1.0f, vCoord);
    }

    // --- 4) Build indices ---
    for (size_t i = 0; i < N-1; ++i) {
        unsigned int a = i*2;
        unsigned int b = a+2;

        // top board
        indices.push_back(a+0); indices.push_back(b+0); indices.push_back(a+1);
        indices.push_back(a+1); indices.push_back(b+0); indices.push_back(b+1);

        // bottom board
        indices.push_back(a+6); indices.push_back(b+6); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+6); indices.push_back(b+7);

        // left side of board
        indices.push_back(a+0); indices.push_back(a+6); indices.push_back(b+0);
        indices.push_back(b+0); indices.push_back(a+6); indices.push_back(b+6);

        // right side of board
        indices.push_back(a+1); indices.push_back(b+1); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+1); indices.push_back(b+7);

        // left railing front
        indices.push_back(a+2); indices.push_back(b+2); indices.push_back(a+3);
        indices.push_back(a+3); indices.push_back(b+2); indices.push_back(b+3);

        // left railing back
        indices.push_back(a+8); indices.push_back(b+8); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+8); indices.push_back(b+9);

        // right railing front
        indices.push_back(a+4); indices.push_back(b+4); indices.push_back(a+5);
        indices.push_back(a+5); indices.push_back(b+4); indices.push_back(b+5);

        // rail left side connectors:
        indices.push_back(a+2); indices.push_back(a+8); indices.push_back(b+2);
        indices.push_back(b+2); indices.push_back(a+8); indices.push_back(b+8);

        indices.push_back(a+3); indices.push_back(b+3); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+3); indices.push_back(b+9);

        // rail right back:
        indices.push_back(a+10); indices.push_back(b+10); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+10); indices.push_back(b+11);

        // rail right side connectors:
        indices.push_back(a+4); indices.push_back(a+10); indices.push_back(b+4);
        indices.push_back(b+4); indices.push_back(a+10); indices.push_back(b+10);

        indices.push_back(a+5); indices.push_back(b+5); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+5); indices.push_back(b+11);
    }

    mesh.vertices = std::move(vertices);
    mesh.indices = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0,3,GL_FLOAT,GL_FALSE,stride,0},
                       {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
                       {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
                       {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}



Mesh createWall(float width, float height, float thickness)
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const float hw = width * 0.5f;
    const float hh = height;
    const float ht = thickness * 0.5f;

    const glm::vec3 color(0.9f, 0.1f, 0.1f);

    auto pushVert = [&](const glm::vec3& pos,
                        const glm::vec3& n,
                        float u, float v)
    {
        vertices.insert(vertices.end(), {
                                            pos.x, pos.y, pos.z,
                                            n.x, n.y, n.z,
                                            u, v,
                                            color.r, color.g, color.b
                                        });
    };

    // 8 corners (axis-aligned cube)
    glm::vec3 p[8] = {
        {-hw, 0,  -ht}, // 0
        { hw, 0,  -ht}, // 1
        { hw, hh, -ht}, // 2
        {-hw, hh, -ht}, // 3

        {-hw, 0,   ht}, // 4
        { hw, 0,   ht}, // 5
        { hw, hh,  ht}, // 6
        {-hw, hh,  ht}  // 7
    };

    // Push faces with normals

    // Front (+Z)
    for (int idx : {4,5,6,7}) pushVert(p[idx], glm::vec3(0,0,1), 0,0);
    // Back (-Z)
    for (int idx : {0,1,2,3}) pushVert(p[idx], glm::vec3(0,0,-1), 0,0);
    // Left (-X)
    for (int idx : {0,3,7,4}) pushVert(p[idx], glm::vec3(-1,0,0), 0,0);
    // Right (+X)
    for (int idx : {1,2,6,5}) pushVert(p[idx], glm::vec3(1,0,0), 0,0);
    // Top (+Y)
    for (int idx : {3,2,6,7}) pushVert(p[idx], glm::vec3(0,1,0), 0,0);
    // Bottom (-Y)
    for (int idx : {0,1,5,4}) pushVert(p[idx], glm::vec3(0,-1,0), 0,0);

    // Indices (6 faces × 2 triangles × 3 indices)
    for (int face = 0; face < 6; ++face) {
        unsigned int base = face * 4;
        indices.insert(indices.end(), {
                                          base+0, base+1, base+2,
                                          base+2, base+3, base+0
                                      });
    }

    // Setup mesh
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
                       {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
                       {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
                       {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}

Mesh createCylinder(float radius, float height, int segments)
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const glm::vec3 color(0.8f, 0.8f, 0.2f);
    float halfH = height * 0.5f;

    auto pushVert = [&](glm::vec3 pos, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(), {
                                            pos.x, pos.y, pos.z,
                                            n.x, n.y, n.z,
                                            u, v,
                                            color.r, color.g, color.b
                                        });
    };

    // ---- Side surface ----
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * glm::two_pi<float>();
        float x = cos(angle);
        float z = sin(angle);
        glm::vec3 normal(x, 0, z);

        glm::vec3 p1(radius * x, -halfH, radius * z);
        glm::vec3 p2(radius * x,  halfH, radius * z);

        pushVert(p1, normal, (float)i/segments, 0);
        pushVert(p2, normal, (float)i/segments, 1);
    }

    int baseSide = 0;

    for (int i = 0; i < segments; i++)
    {
        int a = baseSide + i * 2;
        int b = a + 1;
        int c = a + 2;
        int d = a + 3;

        indices.push_back(a); indices.push_back(b); indices.push_back(c);
        indices.push_back(b); indices.push_back(d); indices.push_back(c);
    }

    // ---- Top cap ----
    int topCenterIndex = vertices.size() / 11;
    pushVert({0, halfH, 0}, {0,1,0}, 0.5f, 0.5f);

    int baseTop = vertices.size() / 11;

    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * glm::two_pi<float>();
        float x = cos(angle);
        float z = sin(angle);
        glm::vec3 p(radius * x, halfH, radius * z);

        pushVert(p, {0,1,0}, (x+1)/2, (z+1)/2);
    }

    for (int i = 0; i < segments; i++)
    {
        indices.push_back(topCenterIndex);
        indices.push_back(baseTop + i + 1);
        indices.push_back(baseTop + i);
    }

    // ---- Bottom cap ----
    int bottomCenterIndex = vertices.size() / 11;
    pushVert({0, -halfH, 0}, {0,-1,0}, 0.5f, 0.5f);

    int baseBottom = vertices.size() / 11;

    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * glm::two_pi<float>();
        float x = cos(angle);
        float z = sin(angle);
        glm::vec3 p(radius * x, -halfH, radius * z);

        pushVert(p, {0,-1,0}, (x+1)/2, (z+1)/2);
    }

    for (int i = 0; i < segments; i++)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(baseBottom + i);
        indices.push_back(baseBottom + i + 1);
    }

    // Setup VAO
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
    };

    mesh.setup();
    return mesh;
}


Mesh createPyramid(float baseSize, float height)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const glm::vec3 color(0.9f, 0.6f, 0.1f);

    auto pushVert = [&](glm::vec3 pos, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(), {
                                            pos.x,pos.y,pos.z,
                                            n.x,n.y,n.z,
                                            u,v,
                                            color.r,color.g,color.b
                                        });
    };

    float h = height;
    float s = baseSize * 0.5f;

    glm::vec3 tip(0,h,0);

    glm::vec3 base[4] = {
        {-s,0,-s},
        { s,0,-s},
        { s,0, s},
        {-s,0, s}
    };

    // Base
    pushVert(base[0], {0,-1,0}, 0,0);
    pushVert(base[1], {0,-1,0}, 1,0);
    pushVert(base[2], {0,-1,0}, 1,1);
    pushVert(base[3], {0,-1,0}, 0,1);

    indices.insert(indices.end(), {0,1,2, 2,3,0});

    // 4 triangular faces
    for (int i = 0; i < 4; i++)
    {
        glm::vec3 a = base[i];
        glm::vec3 b = base[(i+1)%4];
        glm::vec3 c = tip;

        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));

        int baseIndex = vertices.size() / 11;

        pushVert(a,n,0,0);
        pushVert(b,n,1,0);
        pushVert(c,n,0.5,1);

        indices.push_back(baseIndex+0);
        indices.push_back(baseIndex+1);
        indices.push_back(baseIndex+2);
    }

    // Setup
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11*sizeof(float);
    mesh.attributes = {
        {0,3,GL_FLOAT,GL_FALSE,stride,0},
        {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
        {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
        {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)}
    };

    mesh.setup();
    return mesh;
}

Mesh createTriangularPrism(float width, float height, float depth)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const glm::vec3 color(0.2f, 0.8f, 0.3f);

    auto push = [&](glm::vec3 p, glm::vec3 n)
    {
        vertices.insert(vertices.end(),
                        {p.x,p.y,p.z, n.x,n.y,n.z, 0,0, color.r,color.g,color.b});
    };

    float w = width*0.5f;
    float d = depth*0.5f;
    float h = height;

    // 6 vertices
    glm::vec3 A(-w,0,-d);
    glm::vec3 B( w,0,-d);
    glm::vec3 C( 0,h,-d);

    glm::vec3 D(-w,0, d);
    glm::vec3 E( w,0, d);
    glm::vec3 F( 0,h, d);

    glm::vec3 pts[] = {A,B,C, D,E,F};
    glm::vec3 ns[] = { {0,0,-1}, {0,0,1}, {0,-1,0},
                      {1,0,0}, {-1,0,0} };

    // front triangle
    push(A,ns[0]); push(B,ns[0]); push(C,ns[0]);
    // back triangle
    push(D,ns[1]); push(E,ns[1]); push(F,ns[1]);

    // bottom quad
    push(A,ns[2]); push(B,ns[2]); push(E,ns[2]); push(D,ns[2]);

    // right quad
    push(B,ns[3]); push(C,ns[3]); push(F,ns[3]); push(E,ns[3]);

    // left quad
    push(A,ns[4]); push(D,ns[4]); push(F,ns[4]); push(C,ns[4]);

    // Indices
    auto quad = [&](int base)
    {
        indices.push_back(static_cast<unsigned int>(base));
        indices.push_back(static_cast<unsigned int>(base + 1));
        indices.push_back(static_cast<unsigned int>(base + 2));
        indices.push_back(static_cast<unsigned int>(base + 2));
        indices.push_back(static_cast<unsigned int>(base + 3));
        indices.push_back(static_cast<unsigned int>(base));
    };


    // Triangles
    indices.insert(indices.end(), {0,1,2});
    indices.insert(indices.end(), {3,4,5});

    // bottom
    quad(6);
    // right
    quad(10);
    // left
    quad(14);

    // Mesh setup
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11*sizeof(float);
    mesh.attributes = {
                       {0,3,GL_FLOAT,GL_FALSE,stride,0},
                       {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
                       {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
                       {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}






// meshCreation.h / .cpp (wherever you put it)

Mesh createFlatBoard(
    const std::vector<glm::vec3>& path,
    const std::vector<glm::vec3>& T,
    const std::vector<glm::vec3>& N,
    const std::vector<glm::vec3>& B,
    float width)
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const size_t Nsamples = path.size();
    if (Nsamples < 2 || T.size() != Nsamples || N.size() != Nsamples || B.size() != Nsamples)
        return mesh;

    const float halfW = width * 0.5f;

    float railingHeight   = 1.9f;
    float railingOffset   = 0.05f;
    float railingThickness= 0.04f;
    float boardThickness  = 0.05f;

    const glm::vec3 color(1.0f, 1.0f, 1.0f);

    auto pushVert = [&](const glm::vec3& pos,
                        const glm::vec3& n,
                        float u, float v)
    {
        vertices.insert(vertices.end(), {
                                            pos.x, pos.y, pos.z,
                                            n.x,   n.y,   n.z,
                                            u,     v,
                                            color.r, color.g, color.b
                                        });
    };

    // 1) Build vertices using given frames
    for (size_t i = 0; i < Nsamples; ++i) {
        glm::vec3 forward = T[i];
        glm::vec3 right   = N[i];
        glm::vec3 up      = B[i];

        glm::vec3 center   = path[i];
        glm::vec3 leftEdge = center - right * halfW;
        glm::vec3 rightEdge= center + right * halfW;

        float v = float(i) / float(Nsamples - 1);

        // top surface
        pushVert(leftEdge,  up, 0.0f, v);  // 0
        pushVert(rightEdge, up, 1.0f, v);  // 1

        // left railing front
        glm::vec3 leftRailBase = leftEdge - right * railingOffset;
        glm::vec3 rightRailBase= rightEdge + right * railingOffset;

        glm::vec3 leftRailTop  = leftRailBase  + up * railingHeight;
        glm::vec3 rightRailTop = rightRailBase + up * railingHeight;

        float uRailLeft  = -0.1f;
        float uRailRight = 1.1f;

        pushVert(leftRailBase, up, uRailLeft,  v); // 2
        pushVert(leftRailTop,  up, uRailLeft,  v); // 3

        // right railing front
        pushVert(rightRailBase, up, uRailRight, v); // 4
        pushVert(rightRailTop,  up, uRailRight, v); // 5

        // bottom surface
        glm::vec3 leftBottom  = leftEdge  - up * boardThickness;
        glm::vec3 rightBottom = rightEdge - up * boardThickness;

        pushVert(leftBottom,  -up, 0.0f, v); // 6
        pushVert(rightBottom, -up, 1.0f, v); // 7

        // left railing back (thickness)
        glm::vec3 leftRailBaseBack = leftRailBase  + right * railingThickness;
        glm::vec3 leftRailTopBack  = leftRailTop   + right * railingThickness;

        // right railing back (thickness)
        glm::vec3 rightRailBaseBack= rightRailBase - right * railingThickness;
        glm::vec3 rightRailTopBack = rightRailTop  - right * railingThickness;

        pushVert(leftRailBaseBack,  up, uRailLeft,  v); // 8
        pushVert(leftRailTopBack,   up, uRailLeft,  v); // 9

        pushVert(rightRailBaseBack, up, uRailRight, v); // 10
        pushVert(rightRailTopBack,  up, uRailRight, v); // 11
    }

    // 2) indices
    const unsigned int vertsPer = 12;

    for (size_t i = 0; i < Nsamples - 1; ++i) {
        unsigned int a = i * vertsPer;
        unsigned int b = (i + 1) * vertsPer;

        // top board
        indices.push_back(a+0); indices.push_back(b+0); indices.push_back(a+1);
        indices.push_back(a+1); indices.push_back(b+0); indices.push_back(b+1);

        // bottom board
        indices.push_back(a+6); indices.push_back(b+6); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+6); indices.push_back(b+7);

        // left side of board
        indices.push_back(a+0); indices.push_back(a+6); indices.push_back(b+0);
        indices.push_back(b+0); indices.push_back(a+6); indices.push_back(b+6);

        // right side of board
        indices.push_back(a+1); indices.push_back(b+1); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+1); indices.push_back(b+7);

        // left railing front
        indices.push_back(a+2); indices.push_back(b+2); indices.push_back(a+3);
        indices.push_back(a+3); indices.push_back(b+2); indices.push_back(b+3);

        // left railing back
        indices.push_back(a+8); indices.push_back(b+8); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+8); indices.push_back(b+9);

        // right railing front
        indices.push_back(a+4); indices.push_back(b+4); indices.push_back(a+5);
        indices.push_back(a+5); indices.push_back(b+4); indices.push_back(b+5);

        // rail left side connectors:
        indices.push_back(a+2); indices.push_back(a+8); indices.push_back(b+2);
        indices.push_back(b+2); indices.push_back(a+8); indices.push_back(b+8);

        indices.push_back(a+3); indices.push_back(b+3); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+3); indices.push_back(b+9);

        // rail right back:
        indices.push_back(a+10); indices.push_back(b+10); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+10); indices.push_back(b+11);

        // rail right side connectors:
        indices.push_back(a+4); indices.push_back(a+10); indices.push_back(b+4);
        indices.push_back(b+4); indices.push_back(a+10); indices.push_back(b+10);

        indices.push_back(a+5); indices.push_back(b+5); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+5); indices.push_back(b+11);
    }

    // 3) setup mesh
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
                       {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
                       {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
                       {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}


Mesh createFlatBoardLocal(
    const std::vector<glm::vec3>& path,
    const std::vector<glm::vec3>& T,
    const std::vector<glm::vec3>& N,
    const std::vector<glm::vec3>& B,
    float width)
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const size_t Nsamples = path.size();
    if (Nsamples < 2) return mesh;

    // All geometry in LOCAL space
    const float halfW = width * 0.5f;

    const float boardThickness = 0.05f;
    const float railHeight     = 0.30f;
    const float railOffset     = 0.05f;
    const float railThickness  = 0.02f;

    auto pushVert = [&](glm::vec3 localP, glm::vec3 localN, float u, float v)
    {
        // localP is (x,y,z) in local frame
        vertices.insert(vertices.end(), {
                                            localP.x, localP.y, localP.z,
                                            localN.x, localN.y, localN.z,
                                            u, v,
                                            1.0f,1.0f,1.0f // color
                                        });
    };

    // -------------------------
    // Build per-sample mesh
    // -------------------------
    for (size_t i=0; i<Nsamples; ++i)
    {
        float v = float(i) / float(Nsamples - 1);

        glm::vec3 right = N[i];
        glm::vec3 up    = B[i];

        // LOCAL space edges
        glm::vec3 leftEdge  = -right * halfW;
        glm::vec3 rightEdge =  right * halfW;

        // top surface
        pushVert(leftEdge,  up, 0.0f, v);
        pushVert(rightEdge, up, 1.0f, v);

        // bottom surface
        pushVert(leftEdge - up*boardThickness,  -up, 0.0f, v);
        pushVert(rightEdge - up*boardThickness, -up, 1.0f, v);

        // rail bases (symmetric)
        glm::vec3 leftRailB  = leftEdge  - right*railOffset;
        glm::vec3 rightRailB = rightEdge + right*railOffset;

        glm::vec3 leftRailT  = leftRailB  + up * railHeight;
        glm::vec3 rightRailT = rightRailB + up * railHeight;

        // push rails
        pushVert(leftRailB,  up, -0.1f, v);
        pushVert(leftRailT,  up, -0.1f, v);

        pushVert(rightRailB, up, 1.1f, v);
        pushVert(rightRailT, up, 1.1f, v);

        // thick back rails
        pushVert(leftRailB  + right*railThickness, up, -0.1f, v);
        pushVert(leftRailT  + right*railThickness, up, -0.1f, v);

        pushVert(rightRailB - right*railThickness, up, 1.1f, v);
        pushVert(rightRailT - right*railThickness, up, 1.1f, v);
    }

    // indices identical to your previous version (same topology)
    // but using local vertices only
    const unsigned int stride = 12;
    for (unsigned int i=0; i < Nsamples-1; ++i) {
        unsigned int a = i * stride;
        unsigned int b = (i+1) * stride;

        // top board
        indices.push_back(a+0); indices.push_back(b+0); indices.push_back(a+1);
        indices.push_back(a+1); indices.push_back(b+0); indices.push_back(b+1);

        // bottom board
        indices.push_back(a+6); indices.push_back(b+6); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+6); indices.push_back(b+7);

        // left side of board
        indices.push_back(a+0); indices.push_back(a+6); indices.push_back(b+0);
        indices.push_back(b+0); indices.push_back(a+6); indices.push_back(b+6);

        // right side of board
        indices.push_back(a+1); indices.push_back(b+1); indices.push_back(a+7);
        indices.push_back(a+7); indices.push_back(b+1); indices.push_back(b+7);

        // left railing front
        indices.push_back(a+2); indices.push_back(b+2); indices.push_back(a+3);
        indices.push_back(a+3); indices.push_back(b+2); indices.push_back(b+3);

        // left railing back
        indices.push_back(a+8); indices.push_back(b+8); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+8); indices.push_back(b+9);

        // right railing front
        indices.push_back(a+4); indices.push_back(b+4); indices.push_back(a+5);
        indices.push_back(a+5); indices.push_back(b+4); indices.push_back(b+5);

        // rail left side connectors:
        indices.push_back(a+2); indices.push_back(a+8); indices.push_back(b+2);
        indices.push_back(b+2); indices.push_back(a+8); indices.push_back(b+8);

        indices.push_back(a+3); indices.push_back(b+3); indices.push_back(a+9);
        indices.push_back(a+9); indices.push_back(b+3); indices.push_back(b+9);

        // rail right back:
        indices.push_back(a+10); indices.push_back(b+10); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+10); indices.push_back(b+11);

        // rail right side connectors:
        indices.push_back(a+4); indices.push_back(a+10); indices.push_back(b+4);
        indices.push_back(b+4); indices.push_back(a+10); indices.push_back(b+10);

        indices.push_back(a+5); indices.push_back(b+5); indices.push_back(a+11);
        indices.push_back(a+11); indices.push_back(b+5); indices.push_back(b+11);

    }

    mesh.vertices = vertices;
    mesh.indices  = indices;

    GLsizei strideBytes = 11*sizeof(float);
    mesh.attributes = {
                       {0,3,GL_FLOAT,GL_FALSE,strideBytes,0},
                       {1,3,GL_FLOAT,GL_FALSE,strideBytes,3*sizeof(float)},
                       {2,2,GL_FLOAT,GL_FALSE,strideBytes,6*sizeof(float)},
                       {3,3,GL_FLOAT,GL_FALSE,strideBytes,8*sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}


Mesh createDebugPathLine(const std::vector<glm::vec3>& path,
                         const glm::vec3& color)
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    if (path.size() < 2)
        return mesh;

    // position (3) + dummy normal (3) + uv (2) + color (3) = 11 floats
    for (size_t i = 0; i < path.size(); ++i) {
        const glm::vec3& p = path[i];

        vertices.insert(vertices.end(), {
                                            p.x, p.y, p.z,         // position
                                            0.0f, 0.0f, 1.0f,      // dummy normal
                                            0.0f, 0.0f,            // dummy uv
                                            color.r, color.g, color.b // color
                                        });

        indices.push_back(static_cast<unsigned int>(i));
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
                       {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
                       {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
                       {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)},
                       };

    mesh.primitiveType = GL_LINES;
    mesh.setup();
    return mesh;
}


Mesh createDebugFrames(const std::vector<glm::vec3>& path,
                       const std::vector<glm::vec3>& T,
                       const std::vector<glm::vec3>& N,
                       const std::vector<glm::vec3>& B,
                       float axisLength = 1.0f,
                       int   step = 5)      // draw every 5th sample
{
    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    size_t Nsamples = path.size();
    if (Nsamples == 0 || T.size() != Nsamples ||
        N.size() != Nsamples || B.size() != Nsamples)
        return mesh;

    auto addSegment = [&](const glm::vec3& a,
                          const glm::vec3& b,
                          const glm::vec3& color)
    {
        unsigned int idx = static_cast<unsigned int>(vertices.size() / 11);

        // endpoint A
        vertices.insert(vertices.end(), {
                                            a.x, a.y, a.z,
                                            0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f,
                                            color.r, color.g, color.b
                                        });
        // endpoint B
        vertices.insert(vertices.end(), {
                                            b.x, b.y, b.z,
                                            0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f,
                                            color.r, color.g, color.b
                                        });

        indices.push_back(idx);
        indices.push_back(idx + 1);
    };

    for (size_t i = 0; i < Nsamples; i += step) {
        const glm::vec3& c  = path[i];

        glm::vec3 pT = c + T[i] * axisLength;
        glm::vec3 pN = c + N[i] * axisLength;
        glm::vec3 pB = c + B[i] * axisLength;

        addSegment(c, pT, glm::vec3(1, 1, 1)); // T = white
        addSegment(c, pN, glm::vec3(0, 1, 0)); // N = green
        addSegment(c, pB, glm::vec3(0, 0, 1)); // B = blue
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
                       {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
                       {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
                       {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)},
                       };

    mesh.primitiveType = GL_LINES;
    mesh.setup();
    return mesh;
}


Mesh createDebugBulletMesh(const std::vector<DebugLine>& lines)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned> indices;

    unsigned idx = 0;

    for (auto& L : lines)
    {
        auto add = [&](glm::vec3 p, glm::vec3 c) {
            vertices.insert(vertices.end(), {
                                                p.x, p.y, p.z,
                                                0,0,1,
                                                0,0,
                                                c.r, c.g, c.b
                                            });
            indices.push_back(idx++);
        };

        add(L.from, L.color);
        add(L.to,   L.color);
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);
    mesh.primitiveType = GL_LINES;

    GLsizei stride = 11*sizeof(float);
    mesh.attributes = {
                       {0,3,GL_FLOAT,GL_FALSE,stride,0},
                       {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
                       {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
                       {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}





// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& controlPoints,
//     float width,
//     unsigned int pathResolution)
// {
//     Mesh mesh;
//     std::vector<float> vertices;
//     std::vector<unsigned int> indices;

//     if (controlPoints.size() < 2) return mesh;

//     const float halfW = width * 0.5f;

//     float railingHeight = 0.3f;   // how tall
//     float railingOffset = 0.05f;  // how far outward from the board edge
//     float railingThickness = 0.02f; // thickness (flat bar)

//     // -------------------------------------------------------------------
//     // 1) Sample Catmull-Rom path EXACTLY as in your original version
//     // -------------------------------------------------------------------
//     std::vector<glm::vec3> path;
//     for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for (unsigned int j = 0; j < pathResolution; ++j) {
//             float t = float(j) / float(pathResolution);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i + 1];
//             glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

//             path.push_back(catmullRom(p0, p1, p2, p3, t));
//         }
//     }
//     path.push_back(controlPoints.back());

//     const size_t N = path.size();
//     const glm::vec3 color(1.0f, 1.0f, 1.0f);

//     // -------------------------------------------------------------------
//     // 2) Build Parallel Transport Frames for each sample
//     // -------------------------------------------------------------------
//     std::vector<glm::vec3> T(N);  // tangent
//     std::vector<glm::vec3> Nn(N); // normal
//     std::vector<glm::vec3> B(N);  // binormal

//     // --- Initial tangent ---
//     T[0] = glm::normalize(path[1] - path[0]);

//     // --- Choose stable initial normal ---
//     // glm::vec3 arbitrary =
//     //     (fabs(T[0].y) < 0.9f ? glm::vec3(0,1,0) : glm::vec3(1,0,0));

//     // Nn[0] = glm::normalize(glm::cross(arbitrary, T[0]));
//     // B[0]  = glm::cross(T[0], Nn[0]);


//     // Always use worldUp to stabilize the board around curves
//     // glm::vec3 worldUp(0,1,0);

//     // // Ensure worldUp is not parallel to the tangent
//     // glm::vec3 right0 = glm::cross(worldUp, T[0]);
//     // if (glm::length(right0) < 1e-6f)
//     //     right0 = glm::cross(glm::vec3(1,0,0), T[0]);

//     // right0 = glm::normalize(right0);
//     // glm::vec3 up0 = glm::normalize(glm::cross(T[0], right0));

//     // Nn[0] = right0;  // right
//     // B[0]  = up0;     // normal


//     glm::vec3 arbitraryUp(0,1,0);

//     // If tangent is too vertical, choose another reference up-axis
//     if (fabs(glm::dot(T[0], arbitraryUp)) > 0.9f)
//         arbitraryUp = glm::vec3(1,0,0);

//     // Build a *perfectly orthogonal* starting frame
//     glm::vec3 right0 = glm::normalize(glm::cross(arbitraryUp, T[0]));
//     glm::vec3 up0    = glm::normalize(glm::cross(T[0], right0));

//     Nn[0] = right0;
//     B[0]  = up0;



//     for (size_t i = 1; i < N; i++) {
//         glm::vec3 t_prev = T[i-1];
//         glm::vec3 t = glm::normalize(path[i] - path[i-1]);

//         T[i] = t;

//         glm::vec3 v = glm::cross(t_prev, t);
//         float c = glm::dot(t_prev, t);

//         if (glm::length(v) < 1e-6f) {
//             // No rotation needed
//             Nn[i] = Nn[i-1];
//         } else {
//             float s = glm::length(v);
//             glm::vec3 axis = v / s;
//             float angle = atan2(s, c);

//             glm::mat3 K(
//                 0,      -axis.z,  axis.y,
//                 axis.z,  0,      -axis.x,
//                 -axis.y,  axis.x,  0
//                 );

//             glm::mat3 R =
//                 glm::mat3(1.0f) * glm::cos(angle)
//                 + (1 - glm::cos(angle)) * glm::outerProduct(axis, axis)
//                 + glm::sin(angle) * K;

//             Nn[i] = glm::normalize(R * Nn[i-1]);
//         }

//         B[i] = glm::cross(T[i], Nn[i]);
//     }

//     // -------------------------------------------------------------------
//     // 3) Create vertices (left/right positions)
//     // -------------------------------------------------------------------
//     auto pushVert = [&](const glm::vec3& pos,
//                         const glm::vec3& n,
//                         float u, float v)
//     {
//         vertices.insert(vertices.end(), {
//                                             pos.x, pos.y, pos.z,
//                                             n.x, n.y, n.z,
//                                             u, v,
//                                             color.r, color.g, color.b
//                                         });
//     };

//     for (size_t i = 0; i < N; ++i) {
//         // glm::vec3 right = Nn[i];
//         // glm::vec3 normal = B[i];


//         glm::vec3 forward = T[i];
//         glm::vec3 worldUp(0,1,0);

//         // force a stable up vector
//         glm::vec3 up = worldUp - glm::dot(worldUp, forward) * forward;
//         if (glm::length2(up) < 1e-6f)
//             up = B[i];       // fallback (rare)
//         else
//             up = glm::normalize(up);

//         // right = horizontal axis
//         glm::vec3 right = glm::normalize(glm::cross(forward, up));

//         // normal = up
//         glm::vec3 normal = up;

//         // glm::vec3 tangent = T[i];
//         // glm::vec3 up = glm::vec3(0,1,0);
//         // glm::vec3 right = glm::normalize(glm::cross(up, tangent));
//         // glm::vec3 normal = glm::normalize(glm::cross(tangent, right));


//         glm::vec3 center = path[i];
//         glm::vec3 leftEdge  = center - right * halfW;
//         glm::vec3 rightEdge = center + right * halfW;

//         float v = float(i) / float(N - 1);

//         pushVert(leftEdge,normal,0.0f,v);
//         pushVert(rightEdge,normal,1.0f,v);

//         glm::vec3 leftRailBase = leftEdge - right * railingOffset;
//         glm::vec3 rightRailBase = rightEdge + right * railingOffset;

//         glm::vec3 leftRailTop = leftRailBase + normal * railingHeight;
//         glm::vec3 rightRailTop = rightRailBase + normal * railingHeight;

//         float uRailLeft = -0.1f;
//         float uRailRight = 1.1f;

//         pushVert(leftRailBase,normal,uRailLeft,v);
//         pushVert(leftRailTop,normal,uRailLeft,v);

//         pushVert(rightRailBase,normal,uRailRight,v);
//         pushVert(rightRailTop,normal,uRailRight,v);


//         float boardThickness = 0.05f;

//         glm::vec3 leftBottom = leftEdge - normal * boardThickness;
//         glm::vec3 rightBottom = rightEdge - normal * boardThickness;

//         pushVert(leftBottom,-normal,0.0f,v);
//         pushVert(rightBottom,-normal,1.0f,v);

//         glm::vec3 leftRailBase_back = leftRailBase + right * railingThickness;
//         glm::vec3 leftRailTop_back = leftRailTop + right * railingThickness;

//         glm::vec3 rightRailBase_back = rightRailBase - right * railingThickness;
//         glm::vec3 rightRailTop_back = rightRailTop - right * railingThickness;



//         pushVert(leftRailBase_back,normal,uRailLeft,v);
//         pushVert(leftRailTop_back,normal,uRailLeft,v);

//         pushVert(rightRailBase_back,normal,uRailRight,v);
//         pushVert(rightRailTop_back,normal,uRailRight,v);
//     }

//     // -------------------------------------------------------------------
//     // 4) Indices
//     // -------------------------------------------------------------------
//     const unsigned int vertsPer = 12;

//     for (size_t i = 0; i < N - 1; ++i) {

//         unsigned int a = i*vertsPer;
//         unsigned int b = (i+1) * vertsPer;

//         indices.push_back(a+0); indices.push_back(b+0); indices.push_back(a+1);
//         indices.push_back(a+1); indices.push_back(b+0); indices.push_back(b+1);
//         indices.push_back(a+6); indices.push_back(b+6); indices.push_back(a+7);
//         indices.push_back(a+7); indices.push_back(b+6); indices.push_back(b+7);

//         indices.push_back(a+0); indices.push_back(a+6); indices.push_back(b+0);
//         indices.push_back(b+0); indices.push_back(a+6); indices.push_back(b+6);

//         indices.push_back(a+1); indices.push_back(b+1); indices.push_back(a+7);
//         indices.push_back(a+7); indices.push_back(b+1); indices.push_back(b+7);


//         indices.push_back(a+2); indices.push_back(b+2); indices.push_back(a+3);
//         indices.push_back(a+3); indices.push_back(b+2); indices.push_back(b+3);
//         indices.push_back(a+8); indices.push_back(b+8); indices.push_back(a+9);
//         indices.push_back(a+9); indices.push_back(b+8); indices.push_back(b+9);



//         indices.push_back(a+4); indices.push_back(b+4); indices.push_back(a+5);
//         indices.push_back(a+5); indices.push_back(b+4); indices.push_back(b+5);


//         // Rail left side connectors:
//         indices.push_back(a+2); indices.push_back(a+8); indices.push_back(b+2);
//         indices.push_back(b+2); indices.push_back(a+8); indices.push_back(b+8);

//         indices.push_back(a+3); indices.push_back(b+3); indices.push_back(a+9);
//         indices.push_back(a+9); indices.push_back(b+3); indices.push_back(b+9);

//         // Rail right back:
//         indices.push_back(a+10); indices.push_back(b+10); indices.push_back(a+11);
//         indices.push_back(a+11); indices.push_back(b+10); indices.push_back(b+11);

//         // Rail right side connectors:
//         indices.push_back(a+4); indices.push_back(a+10); indices.push_back(b+4);
//         indices.push_back(b+4); indices.push_back(a+10); indices.push_back(b+10);

//         indices.push_back(a+5); indices.push_back(b+5); indices.push_back(a+11);
//         indices.push_back(a+11); indices.push_back(b+5); indices.push_back(b+11);
//     }

//     // -------------------------------------------------------------------
//     // 5) Setup mesh
//     // -------------------------------------------------------------------
//     mesh.vertices = std::move(vertices);
//     mesh.indices  = std::move(indices);

//     GLsizei stride = 11 * sizeof(float);
//     mesh.attributes = {
//                        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
//                        {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
//                        {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
//                        {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)},
//                        };

//     mesh.setup();
//     return mesh;
// }


// Create a mesh that morphs from a flat board (t=0) to a half-pipe (t=1)
// controlPoints : input control points (Catmull-Rom sampled)
// width         : full board width (same convention as createFlatBoard)
// pathResolution: number of samples per control segment (same convention)
// arcSegments   : semicircle subdivisions for the half-pipe cross section
// curveToPipeWeight : shaping exponent for the morph (1.0 = linear smoothstep)
Mesh createMorphingHalfpipe(
    const std::vector<glm::vec3>& controlPoints,
    float width,
    unsigned int pathResolution,
    unsigned int arcSegments = 16,
    float curveToPipeWeight = 1.0f)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (controlPoints.size() < 2) return mesh;
    if (arcSegments < 2) arcSegments = 2;
    if (pathResolution == 0) pathResolution = 10;

    // we'll interpret 'width' as the full board width (same as createFlatBoard).
    // pipeRadius is half that width (so a flat board with width W will morph into a pipe radius = W/2).
    // If you want width to be pipe radius instead, pass width*2 or tweak here.
    const float halfW = width * 0.5f;
    const float pipeRadius = halfW;

    // -------------------------------------------------------------------
    // 1) Sample Catmull-Rom path (same sampler you used elsewhere)
    // -------------------------------------------------------------------
    std::vector<glm::vec3> path;
    for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
        for (unsigned int j = 0; j < pathResolution; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(pathResolution);

            glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
            glm::vec3 p1 = controlPoints[i];
            glm::vec3 p2 = controlPoints[i + 1];
            glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

            path.push_back(catmullRom(p0, p1, p2, p3, t));
        }
    }
    path.push_back(controlPoints.back());

    const size_t N = path.size();
    const glm::vec3 baseColor(0.6f, 0.6f, 0.6f);
    const glm::vec3 railColor(0.9f, 0.9f, 0.9f);

    // parameters for rails and board thickness (tweak to taste)
    const float railHeight = pipeRadius * 0.12f;
    const float railingOffset = 0.05f * width;    // small offset relative to width
    const float boardThickness = 0.05f;          // constant thickness
    const float railingThickness = 0.02f;

    // ring layout
    const unsigned int ringVerts = arcSegments + 1; // semicircle samples
    const unsigned int railsPerRing = 4;
    const unsigned int vertsPerRing = ringVerts + railsPerRing;

    // -------------------------------------------------------------------
    // 2) Build stable frames (parallel-transport style with worldUp fallback)
    // -------------------------------------------------------------------
    std::vector<glm::vec3> T(N), Nn(N), B(N);
    T[0] = glm::normalize(path[1] - path[0]);
    glm::vec3 arbitrary = (fabs(T[0].y) < 0.9f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    Nn[0] = glm::normalize(glm::cross(arbitrary, T[0])); // lateral/right
    B[0]  = glm::normalize(glm::cross(T[0], Nn[0]));     // up/binormal

    for (size_t i = 1; i < N; ++i) {
        glm::vec3 t_prev = T[i-1];
        glm::vec3 t = glm::normalize(path[i] - path[i-1]);
        T[i] = t;

        glm::vec3 v = glm::cross(t_prev, t);
        float c = glm::dot(t_prev, t);

        if (glm::length(v) < 1e-6f) {
            Nn[i] = Nn[i-1];
        } else {
            float s = glm::length(v);
            glm::vec3 axis = v / s;
            float angle = atan2(s, c);

            glm::mat3 K(
                0.0f,      -axis.z,  axis.y,
                axis.z,     0.0f,   -axis.x,
                -axis.y,     axis.x,  0.0f
                );

            glm::mat3 R =
                glm::mat3(1.0f) * glm::cos(angle)
                + (1.0f - glm::cos(angle)) * glm::outerProduct(axis, axis)
                + glm::sin(angle) * K;

            Nn[i] = glm::normalize(R * Nn[i-1]);
        }

        B[i] = glm::normalize(glm::cross(T[i], Nn[i]));
    }

    // -------------------------------------------------------------------
    // 3) Build vertices: for each sample we create ringVerts samples across
    //    the semicircle, but each sample is morphed between a flat profile
    //    and the pipe cross-section using an eased morph factor.
    // -------------------------------------------------------------------
    auto pushVert = [&](const glm::vec3& pos, const glm::vec3& n, float u, float v, const glm::vec3& col) {
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);

        vertices.push_back(n.x);
        vertices.push_back(n.y);
        vertices.push_back(n.z);

        vertices.push_back(u);
        vertices.push_back(v);

        vertices.push_back(col.x);
        vertices.push_back(col.y);
        vertices.push_back(col.z);
    };

    for (size_t i = 0; i < N; ++i) {
        float pathT = static_cast<float>(i) / static_cast<float>(N - 1);

        // morph factor: 0 => pure flat, 1 => full halfpipe
        // apply shaping exponent then smoothstep for nicer ease
        float shaped = glm::pow(pathT, curveToPipeWeight);
        float morph = glm::smoothstep(0.0f, 1.0f, shaped);

        // local frame at sample
        glm::vec3 forward = T[i];
        // try project world up first (keeps top looking consistent)
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        glm::vec3 up = worldUp - glm::dot(worldUp, forward) * forward;
        if (glm::length2(up) < 1e-6f) up = B[i];
        else up = glm::normalize(up);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        float v = pathT;

        // for arc samples, map arc angle -> flat lateral x
        for (unsigned int a = 0; a <= arcSegments; ++a) {
            float arcT = static_cast<float>(a) / static_cast<float>(arcSegments);
            float angle = -glm::half_pi<float>() + arcT * glm::pi<float>(); // -pi/2 -> +pi/2

            // pipe cross-section coordinates (x along right, y along up)
            float x_pipe = pipeRadius * std::sin(angle);                      // lateral offset
            float y_pipe = pipeRadius * (1.0f - std::cos(angle));             // vertical offset from rim downward

            glm::vec3 pos_pipe = path[i] + right * x_pipe + up * y_pipe;

            // flat profile coords: x mapped linearly across width, y = 0
            float x_flat = glm::mix(-halfW, halfW, arcT);
            glm::vec3 pos_flat = path[i] + right * x_flat; // y = 0

            // blended position
            glm::vec3 pos = glm::mix(pos_flat, pos_pipe, morph);

            // compute normals: blend flat normal (up) and pipe normal
            glm::vec3 normal_pipe_local = glm::normalize(glm::vec3(std::sin(angle), std::cos(angle), 0.0f));
            glm::vec3 normal_pipe = glm::normalize(right * normal_pipe_local.x + up * normal_pipe_local.y);
            glm::vec3 normal_flat = up;
            glm::vec3 normal = glm::normalize(glm::mix(normal_flat, normal_pipe, morph));

            float u = arcT;

            pushVert(pos, normal, u, v, baseColor);
        }

        // add rails (left bottom, left top, right bottom, right top) — their height also morphs
        // rail horizontal offset from edge: use edge corresponding to arcT 0 and 1
        glm::vec3 leftEdge = path[i] - right * halfW;
        glm::vec3 rightEdge = path[i] + right * halfW;

        // rail bases move slightly outward as pipe forms; offset scales with morph
        glm::vec3 leftRailBase = leftEdge - right * (railingOffset * (1.0f - morph)) - right * (railingOffset * morph);
        glm::vec3 rightRailBase = rightEdge + right * (railingOffset * (1.0f - morph)) + right * (railingOffset * morph);

        // rail top height scales with morph (so it rises as pipe forms)
        float railH = glm::mix(railingOffset * 0.5f, railHeight, morph);

        glm::vec3 leftRailTop = leftRailBase + up * railH;
        glm::vec3 rightRailTop = rightRailBase + up * railH;

        // left bottom
        pushVert(leftRailBase, up, 0.0f, v, railColor);
        // left top
        pushVert(leftRailTop, up, 0.0f, v, railColor);
        // right bottom
        pushVert(rightRailBase, up, 1.0f, v, railColor);
        // right top
        pushVert(rightRailTop, up, 1.0f, v, railColor);
    }

    // -------------------------------------------------------------------
    // 4) Indices: stitch rings -> quads (two tris each)
    //    each ring has ringVerts + 4 rail verts (vertCountPerRing)
    // -------------------------------------------------------------------
    for (size_t i = 0; i < N - 1; ++i) {
        unsigned int base = static_cast<unsigned int>(i * vertsPerRing);
        unsigned int next = static_cast<unsigned int>((i + 1) * vertsPerRing);

        // halfpipe surface (ringVerts entries)
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

        // rails (optional quads). Layout:
        // L0 = base + ringVerts, L1 = base + ringVerts + 1
        // R0 = base + ringVerts + 2, R1 = base + ringVerts + 3
        unsigned int L0 = base + ringVerts;
        unsigned int L1 = base + ringVerts + 1;
        unsigned int R0 = base + ringVerts + 2;
        unsigned int R1 = base + ringVerts + 3;

        unsigned int NL0 = next + ringVerts;
        unsigned int NL1 = next + ringVerts + 1;
        unsigned int NR0 = next + ringVerts + 2;
        unsigned int NR1 = next + ringVerts + 3;

        // left rail quad -> two tris
        indices.push_back(L0); indices.push_back(NL0); indices.push_back(L1);
        indices.push_back(L1); indices.push_back(NL0); indices.push_back(NL1);

        // right rail quad -> two tris
        indices.push_back(R0); indices.push_back(NR0); indices.push_back(R1);
        indices.push_back(R1); indices.push_back(NR0); indices.push_back(NR1);
    }

    // -------------------------------------------------------------------
    // 5) Setup mesh (same vertex layout as others)
    // -------------------------------------------------------------------
    mesh.vertices = std::move(vertices);
    mesh.indices = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float)},
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6 * sizeof(float)},
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8 * sizeof(float)}
    };

    mesh.setup();
    return mesh;
}




// Mesh createBlendSegment(
//     const TrackPiece& fromPiece, // piece we’re leaving
//     const TrackPiece& toPiece,   // piece we’re entering
//     unsigned int pathResolution,  // samples along blend
//     float width)                  // halfpipe width
// {
//     Mesh mesh;
//     std::vector<float> vertices;
//     std::vector<unsigned int> indices;

//     if (fromPiece.mesh.vertices.empty() || toPiece.mesh.vertices.empty()) return mesh;

//     // Grab end of "from" piece
//     glm::vec3 p0 = fromPiece.endPoint;
//     glm::vec3 t0 = fromPiece.endTangent;
//     glm::vec3 right0 = fromPiece.endRight;
//     glm::vec3 up0 = fromPiece.endUp;

//     // Grab start of "to" piece
//     glm::vec3 p1 = toPiece.startPoint;
//     glm::vec3 t1 = toPiece.startTangent;
//     glm::vec3 right1 = toPiece.startRight;
//     glm::vec3 up1 = toPiece.startUp;

//     const unsigned int arcSegments = 8; // cross-section resolution
//     const unsigned int ringVerts = arcSegments + 1;
//     const unsigned int vertsPerRing = ringVerts + 4; // include rails if needed

//     for (unsigned int i = 0; i <= pathResolution; ++i)
//     {
//         float blendT = float(i) / float(pathResolution);

//         // Interpolate position along path
//         glm::vec3 pos = glm::mix(p0, p1, blendT);

//         // Interpolate tangent/right/up vectors
//         glm::vec3 tangent = glm::normalize(glm::mix(t0, t1, blendT));
//         glm::vec3 right   = glm::normalize(glm::mix(right0, right1, blendT));
//         glm::vec3 up      = glm::normalize(glm::cross(tangent, right));

//         // v-coordinate
//         float v = blendT;

//         // Generate halfpipe ring
//         for (unsigned int a = 0; a <= arcSegments; ++a)
//         {
//             float arcT = float(a) / float(arcSegments);
//             float angle = -glm::half_pi<float>() + arcT * glm::pi<float>();

//             float x = width * std::sin(angle);
//             float y = width * (1.0f - std::cos(angle));

//             glm::vec3 vertPos = pos + right * x + up * y;

//             glm::vec3 normal = glm::normalize(right * std::sin(angle) + up * std::cos(angle));

//             // Push vertex (pos, normal, uv, color)
//             vertices.insert(vertices.end(), {
//                                                 vertPos.x, vertPos.y, vertPos.z,
//                                                 normal.x, normal.y, normal.z,
//                                                 arcT, v,
//                                                 0.6f, 0.6f, 0.6f
//                                             });
//         }
//     }

//     // Build indices (same stitching as halfpipe)
//     for (unsigned int i = 0; i < pathResolution; ++i)
//     {
//         unsigned int base = i * vertsPerRing;
//         unsigned int next = (i + 1) * vertsPerRing;

//         for (unsigned int a = 0; a < ringVerts - 1; ++a)
//         {
//             unsigned int b0 = base + a;
//             unsigned int b1 = base + a + 1;
//             unsigned int n0 = next + a;
//             unsigned int n1 = next + a + 1;

//             indices.push_back(b0); indices.push_back(n0); indices.push_back(b1);
//             indices.push_back(b1); indices.push_back(n0); indices.push_back(n1);
//         }
//     }

//     mesh.vertices = std::move(vertices);
//     mesh.indices  = std::move(indices);

//     GLsizei stride = 11 * sizeof(float);
//     mesh.attributes = {
//         {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
//         {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
//         {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
//         {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
//     };

//     mesh.setup();
//     return mesh;
// }


// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& controlPoints,
//     float width,
//     unsigned int pathResolution)
// {
//     Mesh mesh;
//     std::vector<float> vertices;
//     std::vector<unsigned int> indices;

//     if (controlPoints.size() < 2) return mesh;

//     const float halfW = width * 0.5f;

//     // 1) sample Catmull-Rom path
//     std::vector<glm::vec3> path;
//     for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for (unsigned int j = 0; j < pathResolution; ++j) {
//             float t = float(j) / float(pathResolution);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i + 1];
//             glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

//             path.push_back(catmullRom(p0, p1, p2, p3, t));
//         }
//     }
//     path.push_back(controlPoints.back());

//     const unsigned int vertsPerRing = 2; // left/right per path sample
//     const glm::vec3 color(1.0f, 1.0f, 1.0f); // white

//     // 2) generate vertices
//     for (size_t i = 0; i < path.size(); ++i) {
//         glm::vec3 forward = (i < path.size() - 1) ? glm::normalize(path[i + 1] - path[i])
//                                                   : glm::normalize(path[i] - path[i - 1]);
//         glm::vec3 up = glm::vec3(0,1,0);
//         glm::vec3 right = glm::normalize(glm::cross(up, forward));
//         up = glm::normalize(glm::cross(forward, right));

//         glm::vec3 leftPos  = path[i] - right * halfW;
//         glm::vec3 rightPos = path[i] + right * halfW;
//         glm::vec3 normal = up;

//         float v = float(i) / float(path.size() - 1);

//         auto pushVert = [&](const glm::vec3& pos, const glm::vec3& n, float u, float v) {
//             vertices.insert(vertices.end(), {
//                 pos.x, pos.y, pos.z,
//                 n.x, n.y, n.z,
//                 u, v,
//                 color.r, color.g, color.b
//             });
//         };

//         pushVert(leftPos, normal, 0.0f, v);
//         pushVert(rightPos, normal, 1.0f, v);
//     }

//     // 3) generate indices
//     for (size_t i = 0; i < path.size() - 1; ++i) {
//         unsigned int base = i * vertsPerRing;
//         unsigned int next = (i + 1) * vertsPerRing;

//         // two triangles per quad
//         indices.push_back(base + 0); indices.push_back(next + 0); indices.push_back(base + 1);
//         indices.push_back(base + 1); indices.push_back(next + 0); indices.push_back(next + 1);
//     }

//     // 4) setup mesh
//     mesh.vertices = std::move(vertices);
//     mesh.indices  = std::move(indices);

//     GLsizei stride = 11 * sizeof(float);
//     mesh.attributes = {
//         {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
//         {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
//         {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
//         {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
//     };
//     mesh.setup();

//     return mesh;
// }


Mesh createTube(
    const std::vector<glm::vec3>& controlPoints,
    float radius,
    unsigned int radialSegments)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (controlPoints.size() < 2) return mesh;
    if (radialSegments < 3) radialSegments = 3;

    // 1) sample Catmull-Rom path
    std::vector<glm::vec3> path;
    for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
        for (unsigned int j = 0; j < radialSegments; ++j) {
            float t = float(j) / float(radialSegments);

            glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
            glm::vec3 p1 = controlPoints[i];
            glm::vec3 p2 = controlPoints[i + 1];
            glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

            path.push_back(catmullRom(p0, p1, p2, p3, t));
        }
    }
    path.push_back(controlPoints.back());

    const unsigned int vertsPerRing = radialSegments;

    // 2) generate vertices
    for (size_t i = 0; i < path.size(); ++i) {
        glm::vec3 forward = (i < path.size() - 1) ? glm::normalize(path[i + 1] - path[i])
                                                  : glm::normalize(path[i] - path[i - 1]);
        glm::vec3 worldUp = glm::vec3(0,1,0);
        glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
        if (glm::length(right) < 1e-4f) right = glm::vec3(1,0,0);
        glm::vec3 up = glm::normalize(glm::cross(forward, right));

        float v = float(i) / float(path.size() - 1);

        for (unsigned int s = 0; s < radialSegments; ++s) {
            float angle = float(s) / radialSegments * glm::two_pi<float>();
            glm::vec3 normal = glm::cos(angle)*right + glm::sin(angle)*up;
            glm::vec3 pos = path[i] + normal*radius;

            float u = float(s) / radialSegments;
            glm::vec3 color(1.0f, 1.0f, 1.0f);

            vertices.insert(vertices.end(), {
                pos.x, pos.y, pos.z,
                normal.x, normal.y, normal.z,
                u, v,
                color.r, color.g, color.b
            });
        }
    }

    // 3) generate indices
    for (size_t i = 0; i < path.size() - 1; ++i) {
        unsigned int base = i * vertsPerRing;
        unsigned int next = (i + 1) * vertsPerRing;

        for (unsigned int s = 0; s < radialSegments; ++s) {
            unsigned int sNext = (s + 1) % radialSegments;

            indices.push_back(base + s);
            indices.push_back(base + sNext);
            indices.push_back(next + sNext);

            indices.push_back(base + s);
            indices.push_back(next + sNext);
            indices.push_back(next + s);
        }
    }

    // 4) setup mesh
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
        {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
        {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
        {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
    };
    mesh.setup();

    return mesh;
}



// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& controlPoints,
//     float width,
//     unsigned int pathResolution)
// {
//     Mesh mesh;

//     std::vector<float> verts;
//     std::vector<unsigned int> inds;

//     if (controlPoints.size() < 2) return mesh;

//     const float halfW = width * 0.5f;

//     // 1) sample Catmull-Rom path
//     std::vector<glm::vec3> path;
//     for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for (unsigned int j = 0; j < pathResolution; ++j) {
//             float t = float(j) / float(pathResolution);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i + 1];
//             glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

//             path.push_back(catmullRom(p0, p1, p2, p3, t));
//         }
//     }
//     path.push_back(controlPoints.back());

//     // 2) generate vertices
//     for (size_t i = 0; i < path.size(); ++i) {
//         glm::vec3 forward = (i < path.size() - 1) ? glm::normalize(path[i + 1] - path[i])
//                                                   : glm::normalize(path[i] - path[i - 1]);
//         glm::vec3 up = glm::vec3(0,1,0);

//         glm::vec3 right = glm::normalize(glm::cross(up, forward));
//         up = glm::normalize(glm::cross(forward, right));

//         glm::vec3 leftPos  = path[i] - right * halfW;
//         glm::vec3 rightPos = path[i] + right * halfW;
//         glm::vec3 normal = up;

//         float v = float(i) / float(path.size() - 1);

//         auto pushVert = [&](const glm::vec3& p, const glm::vec3& n, float u, float v){
//             verts.insert(verts.end(), { p.x, p.y, p.z, n.x, n.y, n.z, u, v, 1,1,1 });
//         };

//         pushVert(leftPos,  normal, 0.0f, v);
//         pushVert(rightPos, normal, 1.0f, v);
//     }

//     // 3) generate indices
//     for (unsigned int i = 0; i < path.size() - 1; ++i) {
//         unsigned int i0 = i*2;
//         unsigned int i1 = i0 + 1;
//         unsigned int i2 = i1 + 1;
//         unsigned int i3 = i0 + 2;
//         inds.insert(inds.end(), { i0, i1, i2, i0, i2, i3 });
//     }

//     mesh.vertices = std::move(verts);
//     mesh.indices  = std::move(inds);

//     GLsizei stride = 11 * sizeof(float);
//     mesh.attributes = {
//         {0,3,GL_FLOAT,GL_FALSE,stride,0},
//         {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
//         {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
//         {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)}
//     };
//     mesh.setup();

//     return mesh;
// }



// Mesh createTube(
//     const std::vector<glm::vec3>& controlPoints,
//     float radius,
//     unsigned int radialSegments)
// {
//     Mesh mesh;

//     std::vector<float> verts;
//     std::vector<unsigned int> inds;

//     if (controlPoints.size() < 2) return mesh;

//     // 1) sample Catmull-Rom path
//     std::vector<glm::vec3> path;
//     for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
//         for (unsigned int j = 0; j < radialSegments; ++j) {
//             float t = float(j) / float(radialSegments);

//             glm::vec3 p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
//             glm::vec3 p1 = controlPoints[i];
//             glm::vec3 p2 = controlPoints[i + 1];
//             glm::vec3 p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

//             path.push_back(catmullRom(p0,p1,p2,p3,t));
//         }
//     }
//     path.push_back(controlPoints.back());

//     // 2) generate vertices
//     for (size_t i = 0; i < path.size(); ++i) {
//         glm::vec3 forward = (i < path.size() - 1) ? glm::normalize(path[i+1]-path[i])
//                                                   : glm::normalize(path[i]-path[i-1]);
//         glm::vec3 worldUp = glm::vec3(0,1,0);
//         glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
//         if (glm::length(right) < 1e-4f) right = glm::vec3(1,0,0);
//         glm::vec3 up = glm::normalize(glm::cross(forward, right));

//         for (unsigned int s = 0; s < radialSegments; ++s){
//             float angle = float(s)/radialSegments * glm::two_pi<float>();
//             glm::vec3 normal = glm::cos(angle)*right + glm::sin(angle)*up;
//             glm::vec3 pos = path[i] + normal*radius;

//             float u = float(s)/radialSegments;
//             float v = float(i)/float(path.size()-1);

//             verts.insert(verts.end(), { pos.x,pos.y,pos.z, normal.x,normal.y,normal.z, u,v, 1,1,1 });
//         }
//     }

//     // 3) indices
//     unsigned int ringSize = radialSegments;
//     for (unsigned int i = 0; i < path.size() - 1; ++i){
//         unsigned int ring0 = i * ringSize;
//         unsigned int ring1 = ring0 + ringSize;

//         for (unsigned int s = 0; s < radialSegments; ++s){
//             unsigned int sNext = (s+1)%radialSegments;
//             unsigned int i0 = ring0+s;
//             unsigned int i1 = ring0+sNext;
//             unsigned int i2 = ring1+sNext;
//             unsigned int i3 = ring1+s;
//             inds.insert(inds.end(), { i0,i1,i2, i0,i2,i3 });
//         }
//     }

//     mesh.vertices = std::move(verts);
//     mesh.indices  = std::move(inds);

//     GLsizei stride = 11*sizeof(float);
//     mesh.attributes = {
//         {0,3,GL_FLOAT,GL_FALSE,stride,0},
//         {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
//         {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
//         {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)}
//     };
//     mesh.setup();

//     return mesh;
// }


// Mesh createFlatBoard(
//     const std::vector<glm::vec3>& path,
//     float width,
//     unsigned int resolution
//     ){
//     Mesh mesh;

//     if (path.size() < 2) return mesh;

//     const float halfW = width * 0.5f;

//     std::vector<float> verts;
//     std::vector<unsigned int> inds;

//     for (size_t i = 0; i < path.size(); ++i) {

//         glm::vec3 forward;
//         if (i == path.size() - 1)
//             forward = glm::normalize(path[i] - path[i - 1]);
//         else
//             forward = glm::normalize(path[i + 1] - path[i]);

//         glm::vec3 up = glm::vec3(0,1,0);

//         glm::vec3 right = glm::normalize(glm::cross(up, forward));
//         up = glm::normalize(glm::cross(forward, right)); // re-orthogonalize

//         glm::vec3 leftPos  = path[i] - right * halfW;
//         glm::vec3 rightPos = path[i] + right * halfW;

//         glm::vec3 normal = up;

//         // UVs
//         float v = float(i) / float(path.size() - 1);

//         // Vertex packing helper:
//         auto pushVert = [&](const glm::vec3& p, const glm::vec3& n, float u, float v) {
//             verts.insert(verts.end(), {
//                                           p.x, p.y, p.z,
//                                           n.x, n.y, n.z,
//                                           u, v,
//                                           1.0f, 1.0f, 1.0f      // white color
//                                       });
//         };

//         pushVert(leftPos,  normal, 0.0f, v);
//         pushVert(rightPos, normal, 1.0f, v);
//     }

//     // Indices: two triangles per segment
//     unsigned int vertsPerRow = 2;

//     for (unsigned int i = 0; i < path.size() - 1; i++) {
//         unsigned int i0 = i * vertsPerRow;
//         unsigned int i1 = i0 + 1;
//         unsigned int i2 = i1 + 1;
//         unsigned int i3 = i0 + 2;

//         inds.insert(inds.end(), {
//                                     i0, i1, i2,
//                                     i0, i2, i3
//                                 });
//     }

//     mesh.vertices = std::move(verts);
//     mesh.indices  = std::move(inds);

//     GLsizei stride = 11 * sizeof(float);

//     mesh.attributes = {
//         {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
//         {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
//         {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
//         {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
//     };

//     mesh.setup();
//     return mesh;
// }


// Mesh createTube(
//     const std::vector<glm::vec3>& path,
//     float radius,
//     unsigned int radialSegments)
// {
//     Mesh mesh;

//     if (path.size() < 2) return mesh;

//     std::vector<float> verts;
//     std::vector<unsigned int> inds;

//     for (size_t i = 0; i < path.size(); ++i) {

//         glm::vec3 forward;
//         if (i == path.size() - 1)
//             forward = glm::normalize(path[i] - path[i - 1]);
//         else
//             forward = glm::normalize(path[i + 1] - path[i]);

//         glm::vec3 worldUp = glm::vec3(0, 1, 0);

//         glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
//         if (glm::length(right) < 1e-4f)
//             right = glm::vec3(1,0,0);

//         glm::vec3 up = glm::normalize(glm::cross(forward, right));

//         // Build ring
//         for (unsigned int s = 0; s < radialSegments; ++s){
//             float angle = (float(s) / radialSegments) * glm::two_pi<float>();

//             glm::vec3 normal = glm::cos(angle) * right + glm::sin(angle) * up;
//             glm::vec3 pos = path[i] + normal * radius;

//             float u = float(s) / float(radialSegments);
//             float v = float(i) / float(path.size() - 1);

//             verts.insert(verts.end(), {
//                                           pos.x, pos.y, pos.z,
//                                           normal.x, normal.y, normal.z,
//                                           u, v,
//                                           1,1,1
//                                       });
//         }
//     }

//     // Indices
//     const unsigned int ringSize = radialSegments;

//     for (unsigned int i = 0; i < path.size() - 1; ++i) {
//         unsigned int ring0 = i * ringSize;
//         unsigned int ring1 = ring0 + ringSize;

//         for (unsigned int s = 0; s < radialSegments; ++s){
//             unsigned int sNext = (s + 1) % radialSegments;

//             unsigned int i0 = ring0 + s;
//             unsigned int i1 = ring0 + sNext;
//             unsigned int i2 = ring1 + sNext;
//             unsigned int i3 = ring1 + s;

//             inds.insert(inds.end(), {
//                                         i0, i1, i2,
//                                         i0, i2, i3
//                                     });
//         }
//     }

//     mesh.vertices = std::move(verts);
//     mesh.indices  = std::move(inds);

//     GLsizei stride = 11 * sizeof(float);

//     mesh.attributes = {
//         {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
//         {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
//         {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
//         {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)}
//     };

//     mesh.setup();
//     return mesh;
// }





