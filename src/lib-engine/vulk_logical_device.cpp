#include "vulk_logical_device.hpp"
#include "vulk_physical_device.hpp"
#include "vulk_graphics.hpp"
#include "vulk_surface.hpp"
#include "vulk_swap_chain.hpp"
#include "vulk_graphics.hpp"
#include "application_context.hpp"

#include <yaml-cpp/yaml.h>
#include <set>
#include <ranges>
#include <functional>


namespace qf::vulk
{
    static constexpr std::string_view REQUIRED_DEVICE_EXTENSIONS_PROP_NAME{ "graphics.vulkan.required-extensions.device" };


    LogicalDevice::LogicalDevice(PhysicalDevice& physicalDevice) :
        physicalDevice_(physicalDevice)
    {
    }

    LogicalDevice::~LogicalDevice()
    {
        swapChain_.reset();
        if (device_)
            vkDestroyDevice(device_, nullptr);
    }

	Expected<Box<LogicalDevice>> LogicalDevice::create(PhysicalDevice& device)
	{
		auto logicalDevice = makeBox<LogicalDevice>(device);
		TRY_EXPR_IGNORE_VALUE(logicalDevice->initialize());
		return logicalDevice;
	}
    // Initializes the logical device by setting up queue families and creating the device.
    Expected<void> LogicalDevice::initialize() {
        // Get the required device extensions from the application context.
        requiredDeviceExtensions_ = IApplicationContext::getContext()->getProperty(REQUIRED_DEVICE_EXTENSIONS_PROP_NAME)
            .value_or(YAML::Node{})
            .as<std::vector<std::string>>();


        // Find the queue families supported by the physical device.
        auto queueFamilyResult = physicalDevice_.findQueueFamilies();
        if (!queueFamilyResult) {
            return std::unexpected("Failed to find queue families");
        }
        auto indices = queueFamilyResult.value();

        // Check if the graphics and present queue families are valid, otherwise return an error.
        if (indices.graphics.empty() || indices.present.empty()) {
            return std::unexpected("Missing valid queue families");
        }

        // Create a set of unique queue families to avoid creating duplicate queues.
        auto uniqueFamilies = std::set<u32>{
            indices.graphics.front(),
            indices.present.front()
        };

        // Prepare a list to store the queue create information structures.
        std::vector<VkDeviceQueueCreateInfo> createInfos;

        // Set the priority for the device queues.
        float queuePriority = 1.f;
        for (u32 family : uniqueFamilies) {
            // Populate the queue create information for each unique family.
            VkDeviceQueueCreateInfo queueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = family,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority
            };

            // Add the queue create information to the list.
            createInfos.emplace_back(queueCreateInfo);
        }

        // Check device extension support.
        TRY_EXPR_IGNORE_VALUE(checkDeviceExtensionSupport());

        // Define the features that the logical device will support.
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Populate the device create information structure.
        VkDeviceCreateInfo ci{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<u32>(createInfos.size()),
            .pQueueCreateInfos = createInfos.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        // Get the names of the required device extensions.
        auto names = physicalDevice_.getGraphics().getRequiredDeviceExtensions()
            | std::views::transform(std::mem_fn(&std::string::c_str))
            | std::ranges::to<std::vector<const char*>>();

        // Set the enabled extension names in the device create information structure.
        ci.enabledExtensionCount = names.size();
        ci.ppEnabledExtensionNames = names.data();

        // Create the logical device.
        if (vkCreateDevice(physicalDevice_.getVkPhysicalDevice(), &ci, nullptr, &device_) != VK_SUCCESS) {
            return std::unexpected("Failed to create logical device");
        }

        // Retrieve the queue handles for the graphics and present queues.
        vkGetDeviceQueue(device_, indices.graphics.front(), 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.present.front(), 0, &presentQueue_);

        // Create the swap chain.
        TRY_EXPR(swapChain_, SwapChain::createSwapChain(*this));

        // Return success if the logical device was created without any issues.
        return {};
    }

    Expected<void> LogicalDevice::checkDeviceExtensionSupport() const {

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(requiredDeviceExtensions_.begin(), requiredDeviceExtensions_.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        if (!requiredExtensions.empty()) {
            return std::unexpected(std::format("device does not have required extensions"));
        }

        return {};
    }

}
