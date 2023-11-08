#include <glm/glm.hpp>

#pragma once


class Triangle
{
public:
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : A{ a }, B{ b }, C{ c } {};

	glm::vec3 A;
	glm::vec3 B;
	glm::vec3 C;

	glm::vec3 operator[](int i);

private:
};
