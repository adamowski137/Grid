#pragma once

#include <SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "ImGuiFileDialog.h"
#include "Utils.hpp"
#include "DrawingFunctions.hpp"
#include "Object.hpp"
#include "LightSource.hpp"
#include <omp.h>
#include <iostream>
#include <chrono>
#include <math.h>
#include <execution>
#include "Window.hpp"

Window::Window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return;
    }
    window = SDL_CreateWindow("Polygon Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        return;
    }

    normalMap = IMG_Load("NormalMap.png");
    if (normalMap == NULL) {
        std::cerr << "Failed to load the image: " << IMG_GetError() << std::endl;
    }

    gridTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    surfaceTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetTextureBlendMode(gridTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(surfaceTexture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    SDL_RenderClear(renderer);

    pixelColors = new Uint32[(SCREEN_WIDTH + 1) * (SCREEN_HEIGHT + 1)];
    grid = Utils::generateGrid(2);
    vectors = Utils::getNormalMapVectors(normalMap, SCREEN_WIDTH, SCREEN_HEIGHT);
}

Window::~Window()
{
    for (int i = 0; i < SCREEN_HEIGHT + 1; i++)
    {
        delete(vectors[i]);
    }
    delete(pixelColors);
    delete(vectors);
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui::DestroyContext();
    SDL_FreeSurface(normalMap);
    SDL_DestroyTexture(gridTexture);
    SDL_DestroyTexture(surfaceTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::runWindow()
{
    SDL_Event e;

    bool quit = false;
    bool drawGrid = false;
    bool updateGrid = true;
    bool updateSurface = true;
    bool animation = false;
    bool useNormalMap = false;
    int amount = 2;
    float lColor[3] = { 1.0f, 1.0f, 1.0f };
    float oColor[3] = { 1.0f, 1.0f, 1.0f };
    float prevColor[3] = { lColor[0], lColor[1], lColor[2] };
    float z = 0;
    auto prevTime = std::chrono::high_resolution_clock::now();
    while (!quit)
    {
        //auto startFrame = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - prevTime;
        prevTime = currentTime;
        int prevAmount = amount;
        bool prevDrawGrid = drawGrid;
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Options");
        ImGui::Checkbox("Draw grid", &drawGrid);
        ImGui::SliderInt("Amount of triangles", &amount, 2, 100, "%d");
        ImGui::ColorPicker3("Select light color", lColor);
        ImGui::SliderFloat("Kd", &(Object::kd), 0.0f, 1.0f);
        ImGui::SliderFloat("Ks", &(Object::ks), 0.0f, 1.0f);
        ImGui::SliderFloat("m", &(Object::m), 1.0f, 100.0f, "%1.0f");
        ImGui::Checkbox("Animation", &animation);
        ImGui::SliderFloat("Light Z position", &z, 0.0f, 5.0f, "%0.2f");
        ImGui::ColorPicker3("Select object color", oColor);
        ImGui::Checkbox("Use normal map", &useNormalMap);
        if (ImGui::Button("Select normal map")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png", ".");

        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse)) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                auto sel = ImGuiFileDialog::Instance()->GetSelection();
                normalMap = IMG_Load((*sel.begin()).second.c_str());
                if (normalMap == NULL) {
                    std::cerr << "Failed to load the image: " << IMG_GetError() << std::endl;
                }
                vectors = Utils::getNormalMapVectors(normalMap, SCREEN_WIDTH, SCREEN_HEIGHT);

            }
            ImGuiFileDialog::Instance()->Close();
        }
        
        ImGui::End();

        ImGui::Begin("Control Points");
        ImGui::SliderFloat("p00", &Object::controlPoints[0][0], 0.01f, 1.0f);
        ImGui::SliderFloat("p01", &Object::controlPoints[0][1], 0.01f, 1.0f);
        ImGui::SliderFloat("p02", &Object::controlPoints[0][2], 0.01f, 1.0f);
        ImGui::SliderFloat("p03", &Object::controlPoints[0][3], 0.01f, 1.0f);

        ImGui::SliderFloat("p10", &Object::controlPoints[1][0], 0.01f, 1.0f);
        ImGui::SliderFloat("p11", &Object::controlPoints[1][1], 0.01f, 1.0f);
        ImGui::SliderFloat("p12", &Object::controlPoints[1][2], 0.01f, 1.0f);
        ImGui::SliderFloat("p13", &Object::controlPoints[1][3], 0.01f, 1.0f);

        ImGui::SliderFloat("p20", &Object::controlPoints[2][0], 0.01f, 1.0f);
        ImGui::SliderFloat("p21", &Object::controlPoints[2][1], 0.01f, 1.0f);
        ImGui::SliderFloat("p22", &Object::controlPoints[2][2], 0.01f, 1.0f);
        ImGui::SliderFloat("p23", &Object::controlPoints[2][3], 0.01f, 1.0f);

        ImGui::SliderFloat("p30", &Object::controlPoints[3][0], 0.01f, 1.0f);
        ImGui::SliderFloat("p31", &Object::controlPoints[3][1], 0.01f, 1.0f);
        ImGui::SliderFloat("p32", &Object::controlPoints[3][2], 0.01f, 1.0f);
        ImGui::SliderFloat("p33", &Object::controlPoints[3][3], 0.01f, 1.0f);

        ImGui::End();

        
        ImGui::Render();

        LightSource::position.z = z;

        if (prevAmount != amount)
        {
            updateGrid = true;
            updateSurface = true;
        }

            
        LightSource::setColor(lColor[0], lColor[1], lColor[2]);


        Object::setColor(oColor[0], oColor[1], oColor[2]);

        if (prevDrawGrid != drawGrid)
        {
            updateGrid = true;
        }

        if (updateGrid)
        {
            grid = Utils::generateGrid(amount);

            updateGrid = false;
        }

        if (animation)
        {
            float speed = 0.01f;
            float deltaTime = elapsed.count();
            float movement = speed * deltaTime;
            LightSource::move(movement, z, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        auto processGridElement = [&](const Triangle& element) {
            DrawingFunctions::getPolygonColors(element, SCREEN_WIDTH, SCREEN_HEIGHT, pixelColors, vectors, useNormalMap);
        };

        if (updateSurface)
        {

            std::for_each(std::execution::par, grid.begin(), grid.end(), processGridElement);
        }

        while (SDL_PollEvent(&e) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&e);

            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

        }
        render(drawGrid);

    }
}

void Window::render(bool drawGrid)
{
    SDL_SetRenderTarget(renderer, surfaceTexture);
    SDL_RenderClear(renderer);

    DrawingFunctions::fillScreen(SCREEN_WIDTH, SCREEN_HEIGHT, pixelColors, renderer, surfaceTexture);
    auto end = std::chrono::high_resolution_clock::now();

    if (drawGrid)
    {
        DrawingFunctions::drawGrid(grid, SCREEN_WIDTH, SCREEN_HEIGHT, gridTexture, renderer);
    }
    else
    {
        SDL_SetRenderTarget(renderer, gridTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, surfaceTexture, NULL, NULL);
    SDL_RenderCopy(renderer, gridTexture, NULL, NULL );
    
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}