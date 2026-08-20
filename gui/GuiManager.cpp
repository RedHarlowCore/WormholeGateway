// WormholeGateway - GUI lifecycle and simulation view.

#include "GuiManager.hpp"
#include <utility>

GuiManager::GuiManager() : renderer(), elapsedTime(0.0) {}

GuiManager::GuiManager(Renderer guiRenderer)
	: renderer(std::move(guiRenderer)), elapsedTime(0.0) {}

bool GuiManager::initialize() {
	elapsedTime = 0.0;
	return renderer.initialize();
}

void GuiManager::shutdown() {
	renderer.shutdown();
	elapsedTime = 0.0;
}

void GuiManager::update(std::vector<Wormhole>& wormholes, double deltaTime) {
	if (!isInitialized() || deltaTime <= 0.0) return;
	elapsedTime += deltaTime;
	for (Wormhole& wormhole : wormholes)
		wormhole.updateVisuals(static_cast<float>(deltaTime));
}

void GuiManager::render(const std::vector<Wormhole>& wormholes) {
	if (!isInitialized()) return;
	std::size_t active = 0;
	double stability = 0.0;
	renderer.clear();
	for (const Wormhole& wormhole : wormholes) {
		renderer.drawWormhole(wormhole);
		if (wormhole.isActive()) ++active;
		stability += wormhole.getPhysics().stability;
	}
	const double average = wormholes.empty() ? 0.0 : stability / wormholes.size();
	renderer.drawStatistics(wormholes.size(), active, average, elapsedTime);
	renderer.present();
}

bool GuiManager::isInitialized() const { return renderer.isInitialized(); }
bool GuiManager::isRunning() const { return renderer.isRunning(); }
bool GuiManager::isPaused() const { return renderer.isPaused(); }
const Renderer& GuiManager::getRenderer() const { return renderer; }
// WormholeGateway - C++ file
