#pragma once

#define SDL_MAIN_HANDLED

#include "uuid.hpp"
#include "noncopyable.hpp"

#include <format>
#include <memory>
#include <expected>

#include <glm/common.hpp>
#include <glm/vec3.hpp>

template<> struct std::formatter<glm::vec3> {
	constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	auto format(auto&& val, auto&& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "({},{},{})", val.x, val.y, val.z);
	}
};

//template<> struct std::formatter<e80::UUID> {
//	constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
//		return ctx.end();
//	}
//	auto format(auto&& val, auto&& ctx) -> decltype(ctx.out()) {
//		return format_to(ctx.out(), "{:x}-{:x}-{:x}-{:x}-{:x}{", val.x, val.y, val.z);
//	}
//
//};


namespace e80 {
	template<typename T>
	using ptr = std::shared_ptr<T>;

	template<typename T>
	using up = std::unique_ptr<T>;

	class Graphics;
	class PlatformInterface;
	class Archive;
	class Serializable;

}


namespace e80 
{
	class AbstractClassFactory;

	void registerFactories();

	template<typename T>
	void registerFactory();

	void registerFactory(const UUID& clsid, ptr<AbstractClassFactory> const& cf);

	ptr<Serializable> createInstance_(const UUID& clsid);

	template<typename T>
	ptr<T> createInstance(const UUID& clsid) {
		return std::static_pointer_cast<T>(createInstance_(clsid));
	}

	struct Result {
		enum class Code {
			OK = 0,
			FAILED,
			INIT_FAILED,
		} code_;

		Result(Code code)
			: code_(code) {}

		static Result ok() { return Code::OK; }
		static Result failed() { return Code::FAILED; }

	};


	template<typename T>
	class EnableSharedFromThis 
		: public std::enable_shared_from_this<T>
		, public NonCopyable {
	public:
		virtual ~EnableSharedFromThis() = default;

		template<typename U>
		ptr<U> sharedFromThis() {
			return std::static_pointer_cast<U>(this->shared_from_this());
		}

		template<typename U>
		ptr<U> staticCast() {
			return std::static_pointer_cast<U>(this->shared_from_this());
		}


	};


	class Serializable : public EnableSharedFromThis<Serializable> {
	public:
		virtual std::expected<void, Result> postConstruct() { return {}; }

		Serializable(Archive&);

		Serializable() = default;
	};



	//template<typename ClsId, typename T = ClsId::DefaultInterface>
	//auto CreateInstance() -> std::shared_ptr<T> {
	//	return CreateInstanceInternal<ClsId>()->cast<T>();
	//}

	class IApplicationContext;

	[[nodiscard]]
	static IApplicationContext& GetApplicationContext();

	
	template<typename T> [[nodiscard]] ptr<e80::Serializable> internalCreateInstance_() {
		auto obj = std::make_shared<T>();
		obj->postConstruct();
		return obj;
	}

	template<typename T, UUID const&> [[nodiscard]] ptr<e80::Serializable> internalCreateInstanceById_() {
		auto obj = std::make_shared<T>();
		obj->postConstruct();
		return obj;
	}

	template<typename T> ptr<e80::Serializable> internalCreateInstance();

	template<UUID const &> ptr<e80::Serializable> internalCreateInstanceById();

}

#define IMPLEMENT_CLASS_FACTORY(C) \
	template<> e80::ptr<e80::Serializable> e80::internalCreateInstance<C>() { \
		return e80::internalCreateInstance_<C>(); \
	}

#define IMPLEMENT_CLASS_FACTORY_UUID(C, ID) \
	template<> e80::ptr<e80::Serializable> e80::internalCreateInstanceById<ID>() { \
		return e80::internalCreateInstanceById_<C,ID>(); \
	}
