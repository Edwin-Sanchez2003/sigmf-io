cmake_minimum_required(VERSION 3.16)

project(BurstRFBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(BurstRFBackend main.cpp)

include(GNUInstallDirs)
install(TARGETS BurstRFBackend
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
