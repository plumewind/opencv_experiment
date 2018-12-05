cmake_minimum_required(VERSION 2.8)
project(7_humanskeleton)

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

add_executable(7_humanskeleton main.cpp)
target_link_libraries( 7_humanskeleton NIKinect OpenNI ${OpenCV_LIBS} )

install(TARGETS 7_humanskeleton RUNTIME DESTINATION bin)
