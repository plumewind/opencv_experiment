cmake_minimum_required(VERSION 2.8)
project(0_opencvtest)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("/usr/include/ni/")

add_executable(0_opencvtest main.cpp)
target_link_libraries(0_opencvtest OpenNI ${OpenCV_LIBS})

install(TARGETS 0_opencvtest RUNTIME DESTINATION bin)
