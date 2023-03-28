cmake_minimum_required(VERSION 3.16.3)

project(WebServer1.0)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -g")
SET(CMAKE_CXX_STANDARD 17)

aux_source_directory(./src DIR_SRC)
include_directories(./include)

add_executable(WebServer1.0 ${DIR_SRC})


include(FetchContent)
FetchContent_Declare(spdlog #库名字
    GIT_REPOSITORY git@github.com:gabime/spdlog.git #仓库地址，这里采用的是ssh版本
    GIT_TAG v1.x
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/spdlog
)
FetchContent_MakeAvailable(spdlog)

#链接库spdlog
target_link_libraries(WebServer1.0 PRIVATE spdlog::spdlog)