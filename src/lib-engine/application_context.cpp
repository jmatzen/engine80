#include "application_context.hpp"
#include <unordered_map>


namespace qf
{
	class ApplicationContext;
}

namespace {
	qf::ptr<qf::ApplicationContext> globalContext_;
}

namespace qf {
	class ApplicationContext
		: public IApplicationContext
	{
		std::unordered_map<std::string, ptr<Serializable>> serviceMap_;

		virtual Result RegisterService(std::shared_ptr<Serializable>&& obj, std::string&& contextName) override {
			auto [it, inserted] = serviceMap_.emplace(contextName, std::move(obj));
			return inserted ? Result::ok() : Result::failed();
		}

		virtual Result GetServiceByName(std::string const & contextName, std::shared_ptr<Serializable>& obj) const override {
			auto it = serviceMap_.find(contextName);
			if (it == serviceMap_.end()) {
				return Result::failed();
			}
			obj = it->second;
			return Result::ok();
		}

		virtual std::shared_ptr<Serializable> GetServiceByName(std::string const& serviceName) const override 
		{
			auto it = serviceMap_.find(serviceName);
			if (it == serviceMap_.end()) {
				return nullptr;
			}
			return it->second;
		}


	};

	IApplicationContext& GetApplicationContext() {
		return *globalContext_;
	}

	void IApplicationContext::SetApplicationContext(std::shared_ptr<IApplicationContext> const& context)
	{

	}
}



IMPLEMENT_CLASS_FACTORY(qf::ApplicationContext);