# Cmake template for generic projects by Dario Facchini
# Current implementation: OculusSDK 0.5 (static linking), OgreSDK 1.9 (dynamic linking)
# - - - - - - - - - - - - - - - - - - - - - - - - - - -
# WINDOWS: to make application find mandatory .dll (not the plugins), just add Ogre bin directories to PATH
# WINDOWS: to run this cmake, please set OGRE_HOME as environment variable to your OgreSDK main folder
# WINDOWS: to run this cmake, please set OCULUS_HOME as environment variable to your OculusSDK main folder
# - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FULLY AND EASILY CUSTOMIZABLE directory names
# "CONFIGURE" and "GENERATE" a custom project in a subdirectory (the "working directory") of cmake current folder
# AUTOFINDS include (.h and .hpp) and source files (.cpp) recursively in the dedicated directories
# BUILD FILES are saved in a dedicated directory (different from project subfolder)
# DEDICATED distribution FOLDER (build, configuration and media files are automatically copied on INSTALL)
# ON "INSTALL" - for Ogre:
#	- release executable is copied (if existing)
#	- all needed release .dll are copied to distribution dir
# 	- all needed release plugins .dll (defined in plugins.cfg) are copied to plugins distribution subdir
#	- all needed resources (defined in resources.cfg, both from Ogre folder and from media folder) are copied to media distribution subdir
#	- all release configuration files are copied in config distribution subdir and updated to reference the copied files
# A PostInstall.cmake script template is used to parse configuration files and copy all contents.

cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)
cmake_policy(SET CMP0048 OLD)			# how to set project version (retrocompatibility with OLD rule)

# useful boolean variable to understand fast if this project is used as subproject or main project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(IS_SUBPROJECT false)
else()
	set(IS_SUBPROJECT true)
endif()

# MACRO to set value of a variable if not already set
# useful when we don't know if this project is used as a subproject or not
# (therefore some variable could have been already set from top-level cmakefile)
macro(setifnotdef var_name var_value)
	if(NOT DEFINED ${var_name})
		if(IS_SUBPROJECT)
			set(${var_name} ${var_value} PARENT_SCOPE)
		else()
			set(${var_name} ${var_value})
		endif()
	endif()
endmacro()


################################################################
#                   PROJECT SETTINGS (CUSTOMIZE THIS)          #
################################################################
# Please refer to the README before modifying!!

# project name
project (OKAR)
set(OgreOculusApp_VERSION_MAJOR 0)
set(OgreOculusApp_VERSION_MINOR 2)
set(OgreOculusApp_VERSION_PATCH 2)

# directories names relative to CMAKE_CURRENT_SOURCE_DIR
set (libs_dir_name	 "libs")			# name of the dir containing (ALL or SOME) subprojects required to this project
set (source_dir_name "src")				# name of the dir containing ALL implementations
set (header_dir_name "include")			# name of the dir containing ALL declarations
set (binary_dir_name "build")			# name of the dir containing build results (Release, Debug, ..), IF built separately
set (exe_name "OKAR-Demo")				# name of executable, IF built separately
set (media_dir_name  "media")			# name of the dir containing SAMPLE/STANDARD media files for this project
set (config_dir_name "cfg")				# name of the dir containing SAMPLE/STANDARD configuration files for this project
set (distribution_dir_name "dist")		# name of the dir that will contain final distribution files on INSTALL
set (cmake_dir_name "cmake")			# name of the dir containing cmake files needed to this project
										# N.B.	cmake will search in the standard cmake directory,
										#		then in this directory,
										#		then in any cmake folder of any project including this project,
										#		then will search into sdk own cmake directories (if they have any)
set (install_script_name "PostInstall")	# name of the TEMPLATE cmake file for generating install script of this project
										# all @VARS@ in this file will be replaced with current specific values
										# at the end of this script with configure_file() and then used at install time.
										# This is done using local variable values in an install script!

################################################################
#                   PROJECT/BUILD SETUP     		           #
################################################################

# cmake project variables
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}							# (this is the standard cmake directory)
	"${CMAKE_CURRENT_SOURCE_DIR}/${cmake_dir_name}" # append: where personal project cmake files are stored
	)

# cmake build variables
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
unset(LIBRARIES_TO_LINK)
unset(HEADERS_TO_INCLUDE_PUBLIC)
unset(HEADERS_TO_INCLUDE_PRIVATE)

# cmake binary/build location
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/${binary_dir_name})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if(IS_SUBPROJECT)
	message(SEND_ERROR "${PROJECT_NAME}: This project was not meant to be used as a subproject or external library")
else()
	set (OUTPUT_FILE_NAME ${exe_name})
endif()

# cmake install/distribution variables
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT DEFINED CMAKE_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/${distribution_dir_name}" CACHE PATH "Default install path" FORCE )
    message(STATUS "${PROJECT_NAME}: CMAKE_INSTALL_PREFIX was not defined. Setting install directory to: '${CMAKE_INSTALL_PREFIX}'")
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT false)
endif()
set(CMAKE_INSTALL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${cmake_dir_name}/${install_script_name}.cmake")	# this is cmake template for generating CMAKE_INSTALL_SCRIPT_SPECIFIC
set(CMAKE_INSTALL_SCRIPT_SPECIFIC "${CMAKE_CURRENT_SOURCE_DIR}/${cmake_dir_name}/${install_script_name}.specific.cmake") # result script with hardcoded variable values

# project configuration files location
set(PROJECT_CONFIG_DIR ${CMAKE_SOURCE_DIR}/${config_dir_name})
set(PROJECT_MEDIA_DIR ${CMAKE_SOURCE_DIR}/${media_dir_name})

################################################################
#                   EXTERNAL DEPENDENCIES                      #
################################################################

# OPENCV lib
find_package(OpenCV REQUIRED)						# OpenCV lib config
list(APPEND LIBRARIES_TO_LINK ${OpenCV_LIBS})		# OpenCV lib link
list(APPEND HEADERS_TO_INCLUDE_PUBLIC				# OpenCV lib include
	"${OpenCV_INCLUDE_DIRS}"
	)


################################################################
#                   ADD MAIN PROJECT FILES 					   #
################################################################

# main directories
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${source_dir_name})
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${header_dir_name})

