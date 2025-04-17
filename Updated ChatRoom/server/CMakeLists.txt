cmake_minimum_required(VERSION "3.19.0")

project(server)

add_executable(${PROJECT_NAME} server.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SimpleSocket)
