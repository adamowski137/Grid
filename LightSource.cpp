#pragma once

#include "LightSource.hpp"
#include <math.h>

glm::vec3 LightSource::position = glm::vec3(0.5f, 0.5f, 1.0f);
glm::vec3 LightSource::color = glm::vec3(1.0f, 1.0f, 1.0f);
float LightSource::angle = 0;

void LightSource::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}

void LightSource::move(float angleInc, float dz, int width, int height)
{
	angle += angleInc;
	position += glm::vec3((10.0f / width) * cosf(angle), (10.0f / height) * sinf(angle), 0);
}

glm::vec3 LightSource::realCoordinates(int w, int h, int d)
{
	return glm::vec3(position.x * w, position.y * h, position.z * d);
}