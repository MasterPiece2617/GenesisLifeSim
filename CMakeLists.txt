cmake_minimum_required(VERSION 3.10)
project(MyImGuiProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(IMGUI_SRCS
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui-SFML.cpp
    #libs/imgui/backends/imgui_impl_opengl3.cpp
)

file (GLOB SIM_SRC simulator/src/*.cpp)
add_executable(my_app
    simulator/main.cpp
    ${SIM_SRC}
    ${IMGUI_SRCS}
)

# Exclude helper/tools that contain their own `main()` from the main executable.
# For example, `map_generator.cpp` is a standalone program used during development
# and must not be compiled into `my_app` (it defines `main`). Remove it from the
# globbed list if present.
list(REMOVE_ITEM SIM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/simulator/src/map_generator.cpp")


target_include_directories(my_app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/simulator/include
    #imgui/backends
)

target_link_libraries(my_app PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

# Ensure runtime asset folders are available next to the built executable so
# code that loads "resources/..." or "textures/..." will find them when
# running from the build directory.
add_custom_command(TARGET my_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:my_app>/maps
)

# Also copy top-level textures folder if present.
add_custom_command(TARGET my_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:my_app>/textures
)
