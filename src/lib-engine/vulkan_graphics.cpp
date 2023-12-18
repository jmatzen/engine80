#include "vulkan_graphics.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_logical_device.hpp"
#include "vulk_surface.hpp"
#include "application_context.hpp"
#include "logger.hpp"

#include <ranges>
#include <array>

using namespace qf;
using namespace qf::vulk;

namespace
{
	static auto requiredVulkanExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface"};
	static auto requiredDebugExtensions = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	static auto validationLayers = { "VK_LAYER_KHRONOS_validation" };
	static const std::string_view ENABLE_VALIDATION_PROPERTY_NAME{ "graphics.vulkan.enable-validation" };


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

void VulkanGraphics::config()
{
	auto ctx = IApplicationContext::getContext();
	this->useVulkanValidation_ = ctx->getPropertyAsBool(ENABLE_VALIDATION_PROPERTY_NAME).value_or(false);
}


Expected<void> VulkanGraphics::initialize()
{
	config();
	TRY_EXPR(createInstance());
	TRY_EXPR(setupDebugLogging());
	TRY_EXPR_(surface_, Surface::create(*this));
	TRY_EXPR_(physicalDevice_, PhysicalDevice::factory(*this));
	TRY_EXPR_(logicalDevice_, LogicalDevice::create(*physicalDevice_));
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

Expected<void> VulkanGraphics::createInstance() {

	Expected<std::vector<std::string>> extensions = getInstanceExtensions();
	TRY_EXPR(extensions);

	log::info("extensions: {}", extensions.value());

	if (auto res = hasRequiredExtensions(extensions.value()); !res) {
		return std::unexpected("missing required extensions");
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
	};

	std::vector<const char *> requiredExtensions(requiredVulkanExtensions.begin(), requiredVulkanExtensions.end());

	if (useVulkanValidation_)
	{
		if (hasValidationLayerSupport())
		{
			for (auto const& e : requiredDebugExtensions) {
				requiredExtensions.push_back(e);
			}
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.begin();
		}
		else
		{
			return std::unexpected("requested debug mode, but debug validation layer support doesn't exist");
		}
	}

	createInfo.enabledExtensionCount = requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();


	TRY_VKEXPR(vkCreateInstance(&createInfo, nullptr, &instance_));
	log::info("vkCreateInstance success!");

	return {};
}

VulkanGraphics::~VulkanGraphics() 
{
	if (debugMessenger_) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
		if (func) {
			func(instance_, debugMessenger_, nullptr);
		}
	}
	if (instance_) {
		vkDestroyInstance(instance_, nullptr);
	}
}

bool VulkanGraphics::hasRequiredExtensions(const std::vector<std::string>& extensions)
{
	for (const auto& requiredExtension : requiredVulkanExtensions) {
		if (std::find(extensions.begin(), extensions.end(), requiredExtension) == extensions.end()) {
			return false; // Required extension not found
		}
	}
	return true; // All required extensions found
}

Expected<std::vector<std::string>> VulkanGraphics::getInstanceExtensions() 
{
	u32 count;
	TRY_VKEXPR(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));

	if (count < 1) {
		return std::unexpected("unable to enumerate properties");
	}

	std::vector<VkExtensionProperties> extensionProperties(count);
	TRY_VKEXPR(vkEnumerateInstanceExtensionProperties(nullptr, &count, extensionProperties.data()));

	return extensionProperties
		| std::views::transform([](auto&& p) {	return std::string(p.extensionName); })
		| std::ranges::to<std::vector<std::string>>();

}

Expected<void> VulkanGraphics::setupDebugLogging()
{
	if (useVulkanValidation_) {
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
	}
	return {};
}

Expected<void> VulkanGraphics::pickPhysicalDevice()
{

	return {};
}

Expected<intptr_t> VulkanGraphics::getNativeWindowHandle() const {
	intptr_t h;
	TRY_EXPR_(h, cii_.pi.lock()->getNativeWindowHandle());
	return h;
}
// --------------------------------------------------------------------------

template<>
ptr<Graphics> Graphics::createInstance<VulkanGraphics>(const CreateInstanceInfo& info) {
	auto g = std::make_shared<VulkanGraphics>(info);
	return g;
}

//IMPLEMENT_CLASS_FACTORY(e80::vulk::VulkanGraphics);
