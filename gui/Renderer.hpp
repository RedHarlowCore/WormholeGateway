// WormholeGateway - Native desktop renderer.
#ifndef WORMHOLE_RENDERER_HPP
#define WORMHOLE_RENDERER_HPP

#include "../src/Wormhole.hpp"
#include <cstddef>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

struct WormholeSnapshot {
	std::string name;
	Point3D entrance;
	Point3D exit;
	WormholePhysics physics;
	double distance;
	double travelTime;
	bool active;
	float pulsePhase;
	float size;
};

class Renderer {
public:
	Renderer();
	Renderer(int width, int height);

	bool initialize();
	void shutdown();
	void clear();
	void drawWormhole(const Wormhole& wormhole);
	void drawStatistics(std::size_t total, std::size_t active,
						double averageStability, double elapsedTime);
	void present();
	bool processEvents();
	bool isInitialized() const;
	bool isRunning() const;
	bool isPaused() const;
	int getWidth() const;
	int getHeight() const;

private:
	int width;
	int height;
	bool initialized;
	bool running;
	bool paused;
	std::vector<WormholeSnapshot> snapshots;
	std::size_t total;
	std::size_t active;
	double averageStability;
	double elapsedTime;
	double animationPhase;
	double framesPerSecond;
	unsigned long long frameCounter;
#ifdef _WIN32
	unsigned long long fpsStartTime;
#endif

#ifdef _WIN32
	HWND windowHandle;
	static LRESULT CALLBACK windowProcedure(HWND window, UINT message,
			WPARAM wParam, LPARAM lParam);
	void paintWindow(HDC deviceContext);
#else
	std::vector<std::string> frame;
#endif
};

#endif
// WormholeGateway - C++ file
