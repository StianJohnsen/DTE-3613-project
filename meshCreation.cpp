#include "meshCreation.h"
#include "renderer.h"

Mesh createSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
{
    Mesh mesh;

    std::vector<float> verts;
    std::vector<unsigned int> inds;

    verts.clear();
    inds.clear();

    const float PI = acos(-1.0f);

    float x, y, z, xy;
    float nx, ny, nz;
    float s, t;

    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = PI / 2.0f - i * PI / (float)stackCount;

        xy = radius * cosf(stackAngle);
        z  = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * 2.0f * PI / (float)sectorCount;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);

            nx = x / radius;
            ny = y / radius;
            nz = z / radius;
            verts.push_back(nx);
            verts.push_back(ny);
            verts.push_back(nz);

            s = (float)j / (float)sectorCount;
            t = (float)i / (float)stackCount;
            verts.push_back(s);
            verts.push_back(t);

            verts.push_back(0.0f);
            verts.push_back(0.0f);
            verts.push_back(1.0f);
        }
    }

    for (unsigned int i = 0; i < stackCount; ++i)
    {
        unsigned int k1 = i * (sectorCount + 1);
        unsigned int k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                inds.push_back(k1);
                inds.push_back(k2);
                inds.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                inds.push_back(k1 + 1);
                inds.push_back(k2);
                inds.push_back(k2 + 1);
            }
        }
    }


    mesh.vertices = std::move(verts);
    mesh.indices  = std::move(inds);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
                       {0, 3, GL_FLOAT, GL_FALSE, stride, 0},
                       {1, 3, GL_FLOAT, GL_FALSE, stride, 3*sizeof(float)},
                       {2, 2, GL_FLOAT, GL_FALSE, stride, 6*sizeof(float)},
                       {3, 3, GL_FLOAT, GL_FALSE, stride, 8*sizeof(float)},
                       };

    mesh.setup();
    return mesh;
}


Mesh createSkybox(){
    Mesh mesh;
    std::vector<float> verts = {
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

    mesh.vertices = std::move(verts);
    mesh.indices = {};
    GLsizei stride = 3 * sizeof(float);
    mesh.attributes = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, 0}
    };

    mesh.setup();
    return mesh;
}


