#include "application_context.hpp"
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <functional>

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

		virtual Result GetServiceByName(std::string const& contextName, std::shared_ptr<Serializable>& obj) const override {
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
			auto node = getProperty(name);
			if (node.has_value())
				return node.value().as<bool>();
			return std::nullopt;
		}

		virtual std::optional<YAML::Node> getProperty(const std::string_view& name) const override
		{
			std::function<std::optional<YAML::Node>(const YAML::Node& node, std::vector<std::string_view>&,int)> f;

			f = [&](const YAML::Node& node, std::vector<std::string_view>& keys, int i)->std::optional<YAML::Node> {
				if (i == keys.size()) {
					return node;
				}
				const std::string temp(keys[i].begin(), keys[i].end());
				const YAML::Node child = node[temp];
				if (not child) {
					return std::nullopt;
				}
				return f(child, keys, i + 1);
				};

			auto keys = split(name, '.');
			auto result = f(config_, keys, 0);
			return result;
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