cmake_minimum_required(VERSION 3.22)
set(PROJECT_NAME ArmoniK.Api.Experimental)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(ArmoniK.Api.Client CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC armonik/api/experimental/experimental.h experimental.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${PROJECT_NAME} PUBLIC ArmoniK.Api.Common ArmoniK.Api.Client)

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION 0.0.1)
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)
set_property(TARGET ${PROJECT_NAME} PROPERTY
        INTERFACE_${PROJECT_NAME}_MAJOR_VERSION 0)
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
        COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "0.0.1"
        COMPATIBILITY AnyNewerVersion
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/armonik/api/experimental/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/armonik/api/experimental FILES_MATCHING PATTERN "*.h")

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        PATH_VARS CMAKE_INSTALL_PREFIX CMAKE_INSTALL_LIBDIR
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

add_subdirectory(test)