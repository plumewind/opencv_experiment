cmake_minimum_required(VERSION 2.8)
project(4_humanskeleton)

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("/usr/include/ni/"  )

add_executable(4_humanskeleton main.cpp)
target_link_libraries( 4_humanskeleton OpenNI ${OpenCV_LIBS})

install(TARGETS 4_humanskeleton RUNTIME DESTINATION bin)
