cmake_minimum_required(VERSION 2.8)
project(3_gesturerecognition)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories ("/usr/include/ni/"  )
add_executable(3_gesturerecognition main.cpp)
target_link_libraries( 3_gesturerecognition OpenNI ${OpenCV_LIBS})

install(TARGETS 3_gesturerecognition RUNTIME DESTINATION bin)
