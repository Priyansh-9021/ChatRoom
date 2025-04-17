cmake_minimum_required(VERSION "3.19.0")

project(client)

add_executable(${PROJECT_NAME} client.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SimpleSocket)
