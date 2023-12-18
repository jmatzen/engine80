#pragma once

#define SDL_MAIN_HANDLED

#include "uuid.hpp"
#include "noncopyable.hpp"

#include <format>
#include <memory>
#include <expected>
#include <vector>

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

template<typename T>
struct std::formatter<std::vector<T>> {
	constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	auto format(auto&& val, auto&& ctx) -> decltype(ctx.out()) {
		std::string res = "[\n";
		for (auto const& item : val) {
			res += "   \"" + std::format("{}",item) + "\",\n";
		}
		res += "]\n";
		return format_to(ctx.out(), "{}", res);
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


#define TRY_EXPR(expr) do { if (auto&& res = expr; not res.has_value()) return std::unexpected(Err(res.error().str())); } while (0); 


/*
* propagate a vkresult result as a string message error in a std::expected result
*/
#define TRY_VKEXPR(expr) do { if (auto&& res = expr; res!=VK_SUCCESS) return std::unexpected(getStringForVkResult(res)); } while(0); 


#define TRY_EXPR_(var,expr) \
	do { \
		auto temp = (expr); \
		if (!temp.has_value()) \
		return std::unexpected(temp.error()); \
		var = temp.value(); \
	} while (0);

namespace qf {
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using s8 = int8_t;
	using s16 = int16_t;
	using s32 = int32_t;
	using s64 = int64_t;

	template<typename T>
	using ptr = std::shared_ptr<T>;

	template<typename T>
	using up = std::unique_ptr<T>;

	template<typename T>
	using weak = std::weak_ptr<T>;

	class Graphics;
	class PlatformInterface;
	class Archive;
	class Serializable;

	//using Err = std::string;

	class Err
	{
		std::string err_;
	public:
		Err(const char* val)
			: err_(val) {}

		Err(std::string const& val)
			: err_(val) {}

		Err(std::string&& val)
			: err_(std::move(val)) {}

		Err(Err&& e)
			: err_(std::move(e.err_)) {}

		Err(const Err& e)
			: err_(e.err_) {}



		std::string str() const { return err_; }
	};

	template<typename T>
	using Expected = std::expected<T, Err>;

	//template<typename... Args>
	//std::unexpected<Args...> Unexpected(Args&&... args) {
	//	return std::unexpected(args...);
	//}

	template<typename T>
	auto Unexpected(T&& arg) -> std::unexpected<T> {
		return std::unexpected(std::forward<T>(arg));
	}
}


namespace qf 
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
		virtual std::expected<void, std::string> postConstruct() { return {}; }

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

	
	template<typename T> [[nodiscard]] ptr<qf::Serializable> internalCreateInstance_() {
		auto obj = std::make_shared<T>();
		obj->postConstruct();
		return obj;
	}

	template<typename T, UUID const&> [[nodiscard]] ptr<qf::Serializable> internalCreateInstanceById_() {
		auto obj = std::make_shared<T>();
		obj->postConstruct();
		return obj;
	}

	template<typename T> ptr<qf::Serializable> internalCreateInstance();

	template<UUID const &> ptr<qf::Serializable> internalCreateInstanceById();

}

#define IMPLEMENT_CLASS_FACTORY(C) \
	template<> qf::ptr<qf::Serializable> qf::internalCreateInstance<C>() { \
		return qf::internalCreateInstance_<C>(); \
	}

#define IMPLEMENT_CLASS_FACTORY_UUID(C, ID) \
	template<> qf::ptr<qf::Serializable> qf::internalCreateInstanceById<ID>() { \
		return qf::internalCreateInstanceById_<C,ID>(); \
	}
