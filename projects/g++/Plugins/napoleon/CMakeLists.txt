cmake_minimum_required(VERSION 2.8)

project(napoleonPlugin)

add_definitions( -DNAPOLEON_EXPORT )


file(
	GLOB_RECURSE
	source_files
	${PLUGIN_SOURCE_DIR}/napoleon/*.cpp
	${PLUGIN_SOURCE_DIR}/napoleon/*.h
)

add_library(
	napoleon
	SHARED
	${source_files}
)

target_link_libraries (napoleon mengeCore formations )

