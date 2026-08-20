/* WormholeGateway - C file */
/*
 * WormholeGateway - Utilities Header
 * Core helper functions for memory, math, and configuration.
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "physics.h"

typedef struct {
    double deltaTime;
    double totalTime;
    int maxWormholes;
    double stabilityDecay;
    double energyCost;
} SimulationConfig;

double randomDouble(double min, double max);
void randomPoint3D(Point3D* point, double minCoord, double maxCoord);
double clamp(double value, double min, double max);
double lerp(double a, double b, double t);
double smoothStep(double edge0, double edge1, double x);
double getCurrentTime(void);
void sleepMilliseconds(int milliseconds);
int readConfig(const char* filename, SimulationConfig* config);
int writeConfig(const char* filename, const SimulationConfig* config);
void* safeMalloc(size_t size);
void safeFree(void** ptr);
void safeStringCopy(char* dest, const char* src, size_t destSize);
void initSimulationConfig(SimulationConfig* config);
int isValidConfig(const SimulationConfig* config);
void randomWormholePhysics(WormholePhysics* physics);
void formatDouble(char* buffer, size_t bufferSize, double value, int precision);

#ifdef __cplusplus
}
#endif

#endif