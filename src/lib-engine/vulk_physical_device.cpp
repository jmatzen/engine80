#include "vulk_physical_device.hpp"
#include "vulk_surface.hpp"
#include "vulkan_graphics.hpp"
#include "logger.hpp"
#include <ranges>
#include <set>
using namespace qf;
using namespace qf::vulk;

namespace
{
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(device, &props);

		VkPhysicalDeviceFeatures feats{};
		vkGetPhysicalDeviceFeatures(device, &feats);

		if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			return false;// std::unexpected("requires discrete GPU");
		}

		if (!feats.geometryShader) {
			return false; // std::unexpected("requires geometry shader");
		}

		// TODO: merge this with the findQueueFamilies method
		u32 count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProps(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilyProps.data());

		std::vector<u32> result;
		for (int i = 0; i != queueFamilyProps.size(); ++i) {
			if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				result.emplace_back(i);
			}
		}

		if (result.empty()) {
			return false;
		}

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		return extensionsSupported;

	}

}
Expected<ptr<PhysicalDevice>> PhysicalDevice::factory(VulkanGraphics& graphics)
{
	u32 count = 0;
	TRY_VKEXPR(vkEnumeratePhysicalDevices(graphics.getInstance().value(), &count, nullptr));

	if (count == 0) {
		return std::unexpected("no GPUs with Vulkan support found.");
	}

	std::vector<VkPhysicalDevice> devices(count);
	TRY_VKEXPR(vkEnumeratePhysicalDevices(graphics.getInstance().value(), &count, devices.data()));

	devices = devices 
		| std::views::filter(isDeviceSuitable) 
		| std::ranges::to<std::vector<VkPhysicalDevice>>();

	if (devices.empty()) {
		return std::unexpected("unable to find a suitable physical device");
	}

	return std::make_shared<PhysicalDevice>(graphics, devices.front());
}

auto PhysicalDevice::findQueueFamilies(VkQueueFlagBits flagBits) const -> Expected<QueueFamilyIndices> {
	QueueFamilyIndices result;

	u32 count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProps(count);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilyProps.data());

	auto surface = graphics.lock()->getSurface()->getSurface();

	for (int i = 0; i != queueFamilyProps.size(); ++i) {
		if (queueFamilyProps[i].queueFlags & flagBits) {
			result.graphics.emplace_back(i);
		}
		VkBool32 presentSupport{};
		TRY_VKEXPR(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport));
		if (presentSupport) {
			result.present.emplace_back(i);
		}
	}

	return result;
}

PhysicalDevice::PhysicalDevice(VulkanGraphics& graphics, VkPhysicalDevice device)
	: physicalDevice(device)
	, graphics(graphics.sharedFromThis<VulkanGraphics>())
{
}

PhysicalDevice::~PhysicalDevice()
{
	// no explicit cleanup necessary because the context is destroyed when the instance is destroyed.
}