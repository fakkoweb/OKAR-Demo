cmake_minimum_required(VERSION 2.6)
set(cmake_test_VERSION_MAJOR 0)
set(cmake_test_VERSION_MINOR 1)
set(cmake_test_VERSION_PATCH 0)

################################################################
#                   PROJECT SETTINGS (CUSTOMIZE THIS)          #
################################################################
project (OgreTest)

# all directories relative to CMAKE_SOURCE_DIR
set (source_dir_name "source")		# name of the dir containing ALL implementations
set (header_dir_name "include")		# name of the dir containing ALL declarations
set (binary_dir_name "build")		# name of the dir that will contain build results (Release, Debug, ..) and .dll
set (exe_name "OgreTest")			# mame of executable
set (distribution_dir_name "dist")	# name of the dir that contains final distribution files, such as:
									#	- data files (managed by you)
									#	- final release executable files (automatically copied on INSTALL)
set (cmake_dir_name "cmake")		# name of the dir containing cmake files for this project
									# N.B.	cmake will search in the standard cmake directory, then in this directory,
									#		then will search into libraries own cmake directories (if they have any)


################################################################
#                   PROJECT/BUILD SETUP     		           #
################################################################
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}						# (this is the standard cmake directory)
	"${CMAKE_SOURCE_DIR}/${cmake_dir_name}/" 	# append: where personal project cmake files are stored
	)
include_directories("${PROJECT_BINARY_DIR}")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/${binary_dir_name})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")


################################################################
#                   OGRE SETUP	                               #
################################################################

if(WIN32)

	# check if OGRE_HOME is defined
	if (NOT DEFINED ENV{OGRE_HOME} AND NOT DEFINED OGRE_HOME)
		message(SEND_ERROR "Must define OGRE_HOME (as Environment variable or cmake variable)")
	# if at least one of them is defined, use it	
	else()
		if (NOT DEFINED OGRE_HOME)
			message("Environment variable OGRE_HOME found. This will be used as OculusSDK folder.")
			set(OGRE_HOME $ENV{OGRE_HOME})
		endif()
		if (NOT DEFINED OGRE_BUILD OR "${OGRE_BUILD}" STREQUAL "")
			set(OGRE_BUILD ${OGRE_HOME})
		endif()
		if( NOT DEFINED OGRE_SOURCE_DIR OR "${OGRE_SOURCE_DIR}" STREQUAL "")
			set(OGRE_SOURCE_DIR ${OGRE_HOME})
		endif()

	endif()

	# set cmake directory for OGRE
	set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")	# append: OGRE personal cmake directory
	set(OGRE_SAMPLES_INCLUDEPATH "${OGRE_HOME}/Samples/Common/include")		# useless???

endif(WIN32)

if(UNIX)

	# look for cmake directory for OGRE (different options assumed here)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
 
	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local
 
	elseif(EXISTS "/usr/lib/OGRE/cmake")
 
	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one
 
	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE")

endif(UNIX)

################################################################
#                   OCULUSSDK SETUP	                           #
################################################################

if(WIN32)
	# check if OCULUS_HOME is defined
	if (NOT DEFINED ENV{OCULUS_HOME} AND NOT DEFINED OCULUS_HOME)
		if( NOT EXISTS "${CMAKE_SOURCE_DIR}/OculusSDK")
			message(SEND_ERROR "Must define OCULUS_HOME as Environment variable or cmake variable, or put the OculusSDK folder into the Project's root directory. Make sure the name of the folder is 'OculusSDK'")
		else()
			set(OCULUS_SDK_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OculusSDK")
		endif()
	# if at least one of them is defined, use it	
	else()
		if (NOT DEFINED OCULUS_HOME)
			message("Environment variable OCULUS_HOME found. This will be used as OculusSDK folder.")
			set(OCULUS_HOME $ENV{OCULUS_HOME})
		endif()
		set(OCULUS_SDK_ROOT_DIR ${OCULUS_HOME})
	endif()

	# set cmake directory for OCULUS, if any in your installation
	set(CMAKE_MODULE_PATH "${OCULUS_HOME}/CMake/;${CMAKE_MODULE_PATH}")	# append: OCULUSSDK personal cmake directory
endif()


if(UNIX)	
	# sdk not available on unix (only BETA)
endif(UNIX)
 









################################################################
#                   MAIN PROJECT SOURCE DIR                    #
################################################################
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${source_dir_name})
include_directories("${PROJECT_SOURCE_DIR}")

################################################################
#                   MAIN PROJECT INCLUDE DIR 				   #
################################################################
set (PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${header_dir_name})
include_directories("${PROJECT_INCLUDE_DIR}")

################################################################
#                   ADD MAIN PROJECT FILES 					   #
################################################################
file(GLOB_RECURSE source_files "${source_dir_name}/*.cpp")
source_group("${source_dir_name}" FILES ${source_files})
file(GLOB_RECURSE header_files "${header_dir_name}/*.hpp" "${header_dir_name}/*.h")
source_group("${header_dir_name}" FILES ${header_files})

