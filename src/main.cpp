#include <SDL3/SDL.h>

constexpr int screenWidth = 160;
constexpr int screenHeight = 144;
constexpr int screenMultiplier = 4;

auto main(int argc, char *argv[]) -> int
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Tom Boy",
        screenWidth * screenMultiplier, screenHeight * screenMultiplier, 0);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
