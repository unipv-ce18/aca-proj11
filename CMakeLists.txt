cmake_minimum_required(VERSION 3.12)
project(aca_project CXX)

set(CMAKE_CXX_STANDARD 14)

set(HEADERS
        src/capabilities.h
        src/simd_props.h
        src/core/modes.h
        src/core/modes/operators.h
        src/core/modes/dump_plan.h
        src/morphology/process_parallel.h
        src/morphology/kernels.h
        src/morphology/reference.h
        src/morphology/operator_types.h
        src/morphology/StrEl.h
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
        src/capabilities.cpp
        src/core/modes.cpp
        src/core/modes/operators.cpp
        src/core/modes/dump_plan.cpp
        src/morphology/StrEl.cpp
        src/morphology/reference_dilate.cpp
        src/parallel/even_region.cpp
        src/parallel/ostream_dump_json.cpp
        src/parallel/planner_simd.cpp)

# Build options
option(MORPH_ENABLE_SIMD_SSE2 "Enable SSE2 support" ON)
option(MORPH_ENABLE_SIMD_AVX2 "Enable AVX2 support" ON)
option(MORPH_ENABLE_SIMD_AVX512F "Enable AVX512F support" ON)

# Debugging console output
option(MORPH_DEBUG_PLANNER "Enable debug messages for parallel planner" OFF)

# Site-local configuration
include(CMakeLists.local.txt OPTIONAL)

# Compiler configuration
if (MORPH_ENABLE_SIMD_AVX512F)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-mavx512f -mavx512bw)
    elseif (CMAKE_CXX_COMPILER_ID_MATCHES "MSVC")
        add_compile_options(/arch:AVX512F /arch:AVX512BW)
    endif ()
    add_compile_definitions(MORPH_ENABLE_SIMD_SSE2)
    add_compile_definitions(MORPH_ENABLE_SIMD_AVX2)
    add_compile_definitions(MORPH_ENABLE_SIMD_AVX512F)
    message(STATUS "Morph: Using AVX512F instruction set")

elseif (MORPH_ENABLE_SIMD_AVX2)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-mavx2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:AVX2)
    endif ()
    add_compile_definitions(MORPH_ENABLE_SIMD_SSE2)
    add_compile_definitions(MORPH_ENABLE_SIMD_AVX2)
    message(STATUS "Morph: Using AVX2 instruction set")

elseif (MORPH_ENABLE_SIMD_SSE2)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        add_compile_options(-msse2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options(/arch:SSE2)
    endif ()
    add_compile_definitions(MORPH_ENABLE_SIMD_SSE2)
    message(STATUS "Morph: Using SSE2 instruction set")

else ()
    if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
        add_compile_options(-mno-sse)
    endif ()
    message(STATUS "Morph: Using standard instruction set")

endif ()

# Configure debug output
if (MORPH_DEBUG_PLANNER)
    add_compile_definitions(MORPH_DEBUG_PLANNER)
endif ()

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs highgui)

add_executable(morph ${HEADERS} ${SOURCES})
target_link_libraries(morph PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(morph PUBLIC ${OpenCV_LIBS})
