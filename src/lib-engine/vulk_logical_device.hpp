#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;
	class PhysicalDevice;

	class LogicalDevice
	{
		VkDevice device_{};
		VkQueue graphicsQueue_{};
		VkQueue presentQueue_{};

		weak<PhysicalDevice> physicalDevice_;
		weak<VulkanGraphics> graphics_;

		Expected<void> initialize();
	public:

		LogicalDevice(PhysicalDevice& device);

		~LogicalDevice();

		static Expected<ptr<LogicalDevice>> create(PhysicalDevice& device);

	};
}