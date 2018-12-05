cmake_minimum_required(VERSION 2.8)
project(2_hellokinect03)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("/usr/include/ni/")

add_executable(2_hellokinect03 main.cpp)
target_link_libraries( 2_hellokinect03 OpenNI ${OpenCV_LIBS})

install(TARGETS 2_hellokinect03 RUNTIME DESTINATION bin)
