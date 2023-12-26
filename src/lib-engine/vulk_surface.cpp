#include "vulk_surface.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_graphics.hpp"
#include "vulk_logical_device.hpp"

#include <set>

namespace qf::vulk
{


	Surface::Surface(VulkanGraphics& graphics)
		: graphics_(graphics)
	{
	}

	Surface::~Surface()
	{
		physicalDevice_.reset();
		vkDestroySurfaceKHR(graphics_.getInstance().value(),
			surface_,
			nullptr);
	}

	Expected<Box<Surface>> Surface::create(VulkanGraphics& graphics) {
		auto surfaceObj = std::make_unique<Surface>(graphics);
		surfaceObj->initialize();
		return surfaceObj;
	}

	Expected<void> Surface::initialize() {
		VkWin32SurfaceCreateInfoKHR ci{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = GetModuleHandle(nullptr),
			.hwnd = reinterpret_cast<HWND>(graphics_.getNativeWindowHandle().value()),
		};

		TRY_VKEXPR(vkCreateWin32SurfaceKHR(graphics_.getInstance().value(), &ci, nullptr, &surface_));
		TRY_EXPR(this->physicalDevice_, createPhysicalDevice());
		return {};
	}

	Expected<Box<PhysicalDevice>> Surface::createPhysicalDevice() {
		return PhysicalDevice::create(graphics_, *this).value();
	}
}