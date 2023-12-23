#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace qf::vulk
{
	std::string_view getStringForVkResult(VkResult value);
	std::string_view getStringForVkFormat(VkFormat value);
	VkFormat getVkFormatFromString(const std::string_view& name);

}