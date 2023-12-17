#pragma once

namespace qf
{
	class NonCopyable
	{
		NonCopyable(NonCopyable&) = delete;
		NonCopyable& operator=(NonCopyable&) = delete;
	public:
		NonCopyable() {}
	};
}