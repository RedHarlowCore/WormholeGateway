// WormholeGateway - GUI lifecycle and simulation view.
#ifndef WORMHOLE_GUI_MANAGER_HPP
#define WORMHOLE_GUI_MANAGER_HPP

#include "Renderer.hpp"
#include <cstddef>
#include <vector>

class GuiManager {
public:
	GuiManager();
	explicit GuiManager(Renderer renderer);

	bool initialize();
	void shutdown();
	void update(std::vector<Wormhole>& wormholes, double deltaTime);
	void render(const std::vector<Wormhole>& wormholes);
	bool isInitialized() const;
	bool isRunning() const;
	bool isPaused() const;
	const Renderer& getRenderer() const;

private:
	Renderer renderer;
	double elapsedTime;
};

#endif
// WormholeGateway - C++ file
