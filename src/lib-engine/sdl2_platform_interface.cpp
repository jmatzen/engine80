#include "platform_interface.hpp"
#include "graphics.hpp"
#include "class_ids.hpp"
#include "class_factory.hpp"

#include <thread>
#include <chrono>


namespace e80 {
	class Sdl2PlatformInterface
		: public PlatformInterface
	{
		static inline const char* CLASS_NAME = "Sdl2PlatformInterface";

	public:
		virtual Result initialize() override {
			return Result::ok();
		}

		virtual Result update(float timeDelta) override {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return Result::ok();
		}

		virtual std::expected<void, Result> postConstruct() override {
			return {};
		}

	};
}

IMPLEMENT_CLASS_FACTORY(e80::Sdl2PlatformInterface);

template<>
void e80::registerFactory<e80::Sdl2PlatformInterface>() {
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
