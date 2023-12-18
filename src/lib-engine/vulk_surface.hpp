#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;

	class Surface : public EnableSharedFromThis<Surface>
	{
		VkSurfaceKHR surface_;
		weak<VulkanGraphics> graphics_;



	public:

		Surface(VulkanGraphics& graphics, VkSurfaceKHR surface);

		~Surface();

		static Expected<ptr<Surface>> create(VulkanGraphics& graphics);

		VkSurfaceKHR getSurface() const { return surface_; }
	};
}
