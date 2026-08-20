// WormholeGateway - Native desktop renderer.

#include "Renderer.hpp"
#include "../include/visualization.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <cmath>
#include <mmsystem.h>
#endif

Renderer::Renderer() : Renderer(72, 20) {}

Renderer::Renderer(int rendererWidth, int rendererHeight)
	: width(std::max(20, rendererWidth)),
	  height(std::max(8, rendererHeight)),
	  initialized(false), running(false), paused(false), total(0), active(0),
	  averageStability(0.0), elapsedTime(0.0), animationPhase(0.0),
	  framesPerSecond(0.0), frameCounter(0)
#ifdef _WIN32
	, fpsStartTime(0), windowHandle(nullptr)
#endif
{}

bool Renderer::initialize() {
	#ifdef _WIN32
	const char* className = "WormholeGatewayWindow";
	HINSTANCE instance = GetModuleHandleA(nullptr);
	WNDCLASSA windowClass{};
	windowClass.hInstance = instance;
	windowClass.lpfnWndProc = Renderer::windowProcedure;
	windowClass.lpszClassName = className;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = nullptr;
	RegisterClassA(&windowClass);
	windowHandle = CreateWindowExA(0, className, "Wormhole Gateway - Spacetime Simulation",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1152, 720,
		nullptr, nullptr, instance, this);
	if (!windowHandle) return false;
	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);
	timeBeginPeriod(1);
	initialized = true;
	running = true;
	paused = false;
	#ifdef _WIN32
	fpsStartTime = GetTickCount64();
	#endif
	return true;
	#else
	frame.assign(static_cast<std::size_t>(height),
				 std::string(static_cast<std::size_t>(width), ' '));
	initialized = true;
	running = true;
	paused = false;
	return true;
	#endif
}

void Renderer::shutdown() {
	#ifdef _WIN32
	if (windowHandle) {
		DestroyWindow(windowHandle);
		windowHandle = nullptr;
	}
	timeEndPeriod(1);
	#else
	frame.clear();
	#endif
	initialized = false;
	running = false;
	paused = false;
}

void Renderer::clear() {
	if (!initialized) return;
	#ifdef _WIN32
	snapshots.clear();
	#else
	for (std::string& row : frame) row.assign(static_cast<std::size_t>(width), ' ');
	#endif
}

void Renderer::drawWormhole(const Wormhole& wormhole) {
	if (!initialized) return;
	#ifdef _WIN32
	WormholeSnapshot snapshot{
		wormhole.getName(), wormhole.getEntrance(), wormhole.getExit(),
		wormhole.getPhysics(), wormhole.distanceBetweenPoints(),
		wormhole.getTravelTime(wormhole.getEntrance(), wormhole.getExit()),
		wormhole.isActive(), wormhole.getPulsePhase(), wormhole.getSize()};
	snapshots.push_back(snapshot);
	#else
	const Point3D entrance = wormhole.getEntrance();
	const ScreenPoint point = projectPoint(&entrance, width, height, 200.0);
	if (!point.visible) return;
	frame[static_cast<std::size_t>(point.y)][static_cast<std::size_t>(point.x)] =
		stabilityToSymbol(wormhole.getPhysics().stability);
	#endif
}

void Renderer::drawStatistics(std::size_t total, std::size_t active,
							  double averageStability, double elapsedTime) {
	if (!initialized) return;
	char summary[128];
	std::snprintf(summary, sizeof(summary),
				  "WormholeGateway | total %zu | active %zu | stability %.3f | time %.1fs",
				  total, active, averageStability, elapsedTime);
	this->total = total;
	this->active = active;
	this->averageStability = averageStability;
	this->elapsedTime = elapsedTime;
	#ifdef _WIN32
	return;
	#else
	frame[0].replace(0, std::min(frame[0].size(), std::strlen(summary)), summary);
	#endif
}

void Renderer::present() {
	if (!initialized) return;
	#ifdef _WIN32
	const unsigned long long now = GetTickCount64();
	++frameCounter;
	if (now - fpsStartTime >= 500) {
		const double measuredFps = static_cast<double>(frameCounter) * 1000.0 /
			static_cast<double>(now - fpsStartTime);
		framesPerSecond = framesPerSecond == 0.0 ? measuredFps :
			framesPerSecond * 0.75 + measuredFps * 0.25;
		frameCounter = 0;
		fpsStartTime = now;
	}
	animationPhase += 0.12;
	InvalidateRect(windowHandle, nullptr, FALSE);
	UpdateWindow(windowHandle);
	processEvents();
	#else
	std::cout << "\n--- Wormhole Gateway ---\n";
	for (const std::string& row : frame) std::cout << row << '\n';
	std::cout.flush();
	#endif
}

