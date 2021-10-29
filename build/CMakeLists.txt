cmake_minimum_required(VERSION 3.5)

project(bruh LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    set (DCMAKE_CXX_COMPILER "g++-11")
    set(OPENMP_LIBRARIES "/usr/local/Cellar/llvm/13.0.0_1/lib")
    set(OPENMP_INCLUDES "/usr/local/Cellar/llvm/13.0.0_1/include")
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
endif()

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

if(APPLE AND USE_OpenMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-Wno-unused-command-line-argument")
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
      set(OpenMP_CXX_FLAGS "-Wno-unused-command-line-argument")
      set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
      set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

if(USE_OpenMP)
  find_package(OpenMP REQUIRED)
endif(USE_OpenMP)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(../src)
include_directories(../headers)
include_directories(../obj)
include_directories(../output)

set(SOURCE
    ../src/main.cpp
    ../src/matrices.cpp
    ../src/model.cpp
    ../src/renderer.cpp
    ../src/tgaimage.cpp
    ../src/vectors.cpp
)

add_executable(bruh ${SOURCE})