Mesh createWall(float width, float height, float thickness)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float hw = width * 0.5f;
    const float hh = height * 0.5f;
    const float ht = thickness * 0.5f;

    float uFront  = width      / height;
    float uSide   = thickness  / height;
    float uTopBot = width      / thickness;

    const glm::vec3 color(0.9f, 0.1f, 0.1f);

    auto pushVert = [&](glm::vec3 p, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(),
                        { p.x,p.y,p.z,  n.x,n.y,n.z,  u,v,  color.r,color.g,color.b });
    };

    glm::vec3 p[8] = {
        {-hw, -hh, -ht},
        { hw, -hh, -ht},
        { hw,  hh, -ht},
        {-hw,  hh, -ht},
        {-hw, -hh,  ht},
        { hw, -hh,  ht},
        { hw,  hh,  ht},
        {-hw,  hh,  ht}
    };

    pushVert(p[4], {0,0,1}, 0.0f, 0.0f);
    pushVert(p[5], {0,0,1}, uFront, 0.0f);
    pushVert(p[6], {0,0,1}, uFront, 1.0f);
    pushVert(p[7], {0,0,1}, 0.0f, 1.0f);

    pushVert(p[0], {0,0,-1}, 0.0f, 0.0f);
    pushVert(p[1], {0,0,-1}, uFront, 0.0f);
    pushVert(p[2], {0,0,-1}, uFront, 1.0f);
    pushVert(p[3], {0,0,-1}, 0.0f, 1.0f);

    pushVert(p[0], {-1,0,0}, 0.0f, 0.0f);
    pushVert(p[3], {-1,0,0}, uSide, 0.0f);
    pushVert(p[7], {-1,0,0}, uSide, 1.0f);
    pushVert(p[4], {-1,0,0}, 0.0f, 1.0f);

    pushVert(p[1], {1,0,0}, 0.0f, 0.0f);
    pushVert(p[2], {1,0,0}, uSide, 0.0f);
    pushVert(p[6], {1,0,0}, uSide, 1.0f);
    pushVert(p[5], {1,0,0}, 0.0f, 1.0f);

    pushVert(p[3], {0,1,0}, 0.0f,      0.0f);
    pushVert(p[2], {0,1,0}, uTopBot,   0.0f);
    pushVert(p[6], {0,1,0}, uTopBot,   1.0f);
    pushVert(p[7], {0,1,0}, 0.0f,      1.0f);

    pushVert(p[0], {0,-1,0}, 0.0f,     0.0f);
    pushVert(p[1], {0,-1,0}, uTopBot,  0.0f);
    pushVert(p[5], {0,-1,0}, uTopBot,  1.0f);
    pushVert(p[4], {0,-1,0}, 0.0f,     1.0f);

    for (int face = 0; face < 6; face++) {
        unsigned int o = face * 4;
        indices.insert(indices.end(),
                       { o+0,o+1,o+2,  o+2,o+3,o+0 });
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

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

Mesh createEndWall(float width, float height, float thickness)
{
    Mesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float hw = width * 0.5f;
    float ht = thickness * 0.5f;

    glm::vec3 p[] =
        {
            {-hw, 0,  -ht}, { hw, 0,  -ht}, { hw, height, -ht}, { -hw, height, -ht},
            {-hw, 0,   ht}, { hw, 0,   ht}, { hw, height, ht},  { -hw, height, ht}
        };

    auto push = [&](glm::vec3 pos, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(),
                        {pos.x,pos.y,pos.z, n.x,n.y,n.z, u,v, 1,1,1});
    };

    float uFront  = width     / height;
    float uSide   = thickness / height;
    float uTopBot = width     / thickness;

    push(p[4], {0,0,1}, 0,0);
    push(p[5], {0,0,1}, uFront,0);
    push(p[6], {0,0,1}, uFront,1);
    push(p[7], {0,0,1}, 0,1);

    push(p[0], {0,0,-1}, 0,0);
    push(p[1], {0,0,-1}, uFront,0);
    push(p[2], {0,0,-1}, uFront,1);
    push(p[3], {0,0,-1}, 0,1);

    push(p[0], {-1,0,0}, 0,0);
    push(p[3], {-1,0,0}, uSide,0);
    push(p[7], {-1,0,0}, uSide,1);
    push(p[4], {-1,0,0}, 0,1);

    push(p[1], {1,0,0}, 0,0);
    push(p[2], {1,0,0}, uSide,0);
    push(p[6], {1,0,0}, uSide,1);
    push(p[5], {1,0,0}, 0,1);

    push(p[3], {0,1,0}, 0,0);
    push(p[2], {0,1,0}, uTopBot,0);
    push(p[6], {0,1,0}, uTopBot,1);
    push(p[7], {0,1,0}, 0,1);

    push(p[0], {0,-1,0}, 0,0);
    push(p[1], {0,-1,0}, uTopBot,0);
    push(p[5], {0,-1,0}, uTopBot,1);
    push(p[4], {0,-1,0}, 0,1);

    for (int face = 0; face < 6; face++) {
        unsigned int b = face * 4;
        indices.insert(indices.end(), {b, b+1, b+2, b+2, b+3, b});
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);


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

    auto push = [&](glm::vec3 p, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(),
                        {p.x,p.y,p.z, n.x,n.y,n.z, u,v, color.r,color.g,color.b});
    };

    float h2 = height * 0.5f;
    float s  = baseSize * 0.5f;

    glm::vec3 tip(0, +h2, 0);

    glm::vec3 base[4] = {
        {-s, -h2, -s},
        { s, -h2, -s},
        { s, -h2,  s},
        {-s, -h2,  s}
    };

    push(base[0], {0,-1,0}, 0,0);
    push(base[1], {0,-1,0}, 1,0);
    push(base[2], {0,-1,0}, 1,1);
    push(base[3], {0,-1,0}, 0,1);
    indices.insert(indices.end(), {0,1,2, 2,3,0});

    for (int i = 0; i < 4; i++)
    {
        glm::vec3 a = base[i];
        glm::vec3 b = base[(i+1)%4];
        glm::vec3 c = tip;

        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));

        unsigned int bi = vertices.size() / 11;
        push(a,n, 0,0);
        push(b,n, 1,0);
        push(c,n, 0.5f,1);

        indices.insert(indices.end(), {bi, bi+1, bi+2});
    }

    GLsizei stride = 11*sizeof(float);
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);
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
    std::vector<float> verts;
    std::vector<unsigned int> inds;

    const glm::vec3 color(0.2f, 0.8f, 0.3f);

    auto push = [&](glm::vec3 p, glm::vec3 n, float u, float v)
    {
        verts.insert(verts.end(),
                     {p.x,p.y,p.z, n.x,n.y,n.z, u,v, color.r,color.g,color.b});
    };

    float w = width*0.5f;
    float d = depth*0.5f;
    float h2 = height * 0.5f;

    glm::vec3 A(-w,-h2,-d);
    glm::vec3 B( w,-h2,-d);
    glm::vec3 C( 0, h2,-d);

    glm::vec3 D(-w,-h2, d);
    glm::vec3 E( w,-h2, d);
    glm::vec3 F( 0, h2, d);

    auto quad = [&](unsigned int b){
        inds.insert(inds.end(),
                    {b+0,b+1,b+2,  b+2,b+3,b+0});
    };

    glm::vec3 nF = glm::normalize(glm::cross(B-A,C-A));
    push(A,nF,0,0);
    push(B,nF,1,0);
    push(C,nF,0.5,1);
    inds.insert(inds.end(), {0,1,2});

    glm::vec3 nB = glm::normalize(glm::cross(E-D,F-D));
    unsigned int ib = verts.size()/11;
    push(D,nB,0,0);
    push(E,nB,1,0);
    push(F,nB,0.5,1);
    inds.insert(inds.end(), {ib,ib+1,ib+2});

    int ib2 = verts.size()/11;
    push(A,{0,-1,0},0,0);
    push(B,{0,-1,0},1,0);
    push(E,{0,-1,0},1,1);
    push(D,{0,-1,0},0,1);
    quad(ib2);

    int ib3 = verts.size()/11;
    glm::vec3 nR = glm::normalize(glm::cross(C-B,F-B));
    push(B,nR,0,0);
    push(C,nR,1,0);
    push(F,nR,1,1);
    push(E,nR,0,1);
    quad(ib3);

    int ib4 = verts.size()/11;
    glm::vec3 nL = glm::normalize(glm::cross(D-A,F-A));
    push(A,nL,0,0);
    push(D,nL,1,0);
    push(F,nL,1,1);
    push(C,nL,0,1);
    quad(ib4);

    GLsizei stride = 11*sizeof(float);
    mesh.vertices = std::move(verts);
    mesh.indices  = std::move(inds);
    mesh.attributes = {
        {0,3,GL_FLOAT,GL_FALSE,stride,0},
        {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
        {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
        {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)}
    };
    mesh.setup();
    return mesh;
}

