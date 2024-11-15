cmake_minimum_required(VERSION 3.10)

project(cpp_hw)


find_package(Qt6 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

qt_add_executable(cpp_hw
    main.cpp mc_resolver.hpp mc_client.hpp
)

target_link_libraries(cpp_hw PRIVATE Qt6::Core Qt6::Network)
target_include_directories(cpp_hw PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
