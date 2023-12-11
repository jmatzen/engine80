#pragma once

namespace e80
{
	class NonCopyable
	{
		NonCopyable(NonCopyable&) = delete;
		NonCopyable& operator=(NonCopyable&) = delete;
	public:
		NonCopyable() {}
	};
}