cmake_minimum_required(VERSION 3.16)
project(DJMugumyaProSoundEnhancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DJMugumyaProSoundEnhancer SHARED
    DJMugumyaProSoundEnhancer.cpp
    DJMugumyaProSoundEnhancer.h
    AudioDSP.cpp
    AudioDSP.h
    include/vdjPlugin8.h
    include/vdjDsp8.h
    DJMugumyaProSoundEnhancer.def
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(DJMugumyaProSoundEnhancer PROPERTIES OUTPUT_NAME "DJMugumyaProSoundEnhancer64")
else()
    set_target_properties(DJMugumyaProSoundEnhancer PROPERTIES OUTPUT_NAME "DJMugumyaProSoundEnhancer")
endif()

target_include_directories(DJMugumyaProSoundEnhancer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    set_property(TARGET DJMugumyaProSoundEnhancer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_options(DJMugumyaProSoundEnhancer PRIVATE /O2 /fp:fast /W3)
    target_sources(DJMugumyaProSoundEnhancer PRIVATE DJMugumyaProSoundEnhancer.rc)
endif()