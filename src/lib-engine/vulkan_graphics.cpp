#include "vulkan_graphics.hpp"
#include <ranges>
#include <array>

using namespace e80;
using namespace e80::vulk;

namespace
{
	static auto requiredVulkanExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
	static auto validationLayers = { "VK_LAYER_KHRONOS_validation" };

}


VulkanGraphics::VulkanGraphics(const CreateInstanceInfo& info)
	: cii_(info)
{
}

std::expected<void, std::string> VulkanGraphics::initialize()
{
	TRY_EXPR(createInstance());


	return {};
}

bool VulkanGraphics::hasValidationLayerSupport() {
	u32 count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> layers(count);
	vkEnumerateInstanceLayerProperties(&count, layers.data());

	for (const auto& layer : layers) {
		if (strcmp("VK_LAYER_KHRONOS_validation", layer.layerName) == 0) {
			return true;
		}
	}
	return false;
}

strexpected<void> VulkanGraphics::createInstance() {

	auto extensions = getInstanceExtensions();
	if (!extensions.has_value()) {
		return std::unexpected(extensions.error());
	}

	//bool hasThem = hasRequiredExtentions(extensions);
	if (auto res = hasRequiredExtensions(extensions.value()); !res) {
		return std::unexpected("missing extensions");
	}

	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = cii_.appName.c_str(),
		.applicationVersion = VK_MAKE_VERSION(1,0,0),
		.pEngineName = "QuantaForge",
		.engineVersion = VK_MAKE_VERSION(1,0,0),
		.apiVersion = VK_API_VERSION_1_3,
	};

	VkInstanceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = static_cast<u32>(requiredVulkanExtensions.size()),
		.ppEnabledExtensionNames = requiredVulkanExtensions.begin(),
	};


	if (hasValidationLayerSupport()) {
		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = validationLayers.begin();
	}


	TRY_VKEXPR(vkCreateInstance(&createInfo, nullptr, &instance_));
	return {};
	
	//auto extensions = cii_.pi.lock()->getVulkanInstanceExtensions();
}

VulkanGraphics::~VulkanGraphics() 
{
	if (instance_) {
		vkDestroyInstance(instance_, nullptr);
	}

}

bool VulkanGraphics::hasRequiredExtensions(const std::vector<std::string>& extensions)
{
	u32 count = requiredVulkanExtensions.size();
	for (auto& k : requiredVulkanExtensions) {
		if (std::find(extensions.begin(), extensions.end(), k) != extensions.end()) {
			--count;
		}
	}
	return count == 0;
}

strexpected<std::vector<std::string>> VulkanGraphics::getInstanceExtensions() 
{
	u32 count;
	if (auto res = vkchk(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr)); !res.has_value()) {
		return std::unexpected(res.error());
	}

	if (count < 1) {
		return std::unexpected("unable to enumerate properties");
	}

	std::vector<VkExtensionProperties> extensionProperties(count);
	if (auto res = vkchk(vkEnumerateInstanceExtensionProperties(nullptr, &count, extensionProperties.data())); !res.has_value()) {
		return std::unexpected(res.error());
	}

	return extensionProperties
		| std::views::transform([](auto&& p) {	return std::string(p.extensionName); })
		| std::ranges::to<std::vector<std::string>>();

}

// --------------------------------------------------------------------------

template<>
ptr<Graphics> Graphics::createInstance<VulkanGraphics>(const CreateInstanceInfo& info) {
	auto g = std::make_shared<VulkanGraphics>(info);
	return g;
}

//IMPLEMENT_CLASS_FACTORY(e80::vulk::VulkanGraphics);
