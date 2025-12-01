#include "bezierClass.h"


glm::vec3 BezierTrack::evaluate(float t) const {
    if (segments.empty()) return glm::vec3(0);

    int segCount = (int)segments.size();
    float segT = t * segCount;
    int i = glm::clamp((int)floor(segT), 0, segCount - 1);
    float localT = segT - i;

    const auto& s = segments[i];
    float u = 1.0f - localT;

    return u*u*u*s.p0 +
           3.0f*u*u*localT*s.p1 +
           3.0f*u*localT*localT*s.p2 +
           localT*localT*localT*s.p3;
}

glm::vec3 BezierTrack::tangent(float t) const {
    if (segments.empty()) return glm::vec3(0);
    int segCount = (int)segments.size();
    float segT = t * segCount;
    int i = glm::clamp((int)floor(segT), 0, segCount - 1);
    float localT = segT - i;

    const auto& s = segments[i];
    float u = 1.0f - localT;

    glm::vec3 d =
        -3.0f*u*u*s.p0 +
        (3.0f*u*u - 6.0f*u*localT)*s.p1 +
        (6.0f*u*localT - 3.0f*localT*localT)*s.p2 +
        3.0f*localT*localT*s.p3;
    return glm::normalize(d);
}
