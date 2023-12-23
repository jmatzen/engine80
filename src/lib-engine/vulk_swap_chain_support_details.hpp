#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

}