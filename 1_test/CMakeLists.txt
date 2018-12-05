cmake_minimum_required( VERSION 2.8 )
project( 1_test )

# 添加c++ 11标准支持
set( CMAKE_CXX_FLAGS "-std=c++11" )

# 寻找OpenCV库
find_package( OpenCV 3.2 REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( 1_test 1_HelloOpenCV.cpp )
# 链接OpenCV库
target_link_libraries( 1_test ${OpenCV_LIBS} )
