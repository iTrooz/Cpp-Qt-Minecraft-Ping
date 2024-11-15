cmake_minimum_required(VERSION 3.10)

project(mcping)


find_package(Qt6 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

qt_add_executable(mcping
    main.cpp mc_resolver.hpp mc_client.hpp
)

target_link_libraries(mcping PRIVATE Qt6::Core Qt6::Network)
target_include_directories(mcping PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
