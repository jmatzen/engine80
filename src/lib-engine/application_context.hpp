#pragma once

#include "engine80.hpp"
#include <optional>

namespace YAML {
	class Node;
}

namespace qf {
	class ApplicationContext;

	struct IApplicationContext : public Serializable {

		virtual Result RegisterService(std::shared_ptr<Serializable>&& obj, std::string&& contextName) = 0;

		virtual Result GetServiceByName(std::string const & contextName, std::shared_ptr<Serializable>& objOut) const = 0;

		[[nodiscard]]
		virtual std::shared_ptr<Serializable> 
			GetServiceByName(std::string const& serviceName) const = 0;

		virtual std::optional<bool> getPropertyAsBool(const std::string_view& name) const = 0;

		virtual std::optional<YAML::Node> getProperty(const std::string_view& name) const = 0;


		//template<typename T>
		//[[nodiscard]]
		//std::expected<std::shared_ptr<T>, void> GetServiceByName(std::string const & contextName) const {
		//	std::shared_ptr<Serializable> obj;
		//	auto res = GetServiceByName(std::move(contextName), obj);
		//	if (auto res = GetServiceByName(contextName); res.has_value()) {
		//		return std::static_pointer_cast<T>(res.value());
		//	}
		//	return std::unexpected();
		//}



		static void SetApplicationContext(std::shared_ptr<IApplicationContext> const & context);

		static ptr<IApplicationContext> getContext();
	};

	struct ApplicationContextClass {
		using DefaultInterface = IApplicationContext;
	};
}
