#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Triangle.hpp"
#include <math.h>
#include <SDL.h>


class Utils
{
public:
	static std::vector<Triangle> generateGrid(int amount);

	static float CalculateZ(float x, float y);

	static float B(float i, float t);

	static glm::vec3 Pu(float x, float y);

	static glm::vec3 Pv(float x, float y);

	static glm::vec3 GetNormalVector(glm::vec3 point, glm::vec3 Nt);

	static glm::uvec3 GetVertexColor(glm::vec3 point, float kd, float ks, float m, glm::vec3 Il, glm::vec3 Io, glm::vec3 LP, glm::vec3 N);

	static glm::vec3* getNormalMapVectors(SDL_Surface* normalMap, int width, int height);

private:
	static uint32_t getPixel(SDL_Surface* surface, int x, int y);
};