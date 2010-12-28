# The name of this project is "FBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${FBLOBSEER_SOURCE_DIR} 

cmake_minimum_required (VERSION 2.6)
project (FBLOBSEER)

# set the root prefix of the library dependencies
set(EXTERNAL_ROOT $ENV{HOME}/deploy)	

#set the source directory for blobseer    
set(BLOBSEER_SOURCE_DIR $ENV{HOME}/Work/blobseer/tags/release-0.3.4) 

#set the path to JAVA_HOME, if it is not set as an environment variable
#set(JAVA_HOME "")


# some global compiler flags
add_definitions(-g -D__ASSERT -D__DEBUG -D__INFO -D__BENCHMARK -DSOCK_TYPE=tcp)

# Set up the needed libraries
############################################################
# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# set up boost
set(BOOST_ROOT ${EXTERNAL_ROOT})
find_package(Boost)
include_directories(${BOOST_INCLUDE_DIR})

# set up the libconfig++ library
set(CONFIG_ROOT ${EXTERNAL_ROOT})
find_package(Config)
include_directories(${CONFIG_INCLUDE_DIR})

# set up the JNI headers
find_package(JNI)
include_directories(${JNI_INCLUDE_DIR})

#find the path to the JAVA commands
find_package(JAVA)


#find the path to the Python libraries
find_package(Python)

# set up the FBlobser library
set(FBLOBSEER_ROOT ${FBLOBSEER_SOURCE_DIR})
find_package(FBlobseer)
#############################################################


# set the global link dependencies and include dirs
set(LIB_DEPENDENCIES 
	${CONFIG_LIBRARIES} 
	${BOOST_LIBRARIES}  
)

include_directories(${FBLOBSEER_SOURCE_DIR})
include_directories(${BLOBSEER_SOURCE_DIR})

# Recurse through the directories
add_subdirectory (namespaceMan)
add_subdirectory (lib)
add_subdirectory (test)

# uncomment the next line if you want to build the Python version of the FBlobseer client
add_subdirectory (FBlobseerClient/bindings/python)

# uncomment the next line if you want to build the JAVA version of the FBlobseer client
#add_subdirectory (FBlobseerClient/bindings/java)


