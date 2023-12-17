#include "application_context.hpp"
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace fs = std::filesystem;
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
		std::unordered_map<std::string, ptr<Serializable>> serviceMap_{};
		YAML::Node config_{};

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
	public:
		virtual std::expected<void, std::string> postConstruct() override
		{
			globalContext_ = this->sharedFromThis<ApplicationContext>();

			try {
				config_ = YAML::LoadFile("config.yaml");
			}
			catch (...) {
				return std::unexpected("failed to load config");
			}
			return {};
		}

		virtual std::optional<bool> getPropertyAsBool(const std::string_view& name) const override
		{
			auto keys = split(name, '.');
			YAML::Node node = config_;
			for (auto const& key : keys) {
				const std::string k(key.begin(), key.end());
				node = node[k.c_str()];
				if (not node) {
					return std::nullopt;
				}
			}
			if (node.IsScalar()) {
				return node.as<bool>();
			}
			return std::nullopt;
		}

		static std::vector<std::string_view> split(const std::string_view& value, char delim)
		{
			std::vector<std::string_view> result;
			auto start = value.begin();
			const auto end = value.end();
			while (start != end) {
				auto next = std::find(start, end, delim);
				result.emplace_back(start, next);
				start = next != end ? std::next(next) : next;
			}
			return result;
		}



	};

	IApplicationContext& GetApplicationContext() {
		return *globalContext_;
	}

	void IApplicationContext::SetApplicationContext(std::shared_ptr<IApplicationContext> const& context)
	{

	}


	ptr<IApplicationContext> IApplicationContext::getContext()
	{
		return globalContext_;
	}
}



IMPLEMENT_CLASS_FACTORY(qf::ApplicationContext);