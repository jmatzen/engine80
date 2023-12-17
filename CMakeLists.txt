cmake_minimum_required(VERSION 3.27.1)
project(engine80)
find_package(glm CONFIG REQUIRED)

add_definitions(/std:c++latest)
add_definitions(/DSPDLOG_COMPILED_LIB)


include_directories(src)
add_subdirectory(src/bin-client)
add_subdirectory(src/lib-engine)
