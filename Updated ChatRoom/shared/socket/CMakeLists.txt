cmake_minimum_required(VERSION "3.19.0")

project(SimpleSocket VERSION 1.0.0 DESCRIPTION "Simple socket library to wrap Winsock2")

add_library(SimpleSocket SHARED
    socket.cpp
    socket.h
)

set_target_properties(SimpleSocket PROPERTIES 
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER socket.h
)

# Define SIMPLESOCKET_EXPORT when building the library.
target_compile_definitions(SimpleSocket PRIVATE SIMPLESOCKET_EXPORT)

include(GNUInstallDirs)

install(TARGETS SimpleSocket
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(WIN32)
    target_link_libraries(SimpleSocket PRIVATE wsock32 ws2_32)
endif()
