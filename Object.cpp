#pragma once

#include "Object.hpp"
#include <glm/glm.hpp>

glm::vec3 Object::color = glm::vec3(1.0f, 1.0f, 1.0f);

float Object::kd = 1.0f;
float Object::ks = 0.0f;
float Object::m = 100.0f;
float Object::controlPoints[4][4] = {0};

void Object::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}
