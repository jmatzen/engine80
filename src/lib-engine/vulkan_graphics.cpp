#include "vulkan_graphics.hpp"
#include "logger.hpp"

#include <ranges>
#include <array>

using namespace qf;
using namespace qf::vulk;

namespace
{
	static auto requiredVulkanExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	static auto validationLayers = { "VK_LAYER_KHRONOS_validation" };
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	log::info("{}", pCallbackData->pMessage);

	return VK_FALSE;
}

template<>
struct std::formatter<VkLayerProperties> {
	constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	auto format(auto&& val, auto&& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "{}", val.layerName);
	}
};

VulkanGraphics::VulkanGraphics(const CreateInstanceInfo& info)
	: cii_(info)
{
	log::info("creating vulkan");
}

std::expected<void, std::string> VulkanGraphics::initialize()
{
	TRY_EXPR(createInstance());
	TRY_EXPR(setupDebugLogging());


	return {};
}

bool VulkanGraphics::hasValidationLayerSupport() {
	u32 count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> layers(count);
	vkEnumerateInstanceLayerProperties(&count, layers.data());
	log::info("instance layers: {}", layers);

	for (const auto& layer : layers) {
		if (strcmp("VK_LAYER_KHRONOS_validation", layer.layerName) == 0) {
			log::info("has validation");
			return true;
		}
	}
	log::info("missing validation layer");
	return false;
}

strexpected<void> VulkanGraphics::createInstance() {

	auto extensions = getInstanceExtensions();
	if (!extensions.has_value()) {
		return std::unexpected(extensions.error());
	}

	log::info("extensions: {}", extensions.value());

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
	if (debugMessenger_) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance_, debugMessenger_, nullptr);
		}
	}
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

strexpected<void> VulkanGraphics::setupDebugLogging()
{
	VkDebugUtilsMessengerCreateInfoEXT ci{};
	ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	ci.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
	if (func) {
		auto result = func(instance_, &ci, nullptr, &debugMessenger_);
		TRY_VKEXPR(result);
	}
	return {};
}
// --------------------------------------------------------------------------

template<>
ptr<Graphics> Graphics::createInstance<VulkanGraphics>(const CreateInstanceInfo& info) {
	auto g = std::make_shared<VulkanGraphics>(info);
	return g;
}

//IMPLEMENT_CLASS_FACTORY(e80::vulk::VulkanGraphics);
