#ifndef BEZIERCLASS_H
#define BEZIERCLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct BezierSegment{
    glm::vec3 p0, p1, p2, p3;
};


class BezierTrack{
public:
    std::vector<BezierSegment> segments;


    glm::vec3 evaluate(float t) const;
    glm::vec3 tangent(float t) const;

};

#endif // BEZIERCLASS_H
