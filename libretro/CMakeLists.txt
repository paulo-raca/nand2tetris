include_directories(.)
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/link.T")

add_library(hack_libretro SHARED
        hack_libretro.cpp)

set_target_properties(hack_libretro PROPERTIES PREFIX "")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT}")

set_source_files_properties(libretro/libretro.cpp
        PROPERTIES OBJECT_DEPENDS ${LINKER_SCRIPT})
