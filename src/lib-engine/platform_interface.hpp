#pragma once

#include "engine80.hpp"

#include <vector>
#include <optional>

namespace qf 
{
	class PlatformInterface : public Serializable
	{
	public:
		virtual std::expected<void,std::string> initialize() = 0;

		/*
		* flushes all platform events
		* returns false when the platform wants to end the program
		*/
		virtual bool update(float timeDelta) = 0;

		virtual Expected<intptr_t> getNativeWindowHandle() const = 0;

		virtual std::optional<std::tuple<int, int>> getWindowExtents() const = 0;
	};

}
