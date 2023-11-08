#include "Triangle.hpp"
#include <glm/glm.hpp>


glm::vec3 Triangle::operator[](int i)
{
	if (i == 0)
	{
		return this->A;
	}
	else if (i == 1)
	{
		return this->B;
	}
	else
	{
		return this->C;
	}
}