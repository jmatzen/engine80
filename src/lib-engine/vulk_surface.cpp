#include "vulk_surface.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_graphics.hpp"

#include <set>

namespace qf::vulk
{
	Expected<ptr<Surface>> Surface::create(VulkanGraphics& graphics) {
		VkWin32SurfaceCreateInfoKHR ci{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = GetModuleHandle(nullptr),
			.hwnd = reinterpret_cast<HWND>(graphics.getNativeWindowHandle().value()),
		};

		VkSurfaceKHR surface;
		TRY_VKEXPR(vkCreateWin32SurfaceKHR(graphics.getInstance().value(), &ci, nullptr, &surface));

		return std::make_shared<Surface>(graphics, surface);
	}

	Surface::Surface(VulkanGraphics& graphics, VkSurfaceKHR surface)
		: surface_(surface)
		, graphics_(graphics.sharedFromThis<VulkanGraphics>())
	{
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(graphics_.lock()->getInstance().value(),
			surface_,
			nullptr);
	}
}