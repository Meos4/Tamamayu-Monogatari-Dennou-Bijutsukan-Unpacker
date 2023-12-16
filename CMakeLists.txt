cmake_minimum_required(VERSION 3.8)

# Project
project(tamamayu_monogatari_dennou_bijutsukan_unpacker)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fmt
add_subdirectory(${PROJECT_SOURCE_DIR}/dep/fmt)

# Bin / Lib
set(SOURCES_DIR ${PROJECT_SOURCE_DIR}/src)
set(SOURCES_NO_MAIN
	${SOURCES_DIR}/TMDBExe.cpp
	${SOURCES_DIR}/TMDBExe.hpp
	${SOURCES_DIR}/TMDBTools.cpp
	${SOURCES_DIR}/TMDBTools.hpp
	${SOURCES_DIR}/Types.hpp)

if(TMDBU_LIB)
	add_library(tamamayu_monogatari_dennou_bijutsukan_unpacker)
else()
	add_executable(tamamayu_monogatari_dennou_bijutsukan_unpacker ${SOURCES_DIR}/Main.cpp)
endif()

target_sources(tamamayu_monogatari_dennou_bijutsukan_unpacker PRIVATE ${SOURCES_NO_MAIN})
target_link_libraries(tamamayu_monogatari_dennou_bijutsukan_unpacker PRIVATE fmt::fmt)
target_include_directories(tamamayu_monogatari_dennou_bijutsukan_unpacker PRIVATE ${PROJECT_SOURCE_DIR}/dep/fmt/include)