################################################################
#                   ADD SUBPROJECTS (RECURSIVE)				   #
################################################################








################################################################
#                   EXTERNAL DEPENDENCIES                      #
################################################################
#find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(Qt4 REQUIRED)
#find_package(Eigen REQUIRED)
#find_package(CUDA REQUIRED)

# OIS + OGRE lib
find_package(OIS REQUIRED)							# OIS cmake config
find_package(OGRE REQUIRED)							# OGRE cmake config
include_directories(								# OGRE + OIS include
	"${OIS_INCLUDE_DIRS}"
	"${OGRE_INCLUDE_DIRS}"
	"${OGRE_Overlay_INCLUDE_DIRS}"
	"${OGRE_SAMPLES_INCLUDEPATH}"
	)

# OCULUSSDK lib
find_package(OculusSDK REQUIRED)
include_directories(								# OpenCV include
	"${OCULUS_SDK_INCLUDE_DIRS}"
	)

# OPENCV lib
find_package(OpenCV REQUIRED)						# OpenCV cmake config
include_directories(								# OpenCV include
	"${OpenCV_INCLUDE_DIRS}"
	)

################################################################
#                   EXE OUTPUT                                 #
################################################################
ADD_EXECUTABLE("${exe_name}" ${source_files} ${header_files})

################################################################
#                   LINKING LIBRARIES                          #
################################################################
target_link_libraries("${exe_name}"
	${OpenCV_LIBS}									# OpenCV lib
	${OIS_LIBRARIES}								# OIS lib
	${OGRE_LIBRARIES}								# OGRE libs
	${OGRE_Overlay_LIBRARIES}
	${OCULUS_SDK_LIBRARIES}							# OCULUSSDK lib
	)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)












################################################################
#                   POST-BUILD ("INSTALL")		               #
################################################################

# define "dist" directory where finished project should be installed for final distribution
if(WIN32 AND NOT MINGW)
	add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD
		COMMAND if not exist ..\\${distribution_dir_name} mkdir ..\\${distribution_dir_name} )
	add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\${distribution_dir_name} )
endif(WIN32 AND NOT MINGW)
 
if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${distribution_dir_name})
endif(MINGW OR UNIX)
 
if(WIN32)
 
	install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
		CONFIGURATIONS All)
 
 	if(EXISTS "${CMAKE_SOURCE_DIR}/dist/media" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/dist/media")
		install(DIRECTORY ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/media
			DESTINATION ./
			CONFIGURATIONS Release RelWithDebInfo Debug
		)
	endif()
 
  	if(EXISTS "${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg" AND EXISTS "${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg")
		install(FILES ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/plugins.cfg
			${CMAKE_SOURCE_DIR}/${distribution_dir_name}/resources.cfg
			DESTINATION ${CMAKE_BINARY_DIR}
			CONFIGURATIONS Release RelWithDebInfo
		)
	endif()
 
 	if(EXISTS "${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg" AND EXISTS "${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg")
		install(FILES ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/plugins_d.cfg
			${CMAKE_SOURCE_DIR}/${distribution_dir_name}/resources_d.cfg
			DESTINATION ${CMAKE_BINARY_DIR}
			CONFIGURATIONS Debug
		)
 	endif()

 	# Run "INSTALL" to copy dll into bin folder (application using dll don't work otherwise!)
        # NOTE: from the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
        # so you'll have to change that to make it work with 1.7.1
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D11.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		${OGRE_PLUGIN_DIR_REL}/OgreOverlay.dll
		DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
		CONFIGURATIONS Release RelWithDebInfo
	)
 	
 	# Run "INSTALL" to copy dll into bin folder (application using dll don't work otherwise!)	
 		# NOTE: from the 1.7.1 sdk the OIS debug dll is called OIS_d.dll instead of libOIS_d.dll
        # so you'll have to change that to make it work with 1.7.1
	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D11_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OgreOverlay_d.dll
		DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
		CONFIGURATIONS Debug
	)
 
   # as of sdk 1.7.2 we need to copy the boost dll's as well
   # because they're not linked statically (it worked with 1.7.1 though)
   install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE}
      ${Boost_THREAD_LIBRARY_RELEASE}
      DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
      CONFIGURATIONS Release RelWithDebInfo
   )
 
   install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG}
      ${Boost_THREAD_LIBRARY_DEBUG}
      DESTINATION ${CMAKE_SOURCE_DIR}/${distribution_dir_name}
      CONFIGURATIONS Debug
   )
endif(WIN32)
 
if(UNIX)
 
	install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION ${EXECUTABLE_OUTPUT_PATH}
		CONFIGURATIONS All)
 
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/media
		DESTINATION ${EXECUTABLE_OUTPUT_PATH}
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	install(FILES ${CMAKE_SOURCE_DIR}/${distribution_dir_name}/plugins.cfg
		${CMAKE_SOURCE_DIR}/${distribution_dir_name}/resources.cfg
		DESTINATION ${EXECUTABLE_OUTPUT_PATH}
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
endif(UNIX)





