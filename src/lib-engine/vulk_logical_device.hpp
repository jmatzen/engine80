#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>
#include <optional>
#include <string>

namespace qf::vulk
{
	class VulkanGraphics;
	class PhysicalDevice;
	class SwapChain;

	class LogicalDevice : NonCopyable
	{
		VkDevice device_{};
		VkQueue graphicsQueue_{};
		VkQueue presentQueue_{};

		PhysicalDevice& physicalDevice_;
		Box<SwapChain> swapChain_;

		std::vector<std::string> requiredDeviceExtensions_;

		Expected<void> initialize();
		Expected<void> checkDeviceExtensionSupport() const;



	public:

		LogicalDevice(PhysicalDevice& device);

		~LogicalDevice();

		static Expected<Box<LogicalDevice>> create(PhysicalDevice& device);

		PhysicalDevice& getPhysicalDevice() const { return physicalDevice_; }

		VkDevice getHandle() const { return device_; }

	};
}