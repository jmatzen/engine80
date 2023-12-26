#pragma once

#include "engine80.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	class VulkanGraphics;
	class Surface;
	class LogicalDevice;
	class SwapChain;

	struct SwapChainSupportDetails;


	class PhysicalDevice : public EnableSharedFromThis<PhysicalDevice>
	{
		const VkPhysicalDevice vkPhysicalDeviceHandle{};
		const weak<VulkanGraphics> graphics{};
		const weak<Surface> surface{};
		ptr<LogicalDevice> logicalDevice{};

		Expected<void> initialize();




		/**
		 * Checks if the given Vulkan physical device supports a graphics queue family.
		 *
		 * @param device The Vulkan physical device to check.
		 * @return True if the device supports a graphics queue family, false otherwise.
		 *
		 * This method queries the queue family properties of the specified physical device
		 * and checks if any of the queue families have the VK_QUEUE_GRAPHICS_BIT flag set,
		 * indicating support for graphics operations. It returns true if at least one queue
		 * family supports graphics, and false otherwise.
		 */
		static bool hasGraphicsQueueFamily(VkPhysicalDevice device);

		/**
		 * @brief Checks if a given Vulkan physical device is suitable for the application's needs.
		 *
		 * This method evaluates the physical device based on several criteria: whether it is a discrete GPU,
		 * whether it supports geometry shaders, if it has the necessary queue families, if it supports
		 * the required device extensions, and if it provides adequate swap chain support in terms of formats
		 * and present modes. All criteria must be met for a device to be considered suitable.
		 *
		 * @param device The Vulkan physical device to evaluate.
		 * @param surface The Vulkan surface that the physical device will render to.
		 * @return Expected<bool> Returns an Expected object containing a boolean value. If the physical device
		 *         is suitable, the Expected object will contain 'true'. If the device is not suitable, the
		 *         Expected object will contain an error message detailing the reason.
		 *
		 * @note The method now includes a check for swap chain adequacy, ensuring that the device supports
		 *       at least one format and one present mode for the given surface.
		 */
		static Expected<bool> isDeviceSuitable(VkPhysicalDevice, VkSurfaceKHR);

		static [[nodiscard]] Expected<bool> checkDeviceExtensionSupport(VkPhysicalDevice);

	public:
		PhysicalDevice(VulkanGraphics& graphics, Surface& surface, VkPhysicalDevice deviceHandle);

		~PhysicalDevice();

		void dispose() override;

		struct QueueFamilyIndices {
			std::vector<u32> graphics;
			std::vector<u32> present;
		};

		Expected<QueueFamilyIndices> findQueueFamilies(VkQueueFlagBits flagBits = VK_QUEUE_GRAPHICS_BIT) const;

		Expected<bool> checkDeviceExtensionSupport() const;

		VkPhysicalDevice getVkPhysicalDevice() const { return vkPhysicalDeviceHandle; }

		static Expected<Box<PhysicalDevice>> create(VulkanGraphics& instance, Surface& surface);

		VulkanGraphics& getGraphics() const;

		ptr<Surface> getSurface() const { return surface.lock(); }

		Expected<SwapChainSupportDetails> querySwapChainSupport() const;

		operator VkPhysicalDevice() const { return vkPhysicalDeviceHandle; }
	};
}