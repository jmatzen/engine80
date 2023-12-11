file(GLOB files *.hpp *.cpp)
add_library(lib-engine ${files})
target_link_libraries(lib-engine PRIVATE glm::glm)


find_package(Vulkan) # https://cmake.org/cmake/help/latest/module/FindVulkan.html, CMake 3.21+
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(lib-engine PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)