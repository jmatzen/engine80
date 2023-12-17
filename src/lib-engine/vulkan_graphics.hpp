#include "graphics.hpp"
#include "platform_interface.hpp"
#include <vulkan/vulkan.h>

namespace qf::vulk
{
	template<typename T>
	using strexpected = std::expected<T, std::string>;

	std::string const& getStringForVkResult(VkResult value);

	inline std::expected<void, std::string> vkchk(VkResult arg) {
		if (arg == VK_SUCCESS)
			return {};
		return std::unexpected(getStringForVkResult(arg));
	}


	class VulkanGraphics : public Graphics {

		VkInstance instance_{};
		VkDebugUtilsMessengerEXT debugMessenger_{};

		CreateInstanceInfo cii_;

		constexpr static UUID uuid = UUID("123e4567-e89b-12d3-a456-426614174000");

		[[nodiscard]]
		strexpected<void> createInstance();

		[[nodiscard]]
		static strexpected<std::vector<std::string>> getInstanceExtensions();

		[[nodiscard]]
		static bool hasRequiredExtensions(const std::vector<std::string>& extensions);

		[[nodiscard]] 
		static bool hasValidationLayerSupport();

		strexpected<void> setupDebugLogging();

	public:
		VulkanGraphics(const CreateInstanceInfo& info);
		virtual ~VulkanGraphics() override;

		virtual std::expected<void,std::string> initialize() override;


	};
}