# add files and group them
file(GLOB_RECURSE source_files "${source_dir_name}/*.cpp")
source_group("${source_dir_name}" FILES ${source_files})
file(GLOB_RECURSE header_files "${header_dir_name}/*.hpp" "${header_dir_name}/*.h")
source_group("${header_dir_name}" FILES ${header_files})

# add header files to included directories
list(APPEND HEADERS_TO_INCLUDE_PUBLIC "${PROJECT_INCLUDE_DIR}")


################################################################
#                   ADD SUBPROJECTS (RECURSIVE)				   #
################################################################

# Use OculusOgre App to extend its functionalities with Cameras and Kinect
add_subdirectory(${libs_dir_name}/OgreOculusApp)	# OculusOgre subproject source
list(APPEND HEADERS_TO_INCLUDE_PRIVATE $<TARGET_PROPERTY:OgreOculusApp,INTERFACE_INCLUDE_DIRECTORIES>)	# OculusOgre subproject includes
list(APPEND LIBRARIES_TO_LINK OgreOculusApp)		# OculusOgre subproject lib link


################################################################
#                   EXE OUTPUT                                 #
################################################################
if(IS_SUBPROJECT)
	add_library(${OUTPUT_FILE_NAME} STATIC
		${source_files}
		${header_files}
	)
else()
	ADD_EXECUTABLE("${OUTPUT_FILE_NAME}" ${source_files} ${header_files})
endif()


################################################################
#                   INCLUDING HEADERS                          #
################################################################

if(DEFINED HEADERS_TO_INCLUDE_PUBLIC)
	target_include_directories(${OUTPUT_FILE_NAME} PUBLIC ${HEADERS_TO_INCLUDE_PUBLIC})		# these headers will be accessible from parent projects and can be
endif()																							# included automatically by including:
																								# $<TARGET_PROPERTY:OgreOculusApp,INTERFACE_INCLUDE_DIRECTORIES>
																								# as PRIVATE or PUBLIC to your target.

if(DEFINED HEADERS_TO_INCLUDE_PRIVATE)
	target_include_directories(${OUTPUT_FILE_NAME} PRIVATE ${HEADERS_TO_INCLUDE_PRIVATE})		# these headers cannot be accessed from parent projects, unless
endif()																							# you include them manually by hardcoding their path.
																								# By the way, these are only the headers that are not meant to be
																								# used by parent project (hidden dependencies)

################################################################
#                   LINKING LIBRARIES                          #
################################################################

# All platforms
if(DEFINED LIBRARIES_TO_LINK)
	target_link_libraries("${OUTPUT_FILE_NAME}"
		${LIBRARIES_TO_LINK}
	)
endif()			

# Win32 specific
if( WIN32 )
	target_link_libraries("${OUTPUT_FILE_NAME}"
		winmm.lib
		ws2_32.lib
	)
	
# Unix specific
elseif( UNIX )

	# Where to find UDEV:
	find_package(UDEV REQUIRED)
	# Where to find XINERAMA:
	find_package(Xinerama REQUIRED)
	find_package(X11 REQUIRED)

	target_link_libraries("${OUTPUT_FILE_NAME}"
		librt.so
		libXrandr.so
		libGL.so
		libXxf86vm.so
		libpthread.so
		${XINERAMA_LIBRARY}
		${X11_LIBRARIES}
	)
endif()



################################################################
#           TARGET PROPRIETIES AND COMPILER OPTIONS            #
################################################################

# set "_d" postfix for debug configurations
set_target_properties(${OUTPUT_FILE_NAME} PROPERTIES DEBUG_POSTFIX _d)

# /MP for multiple thread compiling
# WIN32_LEAN_AND_MEAN to avoid conflicts between boost and Leap motion, which both seem to include winsock.h
if( WIN32 )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DWIN32_LEAN_AND_MEAN")
endif()



################################################################
#                   POST-BUILD AND INSTALL 					   #
################################################################
# Run "INSTALL" to copy exe, dll, media and cfg files into /dist folder

# define "dist" directory where finished project should be installed for final distribution
#if(WIN32 AND NOT MINGW)
#	add_custom_command( TARGET ${OUTPUT_FILE_NAME} PRE_BUILD
#		COMMAND if not exist ..\\${distribution_dir_name} mkdir ..\\${distribution_dir_name} )
	#add_custom_command( TARGET ${OUTPUT_FILE_NAME} POST_BUILD
	#	COMMAND copy \"$(TargetPath)\" ..\\${distribution_dir_name} )
#endif(WIN32 AND NOT MINGW)
 
#if(MINGW OR UNIX)
	#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_PREFIX})
#endif(MINGW OR UNIX)

# 
#install (SCRIPT "${CMAKE_SOURCE_DIR}/cmake/PreInstall.cmake")

# copy release executable
if(NOT IS_SUBPROJECT)
	install(TARGETS ${OUTPUT_FILE_NAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
		CONFIGURATIONS Release RelWithDebInfo
	)
endif()

