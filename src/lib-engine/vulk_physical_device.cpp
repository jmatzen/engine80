#include "vulk_physical_device.hpp"
#include "vulk_surface.hpp"
#include "vulk_graphics.hpp"
#include "vulk_logical_device.hpp"
#include "vulk_result.hpp"
#include "vulk_swap_chain_support_details.hpp"
#include "vulk_swap_chain.hpp"

#include "logger.hpp"
#include <ranges>
#include <set>
#include <algorithm>
using namespace qf;
using namespace qf::vulk;

namespace
{
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


}


PhysicalDevice::PhysicalDevice(VulkanGraphics& graphics, Surface& surface, VkPhysicalDevice deviceHandle)
	: vkPhysicalDeviceHandle(deviceHandle)
	, graphics(graphics.sharedFromThis<VulkanGraphics>())
	, surface(surface.sharedFromThis<Surface>())
{
}

PhysicalDevice::~PhysicalDevice()
{
	// no explicit cleanup necessary because the context is destroyed when the instance is destroyed.
}

Expected<ptr<PhysicalDevice>> PhysicalDevice::create(VulkanGraphics& graphics, Surface& surface)
{

	u32 count = 0;
	TRY_VKEXPR(vkEnumeratePhysicalDevices(graphics.getInstance().value(), &count, nullptr));

	if (count == 0) {
		return std::unexpected("no GPUs with Vulkan support found.");
	}

	std::vector<VkPhysicalDevice> devices(count);
	TRY_VKEXPR(vkEnumeratePhysicalDevices(graphics.getInstance().value(), &count, devices.data()));

	VkSurfaceKHR const vkSurface = surface.getSurface();

	devices = devices
		//| std::views::filter(isDeviceSuitable)
		| std::views::filter([&vkSurface](VkPhysicalDevice device) {return isDeviceSuitable(device, vkSurface).value(); })
		| std::ranges::to<std::vector<VkPhysicalDevice>>();

	if (devices.empty()) {
		return std::unexpected("unable to find a suitable physical device");
	}

	auto physicalDevice = std::make_shared<PhysicalDevice>(graphics, surface, devices.front());

	/*
	* now create the logical device
	*/
	TRY_EXPR(physicalDevice->logicalDevice, LogicalDevice::create(*physicalDevice));

	TRY_EXPR(physicalDevice->swapChain, SwapChain::createSwapChain(*physicalDevice));

	return physicalDevice;

}

Expected<void> PhysicalDevice::initialize() {


	return {};
}

auto PhysicalDevice::findQueueFamilies(VkQueueFlagBits flagBits) const -> Expected<QueueFamilyIndices> {
	QueueFamilyIndices result;

	u32 count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDeviceHandle, &count, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProps(count);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDeviceHandle, &count, queueFamilyProps.data());


	for (int i = 0; i != queueFamilyProps.size(); ++i) {
		if (queueFamilyProps[i].queueFlags & flagBits) {
			result.graphics.emplace_back(i);
		}
		VkBool32 presentSupport{};
		TRY_VKEXPR(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDeviceHandle, i, surface.lock()->getSurface(), &presentSupport));
		if (presentSupport) {
			result.present.emplace_back(i);
		}
	}

	return result;
}

Graphics& PhysicalDevice::getGraphics() const {
	return *graphics.lock();
}

auto PhysicalDevice::querySwapChainSupport() const -> Expected<SwapChainSupportDetails> {
	VkSurfaceKHR const surface = this->getSurface()->getSurface();
	return SwapChain::querySwapChainSupport(vkPhysicalDeviceHandle, surface);
}

Expected<bool> PhysicalDevice::checkDeviceExtensionSupport() const {
	return checkDeviceExtensionSupport(vkPhysicalDeviceHandle);
}

Expected<bool> PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
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

Expected<bool> PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(device, &props);

	VkPhysicalDeviceFeatures feats{};
	vkGetPhysicalDeviceFeatures(device, &feats);

	if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		return std::unexpected("requires discrete GPU");
	}

	if (!feats.geometryShader) {
		return std::unexpected("requires geometry shader");
	}

	// Check if the device supports the required queue family
	if (!hasGraphicsQueueFamily(device)) {
		return std::unexpected("problem with queue families");
	}

	bool extensionsSupported;
	TRY_EXPR(extensionsSupported, checkDeviceExtensionSupport(device));
	if (!extensionsSupported) {
		return std::unexpected("doesn't support required device extensions");
	}

	SwapChainSupportDetails swapChainSupport;
	TRY_EXPR(swapChainSupport, SwapChain::querySwapChainSupport(device, surface));
	const bool okSwapChain = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	if (!okSwapChain) {
		return std::unexpected("device doesn't adaquately support swap chain formats or present modes");
	}
	return true;

}

bool PhysicalDevice::hasGraphicsQueueFamily(VkPhysicalDevice device) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProps.data());

	return std::any_of(queueFamilyProps.begin(), queueFamilyProps.end(), [](const VkQueueFamilyProperties& props) {
		return props.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		});
}