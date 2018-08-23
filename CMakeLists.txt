cmake_minimum_required(VERSION 3.10)
project(GameOfChance C)

include_directories(${PROJECT_SOURCE_DIR})

#target_link_libraries()

set(CMAKE_C_STANDARD 99)
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
set(SOURCE_FILES helper.h game_of_chance.c helper.c)

add_executable(Chance ${SOURCE_FILES})
