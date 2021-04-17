cmake_minimum_required (VERSION 3.13.0)

#helper functions
#----------------------------------------------------------------------------------
function(fncSourceGroup)  
    # workaround. couldn't get source_group to work as expected
	# with the files of the subdirectory
	cmake_parse_arguments(
        PARSED_ARGS # prefix
        "" 			# boolean
        "DIR" 		# mono-valued arguments
        "SOURCES" 	# multi-valued arguments
        ${ARGN} 
    )
	
	foreach(SRC_FILE ${PARSED_ARGS_SOURCES})		
		string(CONCAT SRC_FILE_PATH ${PARSED_ARGS_DIR} "/" ${SRC_FILE})	
		source_group(${PARSED_ARGS_DIR} FILES ${SRC_FILE_PATH})
	endforeach()	
endfunction()

function(fncAddSubDir SUBDIR)
	add_subdirectory(${SUBDIR})
	#each subdirectory adds its source files to global scope variable SOURCES
	#fncSourceGroup(SOURCES "${SOURCES}" DIR ${SUBDIR})
endfunction()

function(fncAddFiles)
	cmake_parse_arguments(
        PARSED_ARGS # prefix
        "" 			# boolean
        "" 		# mono-valued arguments
        "SOURCES" 	# multi-valued arguments
        ${ARGN} 
    )	

	#message("${PARSED_ARGS_SOURCES}")
	foreach(FILE ${PARSED_ARGS_SOURCES}) 
		string(CONCAT FILE2 "${CMAKE_CURRENT_SOURCE_DIR}" "/" "${FILE}")    		
		target_sources(checkIncludes PUBLIC "${FILE2}")
		#source_group("${CMAKE_CURRENT_SOURCE_DIR}" FILES "${FILE2}")
		#string(REPLACE "/" "\\" FILE2 "${FILE2}")
		#string(REPLACE "/" "\\" SOURCE_DIR "${CMAKE_SOURCE_DIR}")
		#set (DIR "compileFile")
		#string(CONCAT SRC_FILE_PATH ${DIR} "/" ${FILE})	
		#source_group("${DIR}" FILES "${FILE2}")
		#source_group("${DIR}" FILES "${SRC_FILE_PATH}")
		#source_group(TREE "${CMAKE_SOURCE_DIR}" FILES "${FILE2}")
		#source_group(TREE "${SOURCE_DIR}" PREFIX "compileFile" FILES "${FILE}")
		#message("${FILE2}")
		#message("${SOURCE_DIR}")	
		#message("${SOURCES}")
	endforeach()		
endfunction()
#----------------------------------------------------------------------------------
#end helper functions

#settings
set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#main project
project (checkIncludes CXX)

#settings use c++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#executable
add_executable(checkIncludes "")

#sub directories
fncAddSubDir(compileFile)
fncAddSubDir(compiler)
fncAddSubDir(make)
fncAddSubDir(main)
fncAddSubDir(parser)
fncAddSubDir(projectFile)
fncAddSubDir(system)
fncAddSubDir(tools)

if(WIN32)
	fncAddSubDir(msvc)
	fncAddSubDir(xml)
endif()

#add source dir to include path
target_include_directories(checkIncludes PUBLIC "${PROJECT_SOURCE_DIR}/inc")

# warnings
if(MSVC)
	target_compile_options(checkIncludes PRIVATE /W4)
else()
	target_compile_options(checkIncludes PRIVATE -Wall -Wextra -pedantic)
endif()

# add threads lib for non msvc build systems
if(NOT MSVC)
	set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
	set(THREADS_PREFER_PTHREAD_FLAG TRUE)
	find_package(Threads REQUIRED)
	target_link_libraries(checkIncludes Threads::Threads)
endif()

# gcc8 creates segfault with filesystem::path without this library
# says it should be fixed with gcc9
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")	
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)		
		target_link_libraries(checkIncludes stdc++fs)
	endif()
endif()

