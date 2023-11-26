#pragma once

#define SDL_MAIN_HANDLED

#include <format>

#include <glm/common.hpp>
#include <glm/vec3.hpp>

template<> struct std::formatter<glm::vec3> {
	constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}
	auto format(auto&& val, auto&& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "({},{},{})", val.x, val.y, val.z);
	}
};
