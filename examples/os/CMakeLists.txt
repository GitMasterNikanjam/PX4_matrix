add_subdirectory(shared)

# OS-specific examples stay out of builds for other operating systems.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/linux/CMakeLists.txt")
	add_subdirectory(linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/windows/CMakeLists.txt")
	add_subdirectory(windows)
endif()
