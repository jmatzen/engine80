#include "vulk_logical_device.hpp"
#include "vulk_physical_device.hpp"
#include "vulkan_graphics.hpp"

#include <set>

namespace qf::vulk
{

	Expected<ptr<LogicalDevice>> LogicalDevice::create(PhysicalDevice& device)
	{
		auto logicalDevice = std::make_shared<LogicalDevice>(device);
		TRY_EXPR(logicalDevice->initialize());
		return logicalDevice;
	}

    // Initializes the logical device by setting up queue families and creating the device.
    Expected<void> LogicalDevice::initialize() {
        // Attempt to get a shared pointer to the physical device.
        auto physicalDevice = physicalDevice_.lock();
        if (!physicalDevice) {
            return std::unexpected("Physical device is no longer available");
        }

        auto queueFamilyResult = physicalDevice->findQueueFamilies();
        if (!queueFamilyResult) {
            return std::unexpected("Failed to find queue families");
        }

        auto indices = queueFamilyResult.value();
        // Check if the graphics and present queue families are valid, otherwise return an error.
        if (indices.graphics.empty() || indices.present.empty()) {
            return std::unexpected("missing valid queue families");
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

        // Define the features that the logical device will support.
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Populate the device create information structure.
        VkDeviceCreateInfo ci{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<u32>(createInfos.size()),
            .pQueueCreateInfos = createInfos.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        if (vkCreateDevice(physicalDevice->getVkPhysicalDevice(), &ci, nullptr, &device_) != VK_SUCCESS) {
            return std::unexpected("Failed to create logical device");
        }

        // Retrieve the queue handles for the graphics and present queues.
        vkGetDeviceQueue(device_, indices.graphics.front(), 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.present.front(), 0, &presentQueue_);

        // Return success if the logical device was created without any issues.
        return {};
    }

	LogicalDevice::LogicalDevice(PhysicalDevice& physicalDevice) :
		physicalDevice_(physicalDevice.shared_from_this()) 
	{
	}

	LogicalDevice::~LogicalDevice()
	{
		if (device_)
			vkDestroyDevice(device_, nullptr);
	}


}
