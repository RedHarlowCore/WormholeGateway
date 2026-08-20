/* WormholeGateway - C file */
/*
 * WormholeGateway - Physics Engine Header
 * Professional physics calculations for wormhole dynamics.
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double x, y, z;
} Point3D;

typedef struct {
    double curvature;
    double stability;
    double energy;
    double warpFactor;
    double throatRadius;
} WormholePhysics;

double calculateDistance(const Point3D* p1, const Point3D* p2);
double calculateCurvature(double mass, double distance);
double calculateStability(double initialStability, double time, double energy);
double calculateWarpFactor(double distance, double curvature, double stability);
double calculateEnergyRequired(double mass, double curvature, double stability);
int isTraversable(const WormholePhysics* physics);
void simulatePhysicsStep(WormholePhysics* physics, double deltaTime);
void calculateGravitationalInfluence(const Point3D* wormholePos, double mass, Point3D* objectPos, Point3D* force);
void initWormholePhysics(WormholePhysics* physics);
void copyWormholePhysics(WormholePhysics* dest, const WormholePhysics* src);
int compareWormholePhysics(const WormholePhysics* p1, const WormholePhysics* p2);

#ifdef __cplusplus
}
#endif

#endif