#pragma once

#include <vector>
#include "Triangle.hpp"

struct ActiveEdge 
{
	float x1, y1, x2, y2;
	float yMax;
	float xCurrent;
	float inverseSlope;

	void updateEdge() { xCurrent += inverseSlope; }

	ActiveEdge(float x1, float y1, float x2, float y2)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;

		if (y1 > y2)
		{
			yMax = y1;
			xCurrent = x2;
			inverseSlope = (float)(x1 - x2) / (y1 - y2);
		}
		else
		{
			yMax = y2;
			xCurrent = x1;
			inverseSlope = (float)(x2 - x1) / (y2 - y1);
		}
	}

	bool operator==(ActiveEdge a2)
	{
		return (this->x1 == a2.x1 && this->x2 == a2.x2 && this->y1 == a2.y1 && this->y2 == a2.y2);
	}

};


class DrawingFunctions 
{
public:
	static void drawGrid(std::vector<Triangle> grid, int height, int width, SDL_Texture* texture, SDL_Renderer* renderer);

	static void getPolygonColors(Triangle polygon, int height, int width, uint32_t* pixelData, glm::vec3* vectors);

	static void fillScreen(int height, int width, uint32_t* pixelColor, SDL_Renderer* renderer, SDL_Texture* texture);
};
