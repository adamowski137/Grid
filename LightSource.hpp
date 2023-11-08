#pragma once

#include <glm/glm.hpp>

class LightSource
{
public:

	static glm::vec3 position;
	static glm::vec3 color;

	static void setColor(float r, float g, float b);

	static void move(float angle, float dz);

};