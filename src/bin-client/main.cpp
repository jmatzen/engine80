#include "lib-engine/engine80.hpp"
#include "lib-engine/application_context.hpp"
#include "lib-engine/platform_interface.hpp"
#include "lib-engine/class_ids.hpp"
#include "lib-engine/graphics.hpp"
#include "lib-engine/logger.hpp"

#include <SDL.h>

#include <iostream>
#include <thread>
#include <chrono>

glm::vec3 val{};

namespace qf
{
    class Sdl2PlatformInterface;
}

namespace qf::vulk {
    class VulkanGraphics;
}

using namespace qf;

int main() {

    log::info("hello");

    qf::registerFactories();

    //auto appContext = e80::CreateInstance<e80::ApplicationContextClass>();
    //e80::IApplicationContext::SetApplicationContext(appContext);

    //auto platform = std::make_shared<e80::Sdl2PlatformInterfaceClass>();

    auto platform = createInstance<PlatformInterface>(qf::Sdl2PlatformInterfaceClassId);
    if (auto initRes = platform->initialize(); !initRes.has_value()) {
        std::cerr << initRes.error() << std::endl;
    }

    auto graphics = Graphics::createInstance<vulk::VulkanGraphics>(Graphics::CreateInstanceInfo{
            .pi = platform,
            .appName = "QuantaForge"
        });

    graphics->initialize();

    //appContext->RegisterService(std::move(graphics), "graphics");

    //auto graphics = e80::GetApplicationContext().GetServiceByName<e80::IGraphics>("graphics").value_or(nullptr);
    //graphics->Initialize();


    //int result;
    //if ((result = SDL_Init(SDL_INIT_VIDEO)))
    //{
    //    //log_fatal("unable to initialize SDL, err={}", result);
    //}


    //uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_VULKAN;


    //auto window = SDL_CreateWindow("game"
    //    , SDL_WINDOWPOS_CENTERED
    //    , SDL_WINDOWPOS_CENTERED
    //    , 1920
    //    , 1080
    //    , flags)
    //    ;


    //if (nullptr == window)
    //{
    //    std::cerr << std::format("sdl failed to create window: err={}", SDL_GetError());
    //    return -1;
    //}


    //SDL_Event e{};
    //for (;;) {
    //    while (SDL_PollEvent(&e)) {
    //        switch (e.type) {
    //        case SDL_QUIT: {
    //            return 0;
    //        }
    //        }

    //    }
    //    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //}

    std::cout << std::format("{}", val);
    return 0;
}
