set(SOURCE main.cpp tsvec.h regionNodes.h)
set(PROJECT_NAME regionTree)

cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

set(SFML_DIR "/home/cyanmargh/Загрузки/SFML-compiled")
set(CMAKE_MODULE_PATH "/home/cyanmargh/Загрузки/SFML-compiled" ${CMAKE_MODULE_PATH})
set(SFML_STATIC_LIBRARIES TRUE)
set(CMAKE_CXX_FLAGS "-Os -flto -ffunction-sections -fno-asynchronous-unwind-tables -Wl,--strip-all -Wall -Wextra ")

SET(GCC_COVERAGE_COMPILE_FLAGS "-no-pie")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} sfml-graphics)