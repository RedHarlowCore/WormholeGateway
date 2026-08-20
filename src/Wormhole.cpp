/* WormholeGateway - Wormhole domain model. */

#include "Wormhole.hpp"
#include "../include/utils.h"
#include <cmath>
#include <cstdio>

static const double SIMULATED_WORMHOLE_MASS = 1.0e28;

Wormhole::Wormhole()
    : entrance{0.0, 0.0, 0.0}, exit{0.0, 0.0, 0.0},
      name("Unnamed wormhole"), active(false), pulsePhase(0.0f), size(1.0f) {
    initWormholePhysics(&physics);
}

Wormhole::Wormhole(const Point3D& ent, const Point3D& ext) : Wormhole() {
    entrance = ent;
    exit = ext;
    physics.curvature = calculateCurvature(
        SIMULATED_WORMHOLE_MASS, distanceBetweenPoints());
    physics.warpFactor = calculateWarpFactor(
        distanceBetweenPoints(), physics.curvature, physics.stability);
    active = isTraversable();
}

Wormhole::Wormhole(const Point3D& ent, const Point3D& ext, double stability)
    : Wormhole(ent, ext) {
    setStability(stability);
}

Wormhole::~Wormhole() = default;

Point3D Wormhole::getEntrance() const { return entrance; }
Point3D Wormhole::getExit() const { return exit; }
WormholePhysics Wormhole::getPhysics() const { return physics; }
std::string Wormhole::getName() const { return name; }
bool Wormhole::isActive() const { return active; }
float Wormhole::getPulsePhase() const { return pulsePhase; }
float Wormhole::getSize() const { return size; }

void Wormhole::setEntrance(const Point3D& ent) {
    entrance = ent;
    active = isTraversable();
}

void Wormhole::setExit(const Point3D& ext) {
    exit = ext;
    active = isTraversable();
}

void Wormhole::setStability(double stability) {
    physics.stability = clamp(stability, 0.0, 1.0);
    physics.warpFactor = calculateWarpFactor(
        distanceBetweenPoints(), physics.curvature, physics.stability);
    active = isTraversable();
}

void Wormhole::setName(const std::string& newName) { name = newName; }
void Wormhole::setActive(bool status) { active = status && isTraversable(); }

void Wormhole::setSize(float newSize) {
    size = newSize < 0.1f ? 0.1f : newSize;
    physics.throatRadius = static_cast<double>(size);
}

void Wormhole::updatePhysics(double deltaTime) {
    if (deltaTime <= 0.0) return;
    simulatePhysicsStep(&physics, deltaTime);
    active = isTraversable();
}

void Wormhole::updateVisuals(float deltaTime) {
    if (deltaTime <= 0.0f) return;
    pulsePhase = std::fmod(pulsePhase + deltaTime, 6.28318530718f);
    if (pulsePhase < 0.0f) pulsePhase += 6.28318530718f;
    size = 1.0f + 0.15f * std::sin(pulsePhase);
}

double Wormhole::getTravelTime(const Point3D& from, const Point3D& to) const {
    if (!isTraversable()) return -1.0;
    const double distance = calculateDistance(&from, &to);
    const double effectiveSpeed = 299792458.0 * physics.warpFactor;
    return effectiveSpeed > 0.0 ? distance / effectiveSpeed : -1.0;
}

bool Wormhole::isTraversable() const {
    return ::isTraversable(&physics) && distanceBetweenPoints() > 0.0;
}

double Wormhole::distanceBetweenPoints() const {
    return calculateDistance(&entrance, &exit);
}

void Wormhole::printInfo() const {
    std::printf("%s: distance=%.2f, stability=%.3f, active=%s\n",
                name.c_str(), distanceBetweenPoints(), physics.stability,
                active ? "yes" : "no");
}

void Wormhole::stabilize(double energy) {
    if (energy <= 0.0) return;
    physics.energy += energy;
    physics.stability = clamp(physics.stability + energy / 10000.0, 0.0, 1.0);
    physics.warpFactor = calculateWarpFactor(
        distanceBetweenPoints(), physics.curvature, physics.stability);
    active = isTraversable();
}