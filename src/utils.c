/* WormholeGateway - C file */
/*
 * WormholeGateway - Utilities Implementation
 * Random generation, math helpers, configuration I/O, and memory safety.
 */

#include "../include/utils.h"
#include "../include/physics.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

double randomDouble(double min, double max) {
    if (min > max) { double t = min; min = max; max = t; }
    return min + ((double)rand() / (double)RAND_MAX) * (max - min);
}

void randomPoint3D(Point3D* point, double minCoord, double maxCoord) {
    if (!point) return;
    point->x = randomDouble(minCoord, maxCoord);
    point->y = randomDouble(minCoord, maxCoord);
    point->z = randomDouble(minCoord, maxCoord);
}

double clamp(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

double lerp(double a, double b, double t) {
    return a + clamp(t, 0.0, 1.0) * (b - a);
}

double smoothStep(double edge0, double edge1, double x) {
    if (edge0 == edge1) return x < edge0 ? 0.0 : 1.0;
    double t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

double getCurrentTime(void) {
#ifdef _WIN32
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    unsigned __int64 t = ((unsigned __int64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (double)t / 10000000.0 - 11644473600.0;
#else
    struct timeval tv; gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
#endif
}

void sleepMilliseconds(int milliseconds) {
    if (milliseconds <= 0) return;
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

int readConfig(const char* filename, SimulationConfig* config) {
    if (!filename || !config) return -1;
    FILE* f = fopen(filename, "r");
    if (!f) return -1;
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char key[64]; double val;
        if (sscanf(line, "%63s = %lf", key, &val) == 2) {
            if (strcmp(key, "deltaTime") == 0) { config->deltaTime = val; count++; }
            else if (strcmp(key, "totalTime") == 0) { config->totalTime = val; count++; }
            else if (strcmp(key, "maxWormholes") == 0) { config->maxWormholes = (int)val; count++; }
            else if (strcmp(key, "stabilityDecay") == 0) { config->stabilityDecay = val; count++; }
            else if (strcmp(key, "energyCost") == 0) { config->energyCost = val; count++; }
        }
    }
    fclose(f);
    return count;
}

int writeConfig(const char* filename, const SimulationConfig* config) {
    if (!filename || !config) return -1;
    FILE* f = fopen(filename, "w");
    if (!f) return -1;
    fprintf(f, "# WormholeGateway Config\n");
    fprintf(f, "deltaTime = %.6f\n", config->deltaTime);
    fprintf(f, "totalTime = %.6f\n", config->totalTime);
    fprintf(f, "maxWormholes = %d\n", config->maxWormholes);
    fprintf(f, "stabilityDecay = %.6f\n", config->stabilityDecay);
    fprintf(f, "energyCost = %.6f\n", config->energyCost);
    fclose(f);
    return 0;
}

void* safeMalloc(size_t size) {
    if (size == 0) return NULL;
    void* ptr = malloc(size);
    if (!ptr) { fprintf(stderr, "FATAL: Memory allocation failed.\n"); exit(EXIT_FAILURE); }
    return ptr;
}

void safeFree(void** ptr) {
    if (ptr && *ptr) { free(*ptr); *ptr = NULL; }
}

void safeStringCopy(char* dest, const char* src, size_t destSize) {
    if (!dest || !src || destSize == 0) return;
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

void initSimulationConfig(SimulationConfig* config) {
    if (!config) return;
    config->deltaTime = 0.1;
    config->totalTime = 10.0;
    config->maxWormholes = 5;
    config->stabilityDecay = 0.01;
    config->energyCost = 100.0;
}

int isValidConfig(const SimulationConfig* config) {
    if (!config) return 0;
    return (config->deltaTime > 0.0 && config->totalTime > 0.0 &&
            config->maxWormholes > 0 && config->stabilityDecay >= 0.0 &&
            config->energyCost >= 0.0);
}

void randomWormholePhysics(WormholePhysics* physics) {
    if (!physics) return;
    physics->curvature = randomDouble(0.1, 0.9);
    physics->stability = randomDouble(0.3, 0.95);
    physics->energy = randomDouble(500.0, 5000.0);
    physics->warpFactor = randomDouble(0.1, 0.8);
    physics->throatRadius = randomDouble(0.5, 5.0);
}

void formatDouble(char* buffer, size_t bufferSize, double value, int precision) {
    if (!buffer || bufferSize == 0) return;
    snprintf(buffer, bufferSize, "%.*f", precision, value);
}