#include "graphics.hpp"
#include "platform_interface.hpp"

namespace e80::vulk
{
	class VulkanGraphics : public Graphics {
		std::weak_ptr<PlatformInterface> platform_;

		constexpr static UUID uuid = UUID("123e4567-e89b-12d3-a456-426614174000");



	public:
		VulkanGraphics(PlatformInterface& platform);

		virtual std::expected<void,std::exception> initialize() override;


	};
}