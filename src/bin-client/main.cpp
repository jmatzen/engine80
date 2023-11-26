#include "lib-engine/engine80.hpp"
#include <SDL.h>
#include <iostream>
#include <thread>
#include <chrono>

glm::vec3 val{};

int main() {

    int result;
    if ((result = SDL_Init(SDL_INIT_VIDEO)))
    {
        //log_fatal("unable to initialize SDL, err={}", result);
    }


    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_VULKAN;


    auto window = SDL_CreateWindow("game"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , 1024
        , 768
        , flags)
        ;


    if (nullptr == window)
    {
        std::cerr << std::format("sdl failed to create window: err={}", SDL_GetError());
        return -1;
    }


    SDL_Event e{};
    for (;;) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT: {
                return 0;
            }
            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << std::format("{}", val);
    return 0;
}
