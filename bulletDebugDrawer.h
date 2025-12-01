#ifndef BULLETDEBUGDRAWER_H
#define BULLETDEBUGDRAWER_H
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

struct DebugLine {
    glm::vec3 from;
    glm::vec3 to;
    glm::vec3 color;
};




class BulletDebugDrawer : public btIDebugDraw
{
public:
    int m_debugMode;
    std::vector<DebugLine> lines;

    BulletDebugDrawer() : m_debugMode(DBG_DrawWireframe) {}

    virtual void drawLine(
        const btVector3& from,
        const btVector3& to,
        const btVector3& color) override
    {
        // std::cout << "LINE: "
        //           << from.x() << "," << from.y() << "," << from.z()
        //           << " -> "
        //           << to.x() << "," << to.y() << "," << to.z()
        //           << "\n";


        DebugLine l;
        l.from = glm::vec3(from.x(), from.y(), from.z());
        l.to   = glm::vec3(to.x(), to.y(), to.z());
        l.color= glm::vec3(color.x(), color.y(), color.z());
        lines.push_back(l);
    }

    virtual void drawContactPoint(
        const btVector3& PointOnB,
        const btVector3& normalOnB,
        btScalar distance,
        int lifeTime,
        const btVector3& color) override
    {
        // // Draw a little line for contact point + normal
        // lines.push_back(glm::vec3(PointOnB.x(), PointOnB.y(), PointOnB.z()));
        // glm::vec3 end = glm::vec3(PointOnB.x() + normalOnB.x() * 0.3f,
        //                           PointOnB.y() + normalOnB.y() * 0.3f,
        //                           PointOnB.z() + normalOnB.z() * 0.3f);
        // lines.push_back(end);
    }

    virtual void reportErrorWarning(const char* warningString) override
    {
        std::cout << "[Bullet Warning] " << warningString << "\n";
    }

    virtual void draw3dText(const btVector3& location, const char* textString) override
    {
        // Not needed
    }

    virtual void setDebugMode(int debugMode) override
    {
        m_debugMode = debugMode;
    }

    virtual int getDebugMode() const override
    {
        return m_debugMode;
    }

    void clear()
    {
        lines.clear();
    }
};
#endif // BULLETDEBUGDRAWER_H
