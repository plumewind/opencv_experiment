cmake_minimum_required( VERSION 2.8 )
project(2_hellokinect01)

# 寻找OpenNI库
#find_package( OpenCV REQUIRED )
# 添加头文件
#include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("/usr/include/ni/")

add_executable(2_hellokinect01 main.cpp)

# 链接OpenNI库
#target_link_libraries( 2_hellokinect01  ${OpenCV_LIBS} )
target_link_libraries( 2_hellokinect01 OpenNI)

install(TARGETS 2_hellokinect01 RUNTIME DESTINATION bin)
