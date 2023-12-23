#include "vulk_surface.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_graphics.hpp"

#include <set>

namespace qf::vulk
{


	Surface::Surface(VulkanGraphics& graphics)
		: graphics_(graphics.sharedFromThis<VulkanGraphics>())
	{
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(graphics_.lock()->getInstance().value(),
			surface_,
			nullptr);
	}

	Expected<ptr<Surface>> Surface::create(VulkanGraphics& graphics) {
		auto surfaceObj = std::make_shared<Surface>(graphics);
		surfaceObj->initialize();
		return surfaceObj;
	}

	Expected<void> Surface::initialize() {
		auto graphics = graphics_.lock();

		VkWin32SurfaceCreateInfoKHR ci{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = GetModuleHandle(nullptr),
			.hwnd = reinterpret_cast<HWND>(graphics->getNativeWindowHandle().value()),
		};

		TRY_VKEXPR(vkCreateWin32SurfaceKHR(graphics->getInstance().value(), &ci, nullptr, &surface_));
		TRY_EXPR(this->physicalDevice_, createPhysicalDevice());
		return {};
	}

	Expected<ptr<PhysicalDevice>> Surface::createPhysicalDevice() {
		return PhysicalDevice::create(*graphics_.lock(), *this).value();
	}
}