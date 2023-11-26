file(GLOB files *.hpp *.cpp)
add_library(lib-engine ${files})
target_link_libraries(lib-engine PRIVATE glm::glm)
