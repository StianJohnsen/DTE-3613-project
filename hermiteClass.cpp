#include "hermiteClass.h"


glm::vec3 HermiteTrack::evaluate(float t) const {
    if(segments.empty()) return glm::vec3(0);

    int segCount = (int)segments.size();

    float segT = t * segCount;

    int i = glm::clamp((int)floor(segT),0, segCount - 1);
    float localT = segT - i;

    const auto& s = segments[i];

    float tt = localT * localT;
    float ttt = tt * localT;

    float h00 = 2*ttt - 3*tt + 1;
    float h10 = ttt - 2*tt + localT;
    float h01 = -2*ttt + 3*tt;
    float h11 = ttt - tt;

    return h00*s.p0 + h10*s.m0 + h01*s.p1 + h11*s.m1;
}


glm::vec3 HermiteTrack::tangent(float t) const {
    if (segments.empty()) return glm::vec3(0);

    int segCount = (int)segments.size();
    float segT = t * segCount;

    int i = glm::clamp((int)floor(segT), 0, segCount - 1);
    float localT = segT - i;

    const auto& s = segments[i];

    float tt = localT * localT;

    float dh00 = 6*tt - 6*localT;
    float dh10 = 3*tt - 4*localT + 1;
    float dh01 = -6*tt + 6*localT;
    float dh11 = 3*tt - 2*localT;

    return glm::normalize(dh00*s.p0 + dh10*s.m0 + dh01*s.p1 + dh11*s.m1);

}

void HermiteTrack::fromControlPoints(const std::vector<glm::vec3>& controlPoints){
    segments.clear();
    if(controlPoints.size() < 2) return;

    int n = (int)controlPoints.size();

    for(int i = 0; i < n -1; ++i){
        glm::vec3 p0 = controlPoints[i];
        glm::vec3 p1 = controlPoints[i + 1];

        glm::vec3 pPrev = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
        glm::vec3 pNext = (i + 2 < n) ? controlPoints[i + 2] : controlPoints[i + 1];

        glm::vec3 m0 = 0.5f * (p1 - pPrev);
        glm::vec3 m1 = 0.5f * (pNext - p0);

        segments.push_back({p0,p1,m0,m1});
    }
}


