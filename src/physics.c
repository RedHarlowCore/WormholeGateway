/* WormholeGateway - C file */
/*
 * WormholeGateway - Physics Implementation
 * All core physics calculations for wormhole mechanics.
 */

#include "../include/physics.h"
#include "../include/utils.h"
#include <math.h>
#include <string.h>

#define GRAVITATIONAL_CONSTANT 6.674e-11
#define SPEED_OF_LIGHT 3.0e8

double calculateDistance(const Point3D* p1, const Point3D* p2) {
    if (!p1 || !p2) return 0.0;
    double dx = p1->x - p2->x;
    double dy = p1->y - p2->y;
    double dz = p1->z - p2->z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

double calculateCurvature(double mass, double distance) {
    if (distance <= 0.0) return 0.0;
    double rs = (2.0 * GRAVITATIONAL_CONSTANT * mass) / (SPEED_OF_LIGHT * SPEED_OF_LIGHT);
    return clamp(rs / distance, 0.0, 1.0);
}

double calculateStability(double initialStability, double time, double energy) {
    if (initialStability <= 0.0) return 0.0;
    double decayFactor = exp(-0.01 * time);
    double energyFactor = energy / (1.0 + energy);
    double stability = initialStability * decayFactor + (1.0 - decayFactor) * energyFactor;
    stability += randomDouble(-0.02, 0.02);
    return clamp(stability, 0.0, 1.0);
}

double calculateWarpFactor(double distance, double curvature, double stability) {
    if (distance <= 0.0 || stability <= 0.0) return 0.0;
    double baseFactor = 1.0 / (1.0 + distance * curvature * 0.01);
    return clamp(baseFactor * stability * stability, 0.0, 1.0);
}

double calculateEnergyRequired(double mass, double curvature, double stability) {
    if (stability <= 0.0 || curvature <= 0.0) return 0.0;
    return (mass * curvature * 1e10) + ((1.0 - stability) * 1e9) + (mass * 1e8);
}

int isTraversable(const WormholePhysics* physics) {
    if (!physics) return 0;
    return (physics->stability >= 0.5 &&
            physics->curvature >= 0.01 &&
            physics->curvature <= 1.0 &&
            physics->energy > 0.0 &&
            physics->throatRadius >= 1.0 &&
            physics->warpFactor >= 0.1);
}

void simulatePhysicsStep(WormholePhysics* physics, double deltaTime) {
    if (!physics || deltaTime <= 0.0) return;

    double decayRate = 0.001 * deltaTime;
    physics->stability = clamp(physics->stability - (physics->stability * decayRate), 0.0, 1.0);

    double energyChange = ((physics->stability - 0.5) * 8.0 -
                           (1.0 - physics->stability) * 3.0) * deltaTime;
    energyChange += randomDouble(-0.8, 0.8) * deltaTime;
    physics->energy = clamp(physics->energy + energyChange, 0.0, 1000000.0);

    double curvatureDelta = (physics->stability - 0.5) * 0.1 * deltaTime;
    curvatureDelta += (physics->energy / 100000.0) * 0.01 * deltaTime;
    physics->curvature = clamp(physics->curvature + curvatureDelta, 0.0, 1.0);

    physics->throatRadius = clamp(
        physics->throatRadius + (physics->stability - 0.5) * 0.5 * deltaTime,
        0.5, 10.0
    );

    physics->warpFactor = calculateWarpFactor(100.0, physics->curvature, physics->stability);

    physics->stability += randomDouble(-0.005, 0.005);
    physics->stability = clamp(physics->stability, 0.0, 1.0);
    physics->curvature += randomDouble(-0.005, 0.005);
    physics->curvature = clamp(physics->curvature, 0.0, 1.0);
}

void calculateGravitationalInfluence(const Point3D* wormholePos, double mass, Point3D* objectPos, Point3D* force) {
    if (!wormholePos || !objectPos || !force) return;
    double dist = calculateDistance(wormholePos, objectPos);
    if (dist <= 0.0) { force->x = 0.0; force->y = 0.0; force->z = 0.0; return; }
    double mag = (GRAVITATIONAL_CONSTANT * mass) / (dist * dist + 1.0);
    double inv = 1.0 / dist;
    force->x = mag * (objectPos->x - wormholePos->x) * inv;
    force->y = mag * (objectPos->y - wormholePos->y) * inv;
    force->z = mag * (objectPos->z - wormholePos->z) * inv;
}

void initWormholePhysics(WormholePhysics* physics) {
    if (!physics) return;
    physics->curvature = 0.5;
    physics->stability = 0.8;
    physics->energy = 1000.0;
    physics->warpFactor = 0.5;
    physics->throatRadius = 2.0;
}

void copyWormholePhysics(WormholePhysics* dest, const WormholePhysics* src) {
    if (dest && src) memcpy(dest, src, sizeof(WormholePhysics));
}

int compareWormholePhysics(const WormholePhysics* p1, const WormholePhysics* p2) {
    if (!p1 || !p2) return 0;
    double tol = 0.001;
    if (fabs(p1->curvature - p2->curvature) > tol) return 0;
    if (fabs(p1->stability - p2->stability) > tol) return 0;
    if (fabs(p1->energy - p2->energy) > tol * 1000) return 0;
    if (fabs(p1->warpFactor - p2->warpFactor) > tol) return 0;
    if (fabs(p1->throatRadius - p2->throatRadius) > tol) return 0;
    return 1;
}