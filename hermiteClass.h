#ifndef HERMITECLASS_H
#define HERMITECLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct HermiteSegment{
    glm::vec3 p0, p1;
    glm::vec3 m0, m1;
};


class HermiteTrack{
public:
    std::vector<HermiteSegment> segments;

    glm::vec3 evaluate(float t) const;
    glm::vec3 tangent(float t) const;
    void fromControlPoints(const std::vector<glm::vec3>& controlPoints);


};




#endif // HERMITECLASS_H
