cmake_minimum_required(VERSION 3.12)
project(aca_project CXX)

set(CMAKE_CXX_STANDARD 14)

set(HEADERS
        src/core/modes.h
        src/core/modes/dilate.h
        src/core/modes/dump_plan.h
        src/core/modes/erode.h
        src/morphology/dilate_parallel.h
        src/morphology/StrEl.h
        src/morphology/operators.h
        src/parallel/Chunk.h
        src/parallel/even_region.h
        src/parallel/ostream_dump.h
        src/parallel/Plan.h
        src/parallel/planners.h
        src/parallel/CoreAllocation.h
        src/parallel/Rect.h
        src/parallel/RegionSpec.h)

set(SOURCES
        src/morph.cpp
        src/core/modes.cpp
        src/core/modes/dilate.cpp
        src/core/modes/dump_plan.cpp
        src/core/modes/erode.cpp
        src/morphology/dilate_parallel.cpp
        src/morphology/StrEl.cpp
        src/morphology/operators.cpp
        src/parallel/even_region.cpp
        src/parallel/ostream_dump_json.cpp
        src/parallel/planner_simd.cpp)

# Build options
option(MORPH_ENABLE_SIMD_SSE2 "Enable SSE2 support" ON)
option(MORPH_ENABLE_SIMD_AVX2 "Enable AVX2 support" OFF)

# Site-local configuration
include(CMakeLists.local.txt OPTIONAL)

# Compiler configuration
if (MORPH_ENABLE_SIMD_AVX2)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-mavx2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:AVX2)
    endif ()
    message(STATUS "Morph: Using AVX2 instruction set")

elseif (MORPH_ENABLE_SIMD_SSE2)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-msse2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:SSE2)
    endif ()
    message(STATUS "Morph: Using SSE2 instruction set")

else ()
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
        add_compile_options(-mno-sse)
    endif ()
    message(STATUS "Morph: Using standard instruction set")

endif ()

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(morph ${HEADERS} ${SOURCES})
target_link_libraries(morph PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(morph PUBLIC ${OpenCV_LIBS})
