// WormholeGateway - C++ file
#ifndef WORMHOLE_HPP
#define WORMHOLE_HPP

#include <string>
#include "../include/physics.h"

class Wormhole {
private:
    Point3D entrance;
    Point3D exit;
    WormholePhysics physics;
    std::string name;
    bool active;
    float pulsePhase;
    float size;

public:
    Wormhole();
    Wormhole(const Point3D& ent, const Point3D& ext);
    Wormhole(const Point3D& ent, const Point3D& ext, double stability);
    ~Wormhole();

    Point3D getEntrance() const;
    Point3D getExit() const;
    WormholePhysics getPhysics() const;
    std::string getName() const;
    bool isActive() const;
    float getPulsePhase() const;
    float getSize() const;

    void setEntrance(const Point3D& ent);
    void setExit(const Point3D& ext);
    void setStability(double stability);
    void setName(const std::string& newName);
    void setActive(bool status);
    void setSize(float newSize);

    void updatePhysics(double deltaTime);
    void updateVisuals(float deltaTime);
    double getTravelTime(const Point3D& from, const Point3D& to) const;
    bool isTraversable() const;
    double distanceBetweenPoints() const;
    void printInfo() const;
    void stabilize(double energy);
};

#endif