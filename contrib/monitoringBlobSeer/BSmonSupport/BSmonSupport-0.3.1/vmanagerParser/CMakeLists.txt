# The name of this project is "MonitoringBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${MON_SOURCE_DIR} 
cmake_minimum_required (VERSION 2.6)
project (MON)


#set ApMon path
set(APMON_ROOT "${MON_SOURCE_DIR}")

# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#find the path to the JAVA commands
find_package(JAVA)
find_package(Apmon)


# Add the directories that belong to the project
add_subdirectory (vmonitor)


#Define a make clean command
ADD_CUSTOM_COMMAND(
	    OUTPUT cl
	    PRE_BUILD
	    COMMAND rm *.class
	    WORKING_DIRECTORY vmonitor
		)

ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)

	  
