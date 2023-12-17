#pragma once

#include "engine80.hpp"

namespace qf {

	struct Graphics : public Serializable
	{
		virtual Expected<void> initialize() = 0;

		struct CreateInstanceInfo {
			weak<PlatformInterface> pi;
			std::string appName;
		};

		template<typename T>
		[[nodiscard]]
		static ptr<Graphics> createInstance(const CreateInstanceInfo& info);
	};
}


