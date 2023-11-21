#pragma once

#include <vector>
#include <SDL.h>
#include "Triangle.hpp"
#include <glm/glm.hpp>

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
	SDL_Surface* normalMap;
	SDL_Surface* objectTexture;
	uint32_t* pixelColors;
	glm::vec3** vectors;

	std::vector<Triangle> grid;

	void render(bool drawGrid);
};
