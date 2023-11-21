#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <execution>

#include <SDL.h>

#include "DrawingFunctions.hpp"
#include "Utils.hpp"
#include "LightSource.hpp"
#include "Object.hpp"


void DrawingFunctions::drawGrid(std::vector<Triangle> grid, int width, int height, SDL_Texture* texture, SDL_Renderer* renderer)
{
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	std::for_each(grid.begin(), grid.end(), [&](auto& t) {
		int x1 = static_cast<int>(t.A.x * width);
		int y1 = static_cast<int>(t.A.y * height);
		int x2 = static_cast<int>(t.B.x * width);
		int y2 = static_cast<int>(t.B.y * height);
		int x3 = static_cast<int>(t.C.x * width);
		int y3 = static_cast<int>(t.C.y * height);

		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
		SDL_RenderDrawLine(renderer, x1, y1, x3, y3);
	});
}

void DrawingFunctions::getPolygonColors(Triangle polygon, int width, int height, uint32_t* pixelData, glm::vec3** vectors, bool useNormalMap, bool useTexture)
{
	std::vector<int> xCoordinates{};
	xCoordinates.resize(width);
	std::iota(xCoordinates.begin(), xCoordinates.end(), 0);

	int polygonSize = 3;
	std::vector<glm::vec3> verticies{};
	for (int i = 0; i < polygonSize; i++)
	{
		verticies.push_back(glm::vec3((float)(polygon[i].x * width), (float)(polygon[i].y * height), polygon[i].z));
	}

	std::vector<int> idx;
	idx.resize(polygonSize);
	std::iota(idx.begin(), idx.end(), 0);
	std::sort(std::execution::par, idx.begin(), idx.end(), [&verticies](int i, int j) {return verticies[i].y < verticies[j].y;});
	float yMin = verticies[idx[0]].y;
	float yMax = verticies[idx[idx.size() - 1]].y;


	float Xa = polygon[0].x;
	float Ya = polygon[0].y;
	float Za = polygon[0].z;
	float Xb = polygon[1].x;
	float Yb = polygon[1].y;
	float Zb = polygon[1].z;
	float Xc = polygon[2].x;
	float Yc = polygon[2].y;
	float Zc = polygon[2].z;

	float a1 = (Yb * Xc - Yc * Xb);
	float b1 = (Yc * Xa - Ya * Xc);
	float divisora = ((Yb * Xc - Yc * Xb) + (Yc * Xa - Ya * Xc) + (Ya * Xb - Yb * Xa));
	float divisorb = ((Yb * Xc - Yc * Xb) + (Yc * Xa - Ya * Xc) + (Ya * Xb - Yb * Xa));

	glm::vec3 Na = Utils::GetNormalVector(polygon[0]);
	glm::vec3 Nb = Utils::GetNormalVector(polygon[1]);
	glm::vec3 Nc = Utils::GetNormalVector(polygon[2]);

	std::vector<ActiveEdge> AET{};

	for (float yi = yMin; yi <= yMax + 1; yi++)
	{
		float y = yi - 1;
		for (int k = 0; k < polygonSize; k++)
		{
			if (verticies[idx[k]].y < y)
			{
				continue;
			}
			if (verticies[idx[k]].y > y)
			{
				break;
			}

			int previdx = (idx[k] - 1) < 0 ? (int)idx.size() - 1 : (idx[k] - 1);
			int nextidx = (idx[k] + 1) > (int)idx.size() - 1 ? 0 : (idx[k] + 1);

			if (verticies[previdx].y > verticies[idx[k]].y)
			{
				AET.push_back(ActiveEdge(verticies[previdx].x, verticies[previdx].y, verticies[idx[k]].x, verticies[idx[k]].y));
			}
			else if (verticies[previdx].y < verticies[idx[k]].y)
			{
				for (auto it = AET.begin(); it != AET.end(); it++)
				{
					if (*(it) == ActiveEdge{ verticies[previdx].x, verticies[previdx].y, verticies[idx[k]].x, verticies[idx[k]].y })
					{
						AET.erase(it);
						break;
					}
				}
			}

			if (verticies[nextidx].y > verticies[idx[k]].y)
			{
				AET.push_back(ActiveEdge(verticies[nextidx].x, verticies[nextidx].y, verticies[idx[k]].x, verticies[idx[k]].y));
			}
			else if (verticies[nextidx].y < verticies[idx[k]].y)
			{
				for (auto it = AET.begin(); it != AET.end(); it++)
				{
					if (*(it) == ActiveEdge{ verticies[nextidx].x, verticies[nextidx].y, verticies[idx[k]].x, verticies[idx[k]].y })
					{
						AET.erase(it);
						break;
					}
				}
			}
		}

		std::sort(std::execution::par, AET.begin(), AET.end(), [](ActiveEdge& a1, ActiveEdge& a2) {
			return a1.xCurrent < a2.xCurrent;
			});

		for (int i = 0; i < AET.size(); i += 2)
		{
			for (float xDraw = AET[i].xCurrent; xDraw < AET[(i + 1)].xCurrent; xDraw++)
			{
				float alpha1 = (a1 + (Yc * (xDraw / width) - (y / height) * Xc) + ((y / height) * Xb - Yb * (xDraw / width))) / divisora;
				float beta1 = (b1 + (Ya * (xDraw / width) - (y / height) * Xa) + ((y / height) * Xc - Yc * (xDraw / width))) / divisorb;
				float gamma1 = 1 - alpha1 - beta1;
				
				float z = alpha1 * Za + beta1 * Zb + gamma1 * Zc;
				
				glm::vec3 N = Na * alpha1 + Nb * beta1 + Nc * gamma1;
				glm::vec3 objectColor = Object::color;

				if (useNormalMap)
				{
					glm::vec3 Nt = vectors[(int) y][(int)xDraw];
					glm::vec3 B = glm::all(glm::epsilonEqual(N, glm::vec3{ 0.0f, 0.0f, 1.0f }, 0.000001f)) ? glm::vec3{ 0.0f, 1.0f, 0.0f } : glm::cross(N, glm::vec3{ 0.0f, 0.0f, 1.0f });
					glm::vec3 T = glm::cross(B, N);
					glm::mat3x3 M{ T.x, T.y, T.z, B.x, B.y, B.z, N.x, N.y, N.z };
					N = M * Nt;
				}
				if (useTexture)
				{
					objectColor = Object::texture[(int) y][(int) xDraw];
				}

				glm::vec3 color = Utils::GetVertexColor(
					glm::vec3(xDraw/width, y/height, z),
					Object::kd,
					Object::ks,
					Object::m,
					LightSource::color,
					objectColor,
					LightSource::position,
					N
				);
				uint32_t r = static_cast<uint32_t>(color.x);
				uint32_t g = static_cast<uint32_t>(color.y);
				uint32_t b = static_cast<uint32_t>(color.z);
				r = ((r | (g << 8)) | (b << 16));
				pixelData[(int)xDraw + (int)y * width] = r;
			}
		}

		std::for_each(std::execution::par, AET.begin(), AET.end(), [](ActiveEdge& x) {
			x.updateEdge();
			});


	}
}

void DrawingFunctions::fillScreen(int width, int height, uint32_t* pixelColor, SDL_Renderer* renderer, SDL_Texture* texture)
{
	uint8_t* pixels;
	int pitch;
	const SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
	auto function = [&](int y)
	{
		Uint32* p = (Uint32*)(pixels + pitch * y);
		for (int x = 0; x < width; x++)
		{
			uint32_t color = pixelColor[y * width + x];
			*p = SDL_MapRGBA(pixelFormat, color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, 255);
			p++;
		}
	};

	std::vector<int> idx;
	idx.resize(height);
	std::iota(idx.begin(), idx.end(), 0);
	std::for_each(
		std::execution::par,
		idx.begin(),
		idx.end(),
		function
	);
	SDL_UnlockTexture(texture);
}
