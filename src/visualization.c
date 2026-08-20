/* WormholeGateway - Visualization helpers. */

#include "../include/visualization.h"
#include <stdio.h>

ScreenPoint projectPoint(const Point3D* point, int width, int height,
						 double scale) {
	ScreenPoint result = {0, 0, 0};
	if (!point || width <= 0 || height <= 0 || scale <= 0.0) return result;

	result.x = (int)((point->x / scale + 0.5) * (double)(width - 1));
	result.y = (int)((0.5 - point->y / scale) * (double)(height - 1));
	result.visible = result.x >= 0 && result.x < width &&
					result.y >= 0 && result.y < height;
	return result;
}

double stabilityToBrightness(double stability) {
	if (stability < 0.0) return 0.0;
	if (stability > 1.0) return 1.0;
	return stability;
}

char stabilityToSymbol(double stability) {
	const double brightness = stabilityToBrightness(stability);
	if (brightness >= 0.8) return '#';
	if (brightness >= 0.6) return 'O';
	if (brightness >= 0.4) return 'o';
	if (brightness >= 0.2) return '.';
	return ' ';
}

void formatWormholeSummary(char* buffer, size_t bufferSize,
						   const char* name, const WormholePhysics* physics,
						   double distance, int active) {
	if (!buffer || bufferSize == 0) return;
	if (!name) name = "Unnamed wormhole";
	if (!physics) {
		snprintf(buffer, bufferSize, "%s: unavailable", name);
		return;
	}
	snprintf(buffer, bufferSize,
			 "%s | %s | distance %.2f | stability %.3f | warp %.3f",
			 name, active ? "ACTIVE" : "INACTIVE", distance,
			 physics->stability, physics->warpFactor);
}
/* WormholeGateway - C file */
