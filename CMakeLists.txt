cmake_minimum_required(VERSION 2.8.4)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(append_ycm_conf)

set(CMAKE_CXX_STANDARD 14)
add_executable(ycm-conf-generator "bin.cpp")

# thirdparty
add_definitions(-Dautojsonuselib)
if (NOT TARGET autojson)
    add_subdirectory(thirdparty/autojson)
endif ()

target_link_libraries(ycm-conf-generator autojson)

install(TARGETS ycm-conf-generator RUNTIME DESTINATION bin)
