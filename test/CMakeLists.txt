cmake_minimum_required(VERSION 3.22)
set(PROJECT_NAME ArmoniK.Api.Experimental.Test)

set(CMAKE_CXX_STANDARD 14)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(ArmoniK.Api.Client CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ..)
target_link_libraries(${PROJECT_NAME} PUBLIC ArmoniK.Api.Common ArmoniK.Api.Client ArmoniK.Api.Experimental)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
