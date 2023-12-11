#include "vulkan_graphics.hpp"

using namespace e80;
using namespace e80::vulk;

VulkanGraphics::VulkanGraphics(PlatformInterface& platform) 
	: platform_(platform.sharedFromThis<PlatformInterface>())
{
}

std::expected<void, std::exception> VulkanGraphics::initialize()
{
	return std::unexpected(std::exception("woops"));
}


//IMPLEMENT_CLASS_FACTORY(e80::vulk::VulkanGraphics);
