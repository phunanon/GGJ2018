cmake_minimum_required(VERSION 3.7)

project(Infection)

# Define sources and executable
set(EXECUTABLE_NAME Infection)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.4 COMPONENTS system window graphics audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

add_executable(${EXECUTABLE_NAME} main.cpp math.cpp sound.cpp)

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
