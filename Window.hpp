#pragma once

#include <vector>
#include <SDL.h>
#include "Triangle.hpp"

class Window
{
public:
	Window();
	~Window();

	void runWindow();

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* gridTexture;
	SDL_Texture* surfaceTexture;
	uint32_t* pixelColors;

	std::vector<Triangle> grid;

	void render(bool drawGrid);
};
