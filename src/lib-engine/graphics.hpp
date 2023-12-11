#pragma once

#include "engine80.hpp"

namespace e80 {

	struct Graphics : public Serializable
	{
		virtual std::expected<void,std::exception> initialize() = 0;
	};
}


