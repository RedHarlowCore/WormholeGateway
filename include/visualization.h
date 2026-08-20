/* WormholeGateway - Visualization helpers. */
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "physics.h"

typedef struct {
	int x;
	int y;
	int visible;
} ScreenPoint;

ScreenPoint projectPoint(const Point3D* point, int width, int height,
						 double scale);
double stabilityToBrightness(double stability);
char stabilityToSymbol(double stability);
void formatWormholeSummary(char* buffer, size_t bufferSize,
						   const char* name, const WormholePhysics* physics,
						   double distance, int active);

#ifdef __cplusplus
}
#endif

#endif
/* WormholeGateway - C file */
