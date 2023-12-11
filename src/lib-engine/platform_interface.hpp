#pragma once

#include "engine80.hpp"

namespace e80 
{
	class PlatformInterface : public Serializable
	{
	public:
		virtual Result initialize() = 0;
		virtual Result update(float timeDelta) = 0;
	};

}


