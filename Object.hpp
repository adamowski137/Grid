#pragma once

#include <glm/glm.hpp>

class Object
{
public:

	static glm::vec3 color;
	static float kd;
	static float ks;
	static float m;

	static float controlPoints[4][4];

	static void setColor(float, float, float);
	

};
