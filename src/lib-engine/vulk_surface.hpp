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
		VulkanGraphics& graphics_;
		Box<PhysicalDevice> physicalDevice_;



		Expected<void> initialize();
		Expected<Box<PhysicalDevice>> createPhysicalDevice();

	public:

		Surface(VulkanGraphics& graphics);

		~Surface();

		void dispose();

		static Expected<Box<Surface>> create(VulkanGraphics& graphics);

		VkSurfaceKHR getSurface() const { return surface_; }

	};
}