bool Renderer::processEvents() {
	#ifdef _WIN32
	MSG message;
	while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) {
			running = false;
			return false;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return running;
	#else
	return running;
	#endif
}

bool Renderer::isInitialized() const { return initialized; }
bool Renderer::isRunning() const { return running; }
bool Renderer::isPaused() const { return paused; }
int Renderer::getWidth() const { return width; }
int Renderer::getHeight() const { return height; }

#ifdef _WIN32
static int mapCoordinate(double value, int start, int size) {
	const double normalized = (value + 100.0) / 200.0;
	return start + static_cast<int>(normalized * size);
}

static COLORREF stabilityColor(double stability) {
	if (stability >= 0.75) return RGB(55, 220, 150);
	if (stability >= 0.5) return RGB(245, 190, 70);
	return RGB(235, 80, 90);
}

static COLORREF mixColor(COLORREF first, COLORREF second, double amount) {
	amount = std::max(0.0, std::min(1.0, amount));
	const int red = static_cast<int>(GetRValue(first) * (1.0 - amount) +
		GetRValue(second) * amount);
	const int green = static_cast<int>(GetGValue(first) * (1.0 - amount) +
		GetGValue(second) * amount);
	const int blue = static_cast<int>(GetBValue(first) * (1.0 - amount) +
		GetBValue(second) * amount);
	return RGB(red, green, blue);
}

