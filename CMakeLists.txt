# Copyright Moses Turner, 2021. SPDX-License-Identifier: BSL-1.0
cmake_minimum_required(VERSION 3.10)

# You probably want C++17 for now
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# So that Intellisense/clangd know how your project is compiled
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")


# Uncomment all these if you want address sanitizers:

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -march=native -O3 -fno-omit-frame-pointer -fsanitize=address")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -march=native -O3  -fno-omit-frame-pointer -fsanitize=address")

#set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_VERBOSE_MAKEFILE ON)

project(
  sk-webcam-example
  VERSION "0.0.1"
  LANGUAGES CXX C)

find_package(StereoKitC REQUIRED)
find_package(
  OpenCV
  COMPONENTS core
             calib3d
             highgui
             imgproc
             imgcodecs
             features2d
             video
             videoio
  REQUIRED)

add_library(our-pthreads INTERFACE)
if(WIN32)
  find_package(pthreads_windows REQUIRED)
  target_link_libraries(our-pthreads INTERFACE PThreads4W::PThreads4W_CXXEXC)
else()
  set(CMAKE_THREAD_PREFER_PTHREAD ON)
  find_package(Threads)
  target_link_libraries(our-pthreads INTERFACE Threads::Threads)
endif()

message("Found StereoKitC " ${StereoKitC_Found})

option(FORCE_COLORED_OUTPUT
       "Always produce ANSI-colored output (GNU/Clang only)." TRUE)

if("${FORCE_COLORED_OUTPUT}")
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

add_executable(sk-webcam-example src/main.cpp)

target_link_libraries(sk-webcam-example PRIVATE -L/usr/local/lib StereoKitC
                                         ${OpenCV_LIBRARIES} our-pthreads)

target_include_directories(sk-webcam-example PRIVATE ${OpenCV_INCLUDE_DIRS})