MeshInstance createFlatBoardLocal(
    const std::vector<glm::vec3>& pathWorld,
    float width)
{
    MeshInstance out;

    const size_t Nsamples = pathWorld.size();
    if (Nsamples < 2)
        return out;

    glm::vec3 root = pathWorld.front();

    std::vector<glm::vec3> path(Nsamples);
    for (size_t i = 0; i < Nsamples; ++i)
        path[i] = pathWorld[i] - root;

    std::vector<float> cumLen(Nsamples);
    float totalLen = 0.0f;
    cumLen[0] = 0.0f;
    for (size_t i = 1; i < Nsamples; ++i) {
        totalLen += glm::length(path[i] - path[i - 1]);
        cumLen[i] = totalLen;
    }
    if (totalLen < 1e-6f) totalLen = 1.0f;

    std::vector<glm::vec3> tangent(Nsamples);
    std::vector<glm::vec3> right  (Nsamples);
    std::vector<glm::vec3> up     (Nsamples);

    for (size_t i = 0; i < Nsamples; ++i)
    {
        glm::vec3 T;
        if (i == 0)
            T = path[1] - path[0];
        else if (i == Nsamples - 1)
            T = path[i] - path[i - 1];
        else
            T = path[i + 1] - path[i - 1];

        if (glm::length(T) < 1e-6f)
            T = glm::vec3(0,0,1);

        T = glm::normalize(T);

        glm::vec3 W(0,1,0);
        if (fabs(glm::dot(W, T)) > 0.9f)
            W = glm::vec3(1,0,0);

        glm::vec3 R = glm::normalize(glm::cross(W, T));
        glm::vec3 U = glm::normalize(glm::cross(T, R));

        tangent[i] = T;
        right[i]   = R;
        up[i]      = U;
    }

    Mesh mesh;
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const float halfW = width * 0.5f;

    float railingHeight    = 1.9f;
    float railingOffset    = 0.05f;
    float railingThickness = 0.04f;
    float boardThickness   = 0.05f;

    const glm::vec3 WHITE(1,1,1);

    auto pushVert = [&](glm::vec3 p, glm::vec3 n, float u, float v)
    {
        vertices.insert(vertices.end(),
                        {
                            p.x,p.y,p.z,
                            n.x,n.y,n.z,
                            u,v,
                            WHITE.r,WHITE.g,WHITE.b
                        });
    };

    const unsigned int vertsPer = 16;

    float uScaleRail = totalLen / railingHeight;

    for (size_t i = 0; i < Nsamples; ++i)
    {
        glm::vec3 center = path[i];
        glm::vec3 R      = right[i];
        glm::vec3 U      = up[i];

        glm::vec3 leftEdge  = center - R * halfW;
        glm::vec3 rightEdge = center + R * halfW;

        float vPath = cumLen[i] / totalLen;

        pushVert(leftEdge,  U, 0.0f, vPath);
        pushVert(rightEdge, U, 1.0f, vPath);

        glm::vec3 leftBase  = leftEdge  - R * railingOffset;
        glm::vec3 rightBase = rightEdge + R * railingOffset;
        glm::vec3 leftTop   = leftBase  + U * railingHeight;
        glm::vec3 rightTop  = rightBase + U * railingHeight;

        pushVert(leftBase,  U, vPath * uScaleRail,                      0.0f);
        pushVert(leftTop,   U, vPath * uScaleRail,                      1.0f);
        pushVert(rightBase, U, vPath * uScaleRail + (width/railingHeight), 0.0f);
        pushVert(rightTop,  U, vPath * uScaleRail + (width/railingHeight), 1.0f);

        glm::vec3 leftBot  = leftEdge  - U * boardThickness;
        glm::vec3 rightBot = rightEdge - U * boardThickness;

        pushVert(leftBot,  -U, 0.0f, vPath);
        pushVert(rightBot, -U, 1.0f, vPath);

        glm::vec3 leftBaseB  = leftBase  + R * railingThickness;
        glm::vec3 leftTopB   = leftTop   + R * railingThickness;
        glm::vec3 rightBaseB = rightBase - R * railingThickness;
        glm::vec3 rightTopB  = rightTop  - R * railingThickness;

        pushVert(leftBaseB,  U, vPath * uScaleRail,                      0.0f);
        pushVert(leftTopB,   U, vPath * uScaleRail,                      1.0f);
        pushVert(rightBaseB, U, vPath * uScaleRail + (width/railingHeight), 0.0f);
        pushVert(rightTopB,  U, vPath * uScaleRail + (width/railingHeight), 1.0f);


        pushVert(leftEdge,  -R, vPath * uScaleRail,                      1.0f);
        pushVert(leftBot,   -R, vPath * uScaleRail,                      0.0f);
        pushVert(rightEdge,  R, vPath * uScaleRail + (width/railingHeight), 1.0f);
        pushVert(rightBot,   R, vPath * uScaleRail + (width/railingHeight), 0.0f);
    }

    for (size_t i = 0; i < Nsamples - 1; ++i)
    {
        unsigned int a = i * vertsPer;
        unsigned int b = (i + 1) * vertsPer;

        indices.insert(indices.end(), { a+0,b+0,a+1, a+1,b+0,b+1 });

        indices.insert(indices.end(), { a+6,b+6,a+7, a+7,b+6,b+7 });

        indices.insert(indices.end(), { a+12,a+13,b+12, b+12,a+13,b+13 });
        indices.insert(indices.end(), { a+14,b+14,a+15, a+15,b+14,b+15 });

        indices.insert(indices.end(), { a+2,b+2,a+3, a+3,b+2,b+3 });
        indices.insert(indices.end(), { a+4,b+4,a+5, a+5,b+4,b+5 });

        indices.insert(indices.end(), { a+8,b+8,a+9, a+9,b+8,b+9 });
        indices.insert(indices.end(), { a+10,b+10,a+11, a+11,b+10,b+11 });

        indices.insert(indices.end(), { a+2,a+8,b+2, b+2,a+8,b+8 });
        indices.insert(indices.end(), { a+3,b+3,a+9, a+9,b+3,b+9 });

        indices.insert(indices.end(), { a+4,a+10,b+4, b+4,a+10,b+10 });
        indices.insert(indices.end(), { a+5,b+5,a+11, a+11,b+5,b+11 });
    }

    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    GLsizei stride = 11 * sizeof(float);
    mesh.attributes = {
        {0,3,GL_FLOAT,GL_FALSE,stride,0},
        {1,3,GL_FLOAT,GL_FALSE,stride,3*sizeof(float)},
        {2,2,GL_FLOAT,GL_FALSE,stride,6*sizeof(float)},
        {3,3,GL_FLOAT,GL_FALSE,stride,8*sizeof(float)}
    };

    mesh.setup();


    out.mesh  = mesh;
    out.model = glm::translate(glm::mat4(1.0f), root);

    return out;
}

