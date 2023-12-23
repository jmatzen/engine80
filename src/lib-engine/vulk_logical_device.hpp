#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;
	class PhysicalDevice;
	class SwapChain;

	class LogicalDevice : EnableSharedFromThis<LogicalDevice>
	{
		VkDevice device_{};
		VkQueue graphicsQueue_{};
		VkQueue presentQueue_{};

		weak<PhysicalDevice> physicalDevice_;
		weak<VulkanGraphics> graphics_;
		ptr<SwapChain> swapChain_;

		Expected<void> initialize();


	public:

		LogicalDevice(PhysicalDevice& device);

		~LogicalDevice();

		static Expected<ptr<LogicalDevice>> create(PhysicalDevice& device);

	};
}