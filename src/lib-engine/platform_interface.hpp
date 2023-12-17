#pragma once

#include "engine80.hpp"

#include <vector>

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

		virtual std::expected<std::vector<std::string>, std::string> getVulkanInstanceExtensions() const = 0;
	};

}