static void drawText(HDC dc, int x, int y, const std::string& text,
		COLORREF color, int size = 16) {
	static HFONT fontCache[48]{};
	const int fontIndex = std::max(1, std::min(47, size));
	if (!fontCache[fontIndex]) {
		fontCache[fontIndex] = CreateFontA(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
	}
	SetTextColor(dc, color);
	SetBkMode(dc, TRANSPARENT);
	HGDIOBJ oldFont = SelectObject(dc, fontCache[fontIndex]);
	TextOutA(dc, x, y, text.c_str(), static_cast<int>(text.size()));
	SelectObject(dc, oldFont);
}

static void drawSpaceBackground(HDC dc, const RECT& client, double phase) {
	const int height = std::max(1, static_cast<int>(client.bottom));
	for (int y = 0; y < height; y += 4) {
		const double position = static_cast<double>(y) / height;
		const int red = static_cast<int>(1.0 + 3.0 * position);
		const int green = static_cast<int>(2.0 + 5.0 * position);
		const int blue = static_cast<int>(7.0 + 12.0 * position);
		RECT band{client.left, y, client.right, std::min(y + 4, height)};
		HBRUSH brush = CreateSolidBrush(RGB(red, green, blue));
		FillRect(dc, &band, brush);
		DeleteObject(brush);
	}

	const int clouds[5][4] = {
		{150, 95, 230, 120}, {740, 170, 280, 135},
		{1240, 85, 320, 150}, {520, 720, 360, 120},
		{1420, 740, 300, 110}
	};
	const COLORREF cloudColors[5] = {
		RGB(18, 32, 72), RGB(24, 38, 82), RGB(36, 26, 70),
		RGB(20, 45, 72), RGB(42, 30, 70)
	};
	for (int cloud = 0; cloud < 5; ++cloud) {
		for (int layer = 4; layer >= 0; --layer) {
			const int width = clouds[cloud][2] + layer * 70;
			const int height = clouds[cloud][3] + layer * 36;
			HBRUSH brush = CreateSolidBrush(mixColor(
				RGB(1, 3, 10), cloudColors[cloud], 0.16 - layer * 0.018));
			HGDIOBJ oldBrush = SelectObject(dc, brush);
			Ellipse(dc, clouds[cloud][0] - width / 2, clouds[cloud][1] - height / 2,
				clouds[cloud][0] + width / 2, clouds[cloud][1] + height / 2);
			SelectObject(dc, oldBrush);
			DeleteObject(brush);
		}
	}

	for (int star = 0; star < 420; ++star) {
		const int x = 12 + (star * 1571 + star * star * 13) %
			std::max(20, static_cast<int>(client.right) - 24);
		const int y = 10 + (star * 911 + star * star * 7) %
			std::max(20, static_cast<int>(client.bottom) - 20);
		const double pulse = 0.55 + 0.45 * std::sin(
			static_cast<double>(star) * 1.73 + phase * 0.8);
		const int brightness = static_cast<int>(100.0 + 130.0 * pulse);
		const COLORREF color = (star % 13 == 0)
			? RGB(brightness, brightness - 15, brightness - 35)
			: RGB(brightness, brightness, std::min(255, brightness + 10));
		const int radius = star % 29 == 0 ? 2 : 1;
		if (radius == 1) {
			SetPixelV(dc, x, y, color);
			continue;
		}
		HBRUSH brush = CreateSolidBrush(color);
		HGDIOBJ oldBrush = SelectObject(dc, brush);
		Ellipse(dc, x - radius, y - radius, x + radius + 1, y + radius + 1);
		SelectObject(dc, oldBrush);
		DeleteObject(brush);
	}
}

void Renderer::paintWindow(HDC dc) {
	RECT client;
	GetClientRect(windowHandle, &client);
	drawSpaceBackground(dc, client, animationPhase);

	drawText(dc, 28, 20, "WORMHOLE GATEWAY", RGB(235, 245, 255), 25);
	drawText(dc, 30, 56, "SPACETIME TUNNEL MONITOR  /  LIVE SIMULATION",
		RGB(135, 165, 195), 14);

	char stats[220];
	std::snprintf(stats, sizeof(stats),
		"Gates %zu    Active %zu    Average stability %.1f%%    Simulated time %.1fs    FPS %.0f",
		total, active, averageStability * 100.0, elapsedTime, framesPerSecond);
	drawText(dc, 30, 84, stats, RGB(170, 205, 225), 15);
	drawText(dc, client.right - 235, 52, paused ? "[ SPACE ]  RESUME" : "[ SPACE ]  PAUSE",
		paused ? RGB(245, 190, 70) : RGB(75, 225, 165), 13);
	drawText(dc, 45, client.bottom - 27,
		"GREEN  ACTIVE     AMBER  UNSTABLE     RED  CLOSED     SPACE  PAUSE/RESUME",
		RGB(145, 170, 195), 12);

	const int mapLeft = 30;
	const int mapTop = 112;
	const int mapWidth = std::max(480, static_cast<int>(client.right) - 430);
	const int mapHeight = std::max(400, static_cast<int>(client.bottom) - 155);
	RECT mapRect{mapLeft, mapTop, mapLeft + mapWidth, mapTop + mapHeight};
	HBRUSH mapBrush = CreateSolidBrush(RGB(2, 6, 14));
	FillRect(dc, &mapRect, mapBrush);
	DeleteObject(mapBrush);

	drawText(dc, mapLeft + 18, mapTop + 14, "SPATIAL COORDINATE MAP  /  -100 TO +100",
		RGB(116, 153, 184), 12);
	for (int star = 0; star < 250; ++star) {
		const int starX = mapLeft + 18 + (star * 83) % std::max(1, mapWidth - 36);
		const int starY = mapTop + 18 + (star * 47) % std::max(1, mapHeight - 36);
		const double twinkle = 0.45 + 0.45 *
			std::sin(animationPhase * 0.8 + static_cast<double>(star));
		const int brightness = static_cast<int>(100.0 + 120.0 * twinkle);
		if (star % 11 != 0) {
			SetPixelV(dc, starX, starY, RGB(brightness, brightness,
				std::min(255, brightness + 10)));
			continue;
		}
		HBRUSH starBrush = CreateSolidBrush(RGB(brightness, brightness, brightness));
		HGDIOBJ oldStarBrush = SelectObject(dc, starBrush);
		Ellipse(dc, starX, starY, starX + 2, starY + 2);
		SelectObject(dc, oldStarBrush);
		DeleteObject(starBrush);
	}

	HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(14, 23, 36));
	HGDIOBJ oldPen = SelectObject(dc, gridPen);
	for (int x = 0; x <= 4; ++x) {
		const int px = mapLeft + x * mapWidth / 4;
		MoveToEx(dc, px, mapTop, nullptr);
		LineTo(dc, px, mapTop + mapHeight);
	}
	for (int y = 0; y <= 4; ++y) {
		const int py = mapTop + y * mapHeight / 4;
		MoveToEx(dc, mapLeft, py, nullptr);
		LineTo(dc, mapLeft + mapWidth, py);
	}
	SelectObject(dc, oldPen);
	DeleteObject(gridPen);

	for (const WormholeSnapshot& gate : snapshots) {
		const int entranceX = mapCoordinate(gate.entrance.x, mapLeft, mapWidth);
		const int entranceY = mapCoordinate(-gate.entrance.y, mapTop, mapHeight);
		const int exitX = mapCoordinate(gate.exit.x, mapLeft, mapWidth);
		const int exitY = mapCoordinate(-gate.exit.y, mapTop, mapHeight);
		const COLORREF color = stabilityColor(gate.physics.stability);
		HPEN linePen = CreatePen(PS_SOLID, gate.active ? 3 : 1,
			mixColor(RGB(35, 70, 100), color, 0.75));
		oldPen = SelectObject(dc, linePen);
		MoveToEx(dc, entranceX, entranceY, nullptr);
		const int perpendicularX = -(exitY - entranceY) / 10;
		const int perpendicularY = (exitX - entranceX) / 10;
		for (int segment = 1; segment <= 32; ++segment) {
			const double progress = static_cast<double>(segment) / 32.0;
			const double curve = std::sin(progress * 3.14159265359) * 0.35;
			const int x = entranceX + static_cast<int>((exitX - entranceX) * progress +
				perpendicularX * curve);
			const int y = entranceY + static_cast<int>((exitY - entranceY) * progress +
				perpendicularY * curve);
			LineTo(dc, x, y);
		}
		SelectObject(dc, oldPen);
		DeleteObject(linePen);
		if (gate.active) {
			for (int particle = 0; particle < 4; ++particle) {
				const double progress = std::fmod(animationPhase * 0.035 +
					gate.pulsePhase * 0.08 + particle * 0.25, 1.0);
				const double curve = std::sin(progress * 3.14159265359) * 0.35;
				const int movingX = entranceX + static_cast<int>((exitX - entranceX) * progress +
					perpendicularX * curve);
				const int movingY = entranceY + static_cast<int>((exitY - entranceY) * progress +
					perpendicularY * curve);
				HBRUSH pulseBrush = CreateSolidBrush(RGB(230, 250, 255));
				HGDIOBJ oldPulseBrush = SelectObject(dc, pulseBrush);
				Ellipse(dc, movingX - 4, movingY - 4, movingX + 4, movingY + 4);
				SelectObject(dc, oldPulseBrush);
				DeleteObject(pulseBrush);
			}
		}

		const int pulseRadius = 8 + static_cast<int>(
			4.0 * (1.0 + std::sin(animationPhase + gate.pulsePhase)));
		HBRUSH pointBrush = CreateSolidBrush(color);
		HGDIOBJ oldBrush = SelectObject(dc, pointBrush);
		Ellipse(dc, entranceX - pulseRadius, entranceY - pulseRadius,
			entranceX + pulseRadius, entranceY + pulseRadius);
		Ellipse(dc, exitX - pulseRadius, exitY - pulseRadius,
			exitX + pulseRadius, exitY + pulseRadius);
		SelectObject(dc, oldBrush);
		DeleteObject(pointBrush);
		HPEN ringPen = CreatePen(PS_SOLID, 1, mixColor(color, RGB(240, 250, 255),
			0.55 + 0.25 * std::sin(animationPhase)));
		oldPen = SelectObject(dc, ringPen);
		HGDIOBJ oldRingBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
		Ellipse(dc, entranceX - pulseRadius - 5, entranceY - pulseRadius - 5,
			entranceX + pulseRadius + 5, entranceY + pulseRadius + 5);
		Ellipse(dc, exitX - pulseRadius - 5, exitY - pulseRadius - 5,
			exitX + pulseRadius + 5, exitY + pulseRadius + 5);
		SelectObject(dc, oldRingBrush);
		SelectObject(dc, oldPen);
		DeleteObject(ringPen);
		drawText(dc, entranceX + 10, entranceY - 20, gate.name, color, 13);
	}

	const int panelLeft = mapLeft + mapWidth + 24;
	drawText(dc, panelLeft, 112, "GATE TELEMETRY / LIVE", RGB(235, 245, 255), 17);
	drawText(dc, panelLeft, 136, "ID      STATE      STABILITY   DISTANCE   WARP   ENERGY   TRAVEL",
		RGB(112, 153, 184), 11);
	const int rowTop = 153;
	const int rowHeight = 40;
	for (std::size_t index = 0; index < snapshots.size(); ++index) {
		const WormholeSnapshot& gate = snapshots[index];
		const int y = rowTop + static_cast<int>(index) * rowHeight;
		RECT row{panelLeft, y, static_cast<int>(client.right) - 24, y + rowHeight - 3};
		HBRUSH rowBrush = CreateSolidBrush(index % 2 == 0
			? RGB(18, 34, 54) : RGB(13, 27, 45));
		FillRect(dc, &row, rowBrush);
		DeleteObject(rowBrush);
		const COLORREF color = stabilityColor(gate.physics.stability);
		char detail[220];
		std::snprintf(detail, sizeof(detail), "W%02zu     %-8s    %5.1f%%      %7.2f   %.3f  %7.0f  %.2e",
			index + 1, gate.active ? "ACTIVE" : "CLOSED",
			gate.physics.stability * 100.0, gate.distance,
			gate.physics.warpFactor, gate.physics.energy, gate.travelTime);
		drawText(dc, panelLeft + 8, y + 7, detail, color, 10);
		const double flux = 0.5 + 0.5 * std::sin(animationPhase + gate.pulsePhase);
		std::snprintf(detail, sizeof(detail), "IN %.1f,%.1f,%.1f OUT %.1f,%.1f,%.1f F%.2f",
			gate.entrance.x, gate.entrance.y, gate.entrance.z,
			gate.exit.x, gate.exit.y, gate.exit.z, flux);
		drawText(dc, panelLeft + 8, y + 22, detail, RGB(125, 170, 205), 8);
		const int barWidth = 48;
		RECT bar{static_cast<int>(client.right) - 70, y + 31,
			static_cast<int>(client.right) - 22, y + 36};
		HBRUSH barBrush = CreateSolidBrush(RGB(7, 15, 25));
		FillRect(dc, &bar, barBrush);
		DeleteObject(barBrush);
		RECT fill = bar;
		fill.right = fill.left + static_cast<int>(barWidth *
			std::max(0.0, std::min(1.0, gate.physics.stability)));
		HBRUSH fillBrush = CreateSolidBrush(color);
		FillRect(dc, &fill, fillBrush);
		DeleteObject(fillBrush);
	}
	drawText(dc, panelLeft, rowTop + static_cast<int>(snapshots.size()) * rowHeight + 16,
		"Coordinates available on map endpoints / click-ready telemetry stream",
		RGB(105, 140, 170), 11);
}

