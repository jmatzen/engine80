#pragma once

#include <cstdint>
#include <array>

namespace qf
{
	namespace detail
	{

		constexpr uint8_t nibbleToChar(const char c) {
			return (c >= '0' && c <= '9') ? (c - '0') :
				(c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
				(c >= 'A' && c <= 'F') ? (c - 'A' + 10) : 0;
		};

		constexpr uint32_t hex(const char* p, int bytes) {
			uint32_t res{};
			for (int i = 0; i != bytes; ++i, ++p)
				res = res << 4 | (nibbleToChar(*p));
			return res;
		}
	}

	struct UUID {
		uint32_t p0{};
		uint16_t p1{};
		uint16_t p2{};
		uint16_t p3{};
		uint8_t p4[6]{};

		constexpr UUID(const char* p) {
			p0 = detail::hex(p += 9, 8);
			p1 = detail::hex(p += 5, 4);
			p2 = detail::hex(p += 5, 4);
			p3 = detail::hex(p += 5, 4);
			for (int i = 0; i != 5; ++i)
				p4[i] = detail::hex(p += 2, 2);
		}

		constexpr UUID() = default;

		constexpr bool operator==(const UUID& other) const {
			auto a = (uint64_t[2])other.p0;
			auto b = (uint64_t[2])p0;
			return a[0] == b[0] && a[1] == b[1];
		}

	};

	class FourCC 
	{
		std::array<uint8_t, 4> value_;
	public:
		constexpr FourCC(const char p[4]) 
		{
			for (int i = 0; i != 4; ++i) value_[i] = p[i];
		}

		constexpr auto& get() const { return value_; }

		constexpr uint32_t getint() { return *(uint32_t*)value_.data(); }
	};
}

template<>
struct std::hash<qf::UUID>
{
	constexpr std::size_t operator()(const qf::UUID& k) const {
		const uint32_t* p = &k.p0;
		return p[0] ^ p[1] ^ p[2] ^ p[3];
	}
};