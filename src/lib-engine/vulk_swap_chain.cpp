#include "vulk_swap_chain.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_surface.hpp"
#include "application_context.hpp"
#include <yaml-cpp/yaml.h>
#include <ranges>

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

	auto SwapChain::createSwapChain(PhysicalDevice& device) -> Expected<ptr<SwapChain>> {

		SwapChainSupportDetails details;
		auto surface = device.getSurface();
		TRY_EXPR(details, SwapChain::querySwapChainSupport(
			device.getVkPhysicalDevice(),
			surface->getSurface()
		));


		VkSurfaceFormatKHR surfaceFormat;
		TRY_EXPR(surfaceFormat, chooseSwapSurfaceFormat(details.formats));
		VkPresentModeKHR presentMode;
		TRY_EXPR(presentMode, chooseSwapPresentMode(details.presentModes));
		//VkExtent2D extent = chooseSwapExtent(details.capabilities, 0, 0);

		return {};
	}

	SwapChain::SwapChain(VkSwapchainKHR const swapChain, PhysicalDevice& device) 
		: swapChain(swapChain)
		, physicalDevice(device.sharedFromThis())
	{

	}

	SwapChain::~SwapChain()
	{

	}


	Expected<VkSurfaceFormatKHR> SwapChain::chooseSwapSurfaceFormat(const std::span<VkSurfaceFormatKHR>& availableFormats)
	{
		auto formats = IApplicationContext::getContext()->getProperty("graphics.vulkan.swapchain.format")
			.value_or(YAML::Node{})
			.as<std::vector<std::string>>();

		auto surfaceFormats = formats
			| std::views::transform(getVkFormatFromString)
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

	Expected<VkPresentModeKHR> SwapChain::chooseSwapPresentMode(const std::span<VkPresentModeKHR>& availablePresentModes)
	{
		return {};
	}
}