Mesh createGuideDeflectorLocal(
    float length,
    float innerWidthStart,
    float innerWidthEnd,
    float height)
{
    Mesh mesh;

    std::vector<float> verts;
    std::vector<unsigned int> inds;

    auto push = [&](glm::vec3 p, glm::vec3 n)
    {
        verts.insert(verts.end(),
                     {
                         p.x, p.y, p.z,
                         n.x, n.y, n.z,
                         0,0, 1,1,1
                     });
    };

    float L = length;

    float W0 = innerWidthStart;
    float W1 = innerWidthEnd;
    float H  = height;


    glm::vec3 A( 0, H, 0 );
    glm::vec3 B(-W0, H, 0 );
    glm::vec3 C(-W1, H, L );
    glm::vec3 D( 0, H, L );


    glm::vec3 A2 = glm::vec3(A.x, 0, A.z);
    glm::vec3 B2 = glm::vec3(B.x, 0, B.z);
    glm::vec3 C2 = glm::vec3(C.x, 0, C.z);
    glm::vec3 D2 = glm::vec3(D.x, 0, D.z);

    glm::vec3 N = glm::vec3(0,1,0);

    push(A, N); push(B, N); push(C, N); push(D, N);

    push(A2,-N); push(B2,-N); push(C2,-N); push(D2,-N);

    inds = {
        0,1,2, 0,2,3,
        4,6,5, 4,7,6
    };

    inds.insert(inds.end(), { 1,5,6, 1,6,2 });

    inds.insert(inds.end(), { 0,4,7, 0,7,3 });

    inds.insert(inds.end(), { 0,1,5, 0,5,4 });

    inds.insert(inds.end(), { 3,7,6, 3,6,2 });

    mesh.vertices = verts;
    mesh.indices  = inds;

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
