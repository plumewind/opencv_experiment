cmake_minimum_required(VERSION 2.8)
project(8_handtrack)

# 添加c++ 11标准支持
#set( CMAKE_CXX_FLAGS "-std=c++9" )

#添加NIkiect类的库文件
set(library_sources
    NIKinect.cpp
    NIKinect.h
)
add_library(NIKinect SHARED ${library_sources})

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加OpenCV及openni头文件
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories ("/usr/include/ni/")

add_executable(8_handtrack main.cpp)
target_link_libraries(8_handtrack  NIKinect OpenNI ${OpenCV_LIBS} )

install(TARGETS 8_handtrack RUNTIME DESTINATION bin)
