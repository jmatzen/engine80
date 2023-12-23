#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;
	class PhysicalDevice;

	class Surface : public EnableSharedFromThis<Surface>
	{
		VkSurfaceKHR surface_{};
		weak<VulkanGraphics> graphics_;
		ptr<PhysicalDevice> physicalDevice_;



		Expected<void> initialize();
		Expected<ptr<PhysicalDevice>> createPhysicalDevice();

	public:

		Surface(VulkanGraphics& graphics);

		~Surface();

		static Expected<ptr<Surface>> create(VulkanGraphics& graphics);

		VkSurfaceKHR getSurface() const { return surface_; }

	};
}
