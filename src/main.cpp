#include <SDL3/SDL.h>

#include <iostream>
#include <print>

constexpr int screen_width = 160;
constexpr int screen_height = 144;
constexpr int screen_multiplier = 4;

auto main(int argc, char *argv[]) -> int
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println(std::cerr, "Initialization failed.\n{}", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Tom Boy",
        screen_width * screen_multiplier, screen_height * screen_multiplier, 0);

    if (window == nullptr) {
        std::println(std::cerr, "Window creation failed.\n{}", SDL_GetError());
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::println(
            std::cerr, "Renderer creation failed.\n{}", SDL_GetError());
        return -1;
    }

    bool running = true;
    while (running) {
        // Poll events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
