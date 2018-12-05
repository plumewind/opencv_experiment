cmake_minimum_required(VERSION 2.8)
project(6_gesture1)

#添加nikiect类的库文件
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

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(6_gesture1 main.cpp)
target_link_libraries( 6_gesture1 NIKinect OpenNI ${OpenCV_LIBS} )

install(TARGETS 6_gesture1 RUNTIME DESTINATION bin)
