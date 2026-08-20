/*
 * WormholeGateway - Main Entry Point
 * Professional simulation engine for wormhole dynamics.
 */

#include "Wormhole.hpp"
#include "../include/physics.h"
#include "../include/logger.h"   
#include "../include/utils.h"
#include "../gui/GuiManager.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static vector<Wormhole> g_wormholes;
static SimulationConfig g_config;
static GuiManager g_gui;

static void initSimulation();
static void runSimulation();
static void shutdownSimulation();

static int applicationMain() {
    srand(static_cast<unsigned>(time(nullptr)));

    log_info("WormholeGateway v2.0 starting...");
    
    initSimulation();
    runSimulation();
    shutdownSimulation();

    log_info("Simulation terminated normally.");
    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return applicationMain();
}
#else
int main() {
    return applicationMain();
}
#endif

void initSimulation() {
    initSimulationConfig(&g_config);

    if (!g_gui.initialize()) {
        log_error("GUI initialization failed.");
    }

    g_config.maxWormholes = 12;
    log_info("Configuration loaded: dt=%.3f, total=%.1fs, maxWormholes=%d",
                 g_config.deltaTime, g_config.totalTime, g_config.maxWormholes);

    for (int i = 0; i < g_config.maxWormholes; ++i) {
        Point3D entrance, exit;
        const double angle = (2.0 * 3.14159265359 * i) /
            static_cast<double>(g_config.maxWormholes);
        const double radius = 72.0 + (i % 3) * 6.0;
        entrance.x = std::cos(angle) * radius;
        entrance.y = std::sin(angle) * radius;
        entrance.z = -35.0 + (i % 4) * 23.0;
        exit.x = std::cos(angle + 3.14159265359) * radius;
        exit.y = std::sin(angle + 3.14159265359) * radius;
        exit.z = 35.0 - (i % 4) * 19.0;

        Wormhole w(entrance, exit);
        w.setName("Wormhole-" + to_string(i + 1));
        w.setStability(randomDouble(0.42, 0.98));
        w.setSize(static_cast<float>(randomDouble(1.2, 3.8)));

        g_wormholes.push_back(w);

        log_info("Created %s: entrance(%.2f,%.2f,%.2f) exit(%.2f,%.2f,%.2f) stability=%.3f",
                     w.getName().c_str(),
                     entrance.x, entrance.y, entrance.z,
                     exit.x, exit.y, exit.z,
                     w.getPhysics().stability);
    }
}

void runSimulation() {
    using Clock = std::chrono::steady_clock;
    const auto targetFrame = std::chrono::microseconds(6944);
    auto previousFrame = Clock::now();
    double accumulator = 0.0;
    double simulationTime = 0.0;
    int step = 0;

    while (g_gui.isInitialized() ? g_gui.isRunning()
                                 : simulationTime < g_config.totalTime) {
        const auto frameStart = Clock::now();
        double frameDelta = std::chrono::duration<double>(
            frameStart - previousFrame).count();
        previousFrame = frameStart;
        if (frameDelta > 0.1) frameDelta = 0.1;

        if (!g_gui.isPaused() && simulationTime < g_config.totalTime) {
            accumulator += frameDelta;
            while (accumulator >= g_config.deltaTime &&
                   simulationTime < g_config.totalTime) {
                for (auto& wormhole : g_wormholes) {
                    wormhole.updatePhysics(g_config.deltaTime);
                }
                simulationTime += g_config.deltaTime;
                accumulator -= g_config.deltaTime;
                ++step;

                if (step % 10 == 0) {
                    int active = 0;
                    double averageStability = 0.0;
                    for (const auto& wormhole : g_wormholes) {
                        if (wormhole.isActive()) ++active;
                        averageStability += wormhole.getPhysics().stability;
                    }
                    if (!g_wormholes.empty()) {
                        averageStability /= g_wormholes.size();
                    }
                    log_info("Step %d: active=%d, avg_stability=%.3f",
                             step, active, averageStability);
                }
            }
        }

        g_gui.update(g_wormholes, frameDelta);
        g_gui.render(g_wormholes);

        const auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - frameStart);
        if (frameTime < targetFrame) {
            std::this_thread::sleep_for(targetFrame - frameTime);
        }
    }
}

void shutdownSimulation() {
    g_gui.shutdown();
    g_wormholes.clear();
    log_info("Resources released.");
}