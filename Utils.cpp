#pragma once
#include "Utils.hpp"
#include "Object.hpp"
#include <algorithm>
#include <execution>

 std::vector<Triangle> Utils::generateGrid(int amount)
{
	float height = 1.0f;
	float width = 1.0f;
	std::vector<Triangle> grid{};
	for (float i = 1.0f; i < amount + 1; i++)
	{
		float currentHeight = i * height / amount;
		float prevHeight = (i - 1.0f) * height / amount;

		for (float j = 1.0f; j < amount + 1; j++)
		{
			float currentWidth = j * width / amount;
			float prevWidth = (j - 1.0f) * width / amount;
			glm::vec3 A1{ prevWidth, prevHeight, CalculateZ(prevWidth, prevHeight) };
			glm::vec3 B1{ currentWidth, prevHeight, CalculateZ(currentWidth, prevHeight) };
			glm::vec3 C1{ prevWidth, currentHeight, CalculateZ(prevWidth, currentHeight) };
			glm::vec3 A2{ currentWidth, currentHeight, CalculateZ(currentWidth, currentHeight) };
			glm::vec3 B2{ currentWidth, prevHeight, CalculateZ(currentWidth, prevHeight) };
			glm::vec3 C2{ prevWidth, currentHeight, CalculateZ(prevWidth, currentHeight) };
			grid.push_back(Triangle{ A1, B1, C1 });
			grid.push_back(Triangle{ A2, B2, C2 });	
		}
	}
	return grid;
}

float Utils::CalculateZ(float x, float y)
{
	float sum = 0.0f;
	for (float i = 0.0f; i < 4.0f; i++)
	{
		for (float j = 0.0f; j < 4.0f; j++)
		{
			sum += Object::controlPoints[(int)i][(int)j] * (B(i, x) * B(j, y));
		}
	}

	return sum;
}

 float Utils::B(float i, float t)
{
	float n = 3;

	float m = 1.0f;
	if (i == 1.0f || i == 2.0f)
	{
		m = 3.0f;
	}

	float term1 = powf(t, i);
	float term2 = powf(1 - t, n - i);

	return m * term1 * term2;
}

 glm::vec3 Utils::Pu(float x, float y)
{
	float epsilon = 0.00001f;
	float z = (CalculateZ(x + epsilon, y) - CalculateZ(x - epsilon, y)) / (2 * epsilon);
	return glm::vec3(1, 0, z);
}

 glm::vec3 Utils::Pv(float x, float y)
{
	float epsilon = 0.00001f;
	float z = (CalculateZ(x, y + epsilon) - CalculateZ(x, y - epsilon)) / (2 * epsilon);
	return glm::vec3(0, 1, z);
}

 glm::vec3 Utils::GetNormalVector(glm::vec3 point, glm::vec3 Nt)
{
	glm::vec3 N = glm::cross(Pu(point.x, point.y), Pv(point.x, point.y));
	N = glm::normalize(N);

	glm::vec3 b = N == glm::vec3{ 0.0f, 0.0f, 1.0f } ? glm::vec3{ 0.0f, 1.0f, 0.0f } : glm::cross(N, glm::vec3{ 0.0f, 0.0f, 1.0f });
	glm::vec3 T = glm::cross(b, N);
	//glm::mat3x3 M{ T, b, N };
	//glm::mat3x3 M{ T.x, b.x, N.x, T.y, b.y, N.y, T.z, b.z, N.z };
	glm::mat3x3 M{ T.x, T.y, T.z, b.x, b.y, b.z, N.x, N.y, N.z };


	return (M * Nt);
}

 glm::uvec3 Utils::GetVertexColor(glm::vec3 point, float kd, float ks, float m, glm::vec3 Il, glm::vec3 Io, glm::vec3 LP, glm::vec3 N)
{

	glm::vec3 L = glm::normalize(glm::vec3(LP.x - point.x, LP.y - point.y, LP.z - point.z));
	glm::vec3 V = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 R = ((2 * glm::dot(N, L) * N) - L);

	float nl = glm::dot(N, L);
	float vr = glm::dot(V, R);
	nl = nl < 0 ? 0 : nl;
	vr = vr < 0 ? 0 : vr;

	glm::vec3 I = kd * Il * Io * nl + ks * Il * Io * powf(vr, m);

	return glm::uvec3((int)fminf(I.x * 255, 255), (int)fminf(I.y * 255, 255), (int)fminf(I.z * 255, 255));
}

 uint32_t Utils::getPixel(SDL_Surface* surface, int x, int y)
 {
	 int bpp = surface->format->BytesPerPixel;
	 /* Here p is the address to the pixel we want to retrieve */
	 Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	 switch (bpp)
	 {
	 case 1:
		 return *p;
		 break;

	 case 2:
		 return *(Uint16*)p;
		 break;

	 case 3:
		 if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			 return p[0] << 16 | p[1] << 8 | p[2];
		 else
			 return p[0] | p[1] << 8 | p[2] << 16;
		 break;

	 case 4:
		 return *(Uint32*)p;
		 break;

	 default:
		 return 0;
	 }
 }

 glm::vec3* Utils::getNormalMapVectors(SDL_Surface* normalMap, int width, int height)
 {
	 glm::vec3* vectors = new glm::vec3[(width + 1) * (height + 1)];
	 int w = normalMap->w;
	 int h = normalMap->h;
	 int bpp = normalMap->format->BytesPerPixel;

	 auto function = [&](int y)
	 {
		 for (int x = 0; x <= width; x++)
		 {
			 SDL_Color rgb;
			 Uint32 data = getPixel(normalMap, x, y);
			 SDL_GetRGB(data, normalMap->format, &rgb.r, &rgb.g, &rgb.b);
			 vectors[x + y * width] = glm::vec3(rgb.r / 255.0f, rgb.g / 255.0f, rgb.b / 255.0f);
		 }
	 };

	 std::vector<int> idx;
	 idx.resize(std::max(width, height));
	 std::iota(idx.begin(), idx.end(), 0);
	 std::for_each(
		 std::execution::par,
		 idx.begin(),
		 idx.begin() + height,
		 function
	 );
	 return vectors;
 }
