cmake_minimum_required(VERSION 2.8.3)
project(MyWebServer CXX C)

#设置编译类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 要记得链接OpenCV的库哦
find_package(OpenCV)
# 向工程中添加特定的头文件搜索路径，相当于-I参数
include_directories(
    Base Http Logging Timer Util ${OpenCV}
)

# 设置g++编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -w -std=c++11 -fsanitize=address")

# 将CMAKE_BUILD_TYPE转换为大写
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
# 设置一些无关经要的优化参数
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 链接多线程库
find_package(Threads REQUIRED)

# 向用户打印一些消息,正常消息
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})
message(STATUS "CMAKE_THREAD_LIBS_INIT = " ${CMAKE_THREAD_LIBS_INIT})

# 寻找每个目录的源文件，并合并为变量
aux_source_directory(Base SRC_LIST1)
aux_source_directory(Http SRC_LIST2)
aux_source_directory(Timer SRC_LIST3)
aux_source_directory(Logging SRC_LIST4)
aux_source_directory(Util SRC_LIST5)

# 将可执行文件输出路径设置为当前工程的顶层目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# 生成可执行文件
add_executable(WebServer main.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})
# 为可执行文件添加链接库
target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBRARIES})







