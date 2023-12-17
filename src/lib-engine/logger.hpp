#pragma once

#include <format>
#include <iostream>

namespace qf
{
	namespace log
	{
		template<typename... Args>
		void info(std::format_string<Args...> f, Args&&... args) {
			std::cout << std::format(f, args...) << std::endl;
		}
	}
}