LRESULT CALLBACK Renderer::windowProcedure(HWND window, UINT message,
		WPARAM wParam, LPARAM lParam) {
	Renderer* renderer = reinterpret_cast<Renderer*>(
		GetWindowLongPtr(window, GWLP_USERDATA));
	if (message == WM_NCCREATE) {
		const CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		renderer = static_cast<Renderer*>(create->lpCreateParams);
		SetWindowLongPtr(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(renderer));
		renderer->windowHandle = window;
	}
	if (message == WM_PAINT && renderer) {
		PAINTSTRUCT paint;
		HDC dc = BeginPaint(window, &paint);
		RECT client;
		GetClientRect(window, &client);
		const int bufferWidth = std::max(1, static_cast<int>(client.right));
		const int bufferHeight = std::max(1, static_cast<int>(client.bottom));
		HDC buffer = CreateCompatibleDC(dc);
		HBITMAP bitmap = CreateCompatibleBitmap(dc, bufferWidth, bufferHeight);
		HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);
		renderer->paintWindow(buffer);
		BitBlt(dc, 0, 0, bufferWidth, bufferHeight, buffer, 0, 0, SRCCOPY);
		SelectObject(buffer, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(buffer);
		EndPaint(window, &paint);
		return 0;
	}
	if (message == WM_ERASEBKGND) return 1;
	if (message == WM_CLOSE) {
		DestroyWindow(window);
		return 0;
	}
	if (message == WM_KEYDOWN && renderer && wParam == VK_SPACE) {
		renderer->paused = !renderer->paused;
		return 0;
	}
	if (message == WM_DESTROY) {
		if (renderer) renderer->running = false;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(window, message, wParam, lParam);
}
#endif
// WormholeGateway - C++ file
