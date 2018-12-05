cmake_minimum_required(VERSION 2.8)
project(5_trackbar)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("/usr/include/ni/")

add_executable(5_trackbar main.cpp)
target_link_libraries(5_trackbar OpenNI ${OpenCV_LIBS})

install(TARGETS 5_trackbar RUNTIME DESTINATION bin)
