#pragma once

#include "engine80.hpp"
#include <optional>


namespace qf {

	struct PlatformInterface;

	struct Graphics : public Serializable
	{
		virtual Expected<void> initialize() = 0;

		virtual std::optional<ptr<PlatformInterface>> getPlatform() const = 0;

		struct CreateInstanceInfo {
			weak<PlatformInterface> pi;
			std::string appName;
		};

		template<typename T>
		[[nodiscard]]
		static ptr<Graphics> createInstance(const CreateInstanceInfo& info);


	};
}


