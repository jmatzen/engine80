#pragma once

#include "graphics.hpp"
#include "platform_interface.hpp"
#include "vulk_result.hpp"
#include <vulkan/vulkan.h>
#include <optional>

namespace qf::vulk
{
	class PhysicalDevice;
	class LogicalDevice;
	class Surface;


	inline std::expected<void, std::string_view> vkchk(VkResult arg) {
		if (arg == VK_SUCCESS)
			return {};
		return std::unexpected(getStringForVkResult(arg));
	}


	class VulkanGraphics : public Graphics {

		VkInstance instance_{};
		VkDebugUtilsMessengerEXT debugMessenger_{};
		Box<Surface> surface_{};

		std::vector<std::string> requiredExtensions_{};
		std::vector<std::string> requiredDebugExtensions_{};
		std::vector<std::string> validationLayers_{};
		std::vector<std::string> requirdDeviceExtensions_{};

		bool useVulkanValidation_ = false;


		CreateInstanceInfo cii_;

		constexpr static UUID uuid = UUID("123e4567-e89b-12d3-a456-426614174000");

		[[nodiscard]]
		Expected<void> createInstance();

		[[nodiscard]]
		Expected<std::vector<std::string>> getInstanceExtensions();

		[[nodiscard]]
		bool hasRequiredExtensions(const std::vector<std::string>& extensions);

		[[nodiscard]] 
		static bool hasValidationLayerSupport();

		Expected<void> setupDebugLogging();

		Expected<void> pickPhysicalDevice();


		void config();

	public:
		VulkanGraphics(const CreateInstanceInfo& info);
		virtual ~VulkanGraphics() override;
		void dispose() override;

		virtual Expected<void> initialize() override;

		std::optional<VkInstance> getInstance() const 
		{
			if (instance_ == VK_NULL_HANDLE)
				return std::nullopt;
			return instance_;
		}

		Expected<intptr_t> getNativeWindowHandle() const;

		virtual std::optional<ptr<PlatformInterface>> getPlatform() const override;

		const Surface& getSurface() const {
			return *surface_;
		}

		auto& getRequiredDeviceExtensions() const { return requirdDeviceExtensions_; }

	};
}