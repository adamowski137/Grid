
#include "SDL.h"
#include <iostream>
#include <omp.h>
#include "Window.hpp"

int main(int argc, char* args[])
{
    Window window{};
    window.runWindow();

    return 0;
}

