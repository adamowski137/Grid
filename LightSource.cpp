#pragma once

#include "LightSource.hpp"

glm::vec3 LightSource::position = glm::vec3(0.5f, 0.5f, 1.0f);
glm::vec3 LightSource::color = glm::vec3(1.0f, 1.0f, 1.0f);
float LightSource::angle = 0;
int LightSource::depth = 100;

void LightSource::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}

void LightSource::move(float angleInc, float dz, int width, int height)
{
	angle -= angleInc;

	float c = -0.0001f;
	position += glm::vec3(c * angle * cosf(angle), c * angle * sinf(angle), 0);
}

glm::vec3 LightSource::realCoordinates(int w, int h)
{
	return glm::vec3(position.x * w, position.y * h, position.z * depth);
}