#include "platform_interface.hpp"
#include "graphics.hpp"
#include "class_ids.hpp"
#include "class_factory.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <thread>
#include <chrono>
#include <ranges>

namespace qf {
	class Sdl2PlatformInterface
		: public PlatformInterface
	{
		static inline const char* CLASS_NAME = "Sdl2PlatformInterface";
		SDL_Window* window_{};

	public:
		virtual std::expected<void, std::string> initialize() override {
			int result;
			if ((result = SDL_Init(SDL_INIT_VIDEO)))
			{
				return std::unexpected(SDL_GetError());
			}

			uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_VULKAN;

			window_ = SDL_CreateWindow("game"
				, SDL_WINDOWPOS_CENTERED
				, SDL_WINDOWPOS_CENTERED
				, 1920
				, 1080
				, flags)
				;

			if (window_ == nullptr) {
				return std::unexpected(SDL_GetError());
			}

			return {};
		}

		virtual bool update(float timeDelta) override {
			SDL_Event e{};
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					return false;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return true;
		}


		virtual std::expected<std::vector<std::string>, std::string> getVulkanInstanceExtensions() const override {
			u32 count;
			SDL_Vulkan_GetInstanceExtensions(window_, &count, nullptr);
			std::vector<const char*> arr(count);
			SDL_Vulkan_GetInstanceExtensions(window_, &count, arr.data());
			return arr 
				| std::views::transform([](auto&& p) {	return std::string(p);}) 
				| std::ranges::to<std::vector<std::string>>();
			//return arr 
			//	| std::views::transform(std::string{})
			//	| std::ranges::to<std::vector<std::string>>();
		}
	};
}

IMPLEMENT_CLASS_FACTORY(qf::Sdl2PlatformInterface);

template<>
void qf::registerFactory<qf::Sdl2PlatformInterface>() {
	//class Sdl2PlatformInterfaceClassFactory : public ClassFactory
	//{
	//	virtual ptr<Serializable> create() const override {
	//		auto obj = std::make_shared<Sdl2PlatformInterface>();
	//		if (obj) {
	//			if (auto res = obj->postConstruct(); res.has_value()) {
	//				return obj;
	//			}
	//		}
	//		return nullptr;
	//	}
	//};
	registerFactory(Sdl2PlatformInterfaceClassId, std::make_shared<TClassFactory<Sdl2PlatformInterface>>());
}
