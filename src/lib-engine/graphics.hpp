#pragma once

#include "engine80.hpp"

namespace e80 {

	struct Graphics : public Serializable
	{
		virtual std::expected<void,std::string> initialize() = 0;

		struct CreateInstanceInfo {
			weak<PlatformInterface> pi;
			std::string appName;
		};

		template<typename T>
		[[nodiscard]]
		static ptr<Graphics> createInstance(const CreateInstanceInfo& info);
	};
}


