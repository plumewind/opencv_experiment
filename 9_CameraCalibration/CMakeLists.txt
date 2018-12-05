cmake_minimum_required(VERSION 2.8)
project(9_cameracalibration)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加OpenCV及openni头文件
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories ("/usr/include/ni/")

add_executable(9_cameracalibration main.cpp)
target_link_libraries(9_cameracalibration  ${OpenCV_LIBS} )

install(TARGETS 9_cameracalibration RUNTIME DESTINATION bin)
