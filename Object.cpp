#pragma once

#include "Object.hpp"
#include <glm/glm.hpp>

glm::vec3 Object::color = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3** Object::texture = nullptr;

float Object::kd = 1.0f;
float Object::ks = 0.0f;
float Object::m = 100.0f;
float Object::controlPoints[4][4] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };

void Object::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}
