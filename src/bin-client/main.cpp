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

    auto appContext = qf::internalCreateInstance<qf::ApplicationContext>();
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


    std::cout << std::format("{}", val);
    return 0;
}
