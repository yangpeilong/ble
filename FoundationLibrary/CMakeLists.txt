# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

#将在gcc命令行添加DEBUG宏定义
#add_definitions(-DDEBUG)

#access library
ADD_SUBDIRECTORY(src/main/jni/access)

#secure library
#ADD_SUBDIRECTORY(src/main/jni/secure)