find_package(SDL2 CONFIG REQUIRED)
file(GLOB files *.hpp *.cpp)
add_executable(bin-client ${files})

target_link_libraries(bin-client PRIVATE glm::glm lib-engine )

target_link_libraries(bin-client
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)