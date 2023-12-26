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


#define TRY_EXPR_IGNORE_VALUE(expr) do { if (auto&& res = expr; not res.has_value()) return std::unexpected(Err(res.error().str())); } while (0); 


/*
* propagate a vkresult result as a string message error in a std::expected result
*/
#define TRY_VKEXPR(expr) do { if (auto res = (expr); res!=VK_SUCCESS) return std::unexpected(std::string(getStringForVkResult(res))); } while(0); 


#define TRY_EXPR(var,expr) \
	do { \
		auto temp = (expr); \
		if (!temp.has_value()) \
		return std::unexpected(temp.error()); \
		var = std::move(temp.value()); \
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
	using Box = std::unique_ptr<T>;

	template<typename T, typename... Args>
	Box<T> makeBox(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

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

	/**
	 * @brief A base class that enables shared ownership and access to the derived class instance.
	 *
	 * This class is used as a base class for classes that need to provide shared ownership and access to the derived class
	 * instance. It inherits from `std::enable_shared_from_this` and `NonCopyable` to enforce non-copyability.
	 *
	 * @tparam T The derived class type.
	 */
	template<typename T>
	class EnableSharedFromThis 
		: public std::enable_shared_from_this<T>
		, public NonCopyable {
	public:
		/**
		 * @brief Default destructor.
		 */
		virtual ~EnableSharedFromThis()
		{

		}

		/**
		 * @brief Returns a shared pointer to the derived class instance.
		 *
		 * This method returns a shared pointer to the derived class instance, casted to the specified type `U`.
		 *
		 * @tparam U The type to cast the shared pointer to.
		 * @return A shared pointer to the derived class instance.
		 */
		template<typename U=T>
		ptr<U> sharedFromThis() {
			return std::static_pointer_cast<U>(this->shared_from_this());
		}

		virtual void dispose() {};

	};

	template<typename T>
	struct Deleter_ {
		void operator()(T* p) const {
			if(p)
				p->dispose();
		}
	};
	template<typename T, typename... Args>
	std::shared_ptr<T> makeShared(Args&&... args) {
		return std::shared_ptr<T>(new T(args...), Deleter_<T>());
	}

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
