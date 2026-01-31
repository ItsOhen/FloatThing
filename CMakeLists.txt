cmake_minimum_required(VERSION 3.27)

project(floatthing
    DESCRIPTION "Float things based on title"
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SRC})

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
)
target_link_libraries(${PROJECT_NAME} PRIVATE rt PkgConfig::deps)

install(TARGETS ${PROJECT_NAME})
