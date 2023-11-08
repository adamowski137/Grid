#include "LightSource.hpp"
#include <glm/glm.hpp>
#include <math.h>

glm::vec3 LightSource::position = glm::vec3(0.5f, 0.5f, 100);
glm::vec3 LightSource::color = glm::vec3(1.0f, 1.0f, 1.0f);

void LightSource::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}

void LightSource::move(float angle, float dz)
{
	position += glm::vec3(0.1*angle*cosf(angle), 0.1*angle*sinf(angle), dz);
}