##
# The MIT License (MIT)
#
# Copyright (c) 2015 Diego Stamigni
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
##

cmake_minimum_required (VERSION 2.8)
project (thread_pool_demo)

add_compile_options(-std=c++1y -stdlib=libc++)
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/3rdpart/priority_queue/include")

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE headers include⁄*.hpp)

set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost QUIET COMPONENTS 
  system 
  thread
REQUIRED)
include_directories("${Boost_INCLUDE_DIR}")

set (LIST_SOURCES ${headers} ${sources})
add_executable(${PROJECT_NAME} ${LIST_SOURCES})
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
