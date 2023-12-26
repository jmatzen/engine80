
#include "vulk_swap_chain.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_logical_device.hpp"
#include "vulk_surface.hpp"
#include "application_context.hpp"
#include "vulk_graphics.hpp"
#include "platform_interface.hpp"
#include <yaml-cpp/yaml.h>
#include <ranges>
#include <limits>

namespace qf::vulk
{
	auto SwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) -> Expected<SwapChainSupportDetails> 
	{
		SwapChainSupportDetails details;
		TRY_VKEXPR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities));

		uint32_t formatCount;
		TRY_VKEXPR(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			TRY_VKEXPR(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()));
		}

		uint32_t presentModeCount;
		TRY_VKEXPR(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			TRY_VKEXPR(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data()));
		}

		return details;
	}

	auto SwapChain::createSwapChain(LogicalDevice& logicalDevice) -> Expected<Box<SwapChain>> {

		auto& physicalDevice = logicalDevice.getPhysicalDevice();

		SwapChainSupportDetails details;
		auto& surface = physicalDevice.getSurface();
		TRY_EXPR(details, SwapChain::querySwapChainSupport(
			physicalDevice,
			surface.getSurface()
		));


		VkSurfaceFormatKHR surfaceFormat;
		TRY_EXPR(surfaceFormat, chooseSwapSurfaceFormat(details.formats));

		VkPresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);

		auto [width, height] = physicalDevice.getGraphics()
			.getPlatform()
			.value()->getWindowExtents()
			.value();

		VkExtent2D extent = chooseSwapExtent(details.capabilities, width, height);

		uint32_t imageCount = details.capabilities.minImageCount + 1;

		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.getSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		auto indices = physicalDevice.findQueueFamilies().value();
		uint32_t queueFamilyIndices[] = { indices.graphics.front(), indices.graphics.front() };

		if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkSwapchainKHR swapChain;
		TRY_VKEXPR(vkCreateSwapchainKHR(logicalDevice.getHandle(), &createInfo, nullptr, &swapChain));

		return makeBox<SwapChain>(swapChain, logicalDevice);
	}

	SwapChain::SwapChain(VkSwapchainKHR const swapChain, LogicalDevice& device) 
		: swapChain(swapChain)
		, logicalDevice(device)
	{

	}

	SwapChain::~SwapChain()
	{
		vkDestroySwapchainKHR(logicalDevice.getHandle(), swapChain, nullptr);
	}

	Expected<VkSurfaceFormatKHR> SwapChain::chooseSwapSurfaceFormat(const std::span<VkSurfaceFormatKHR>& availableFormats)
	{
		auto formats = IApplicationContext::getContext()->getProperty("graphics.vulkan.swapchain.format")
			.value_or(YAML::Node{})
			.as<std::vector<std::string>>();

		auto surfaceFormats = formats
			| std::views::transform(getVkValue<VkFormat>)
			| std::views::transform([&](auto&& format) {
					auto it = std::find_if(
						availableFormats.begin(),
						availableFormats.end(),
						[&](auto&& f) {return f.format == format; });
					if (it == availableFormats.end()) {
						return VkSurfaceFormatKHR{};
					}
					return *it;
				})
			| std::views::filter([](auto&& format) { return format.format != VkFormat{}; })
			| std::ranges::to<std::vector<VkSurfaceFormatKHR>>();

		if (surfaceFormats.empty()) {
			return std::unexpected("unable to find a suitable swap surface format");
		}
		return surfaceFormats.front();
	}

	VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::span<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) {
		if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
			return capabilities.currentExtent;
		}

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
		return {};
	}


}


