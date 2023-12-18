#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;

	class PhysicalDevice : public std::enable_shared_from_this<PhysicalDevice>
	{
		VkPhysicalDevice physicalDevice{};
		weak<VulkanGraphics> graphics{};

	public:
		PhysicalDevice(VulkanGraphics& graphics, VkPhysicalDevice device);

		~PhysicalDevice();

		struct QueueFamilyIndices {
			std::vector<u32> graphics;
			std::vector<u32> present;
		};

		Expected<QueueFamilyIndices> findQueueFamilies(VkQueueFlagBits flagBits = VK_QUEUE_GRAPHICS_BIT) const;

		VkPhysicalDevice getVkPhysicalDevice() const { return physicalDevice; }

		static Expected<ptr<PhysicalDevice>> factory(VulkanGraphics& instance);
	};
}