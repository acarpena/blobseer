# The name of this project is "MonitoringBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${MON_SOURCE_DIR} 

cmake_minimum_required (VERSION 2.6)
project (MON)

# set the root prefix of the library dependencies
set(EXTERNAL_ROOT $ENV{HOME}/deploy)	

#set the source directory for blobseer    
set(BLOBSEER_SOURCE_DIR $ENV{HOME}/Work/blobseer/tags/release-0.3.3) 


#set the root prefix for the ApMon libraries 
set(APMON_ROOT ${EXTERNAL_ROOT})
#set the path for the ApMon JAR
set(APMON_JAVA_ROOT ${EXTERNAL_ROOT})






# some global compiler flags
add_definitions(-O2 -D__ASSERT -D__INFO -D__BENCHMARK -DSOCK_TYPE=tcp)


# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# find the OpenSSL library
set(OPENSSL_ROOT ${EXTERNAL_ROOT})
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

# the elegant way to set up boost
set(BOOST_ROOT ${EXTERNAL_ROOT})
find_package(Boost)
include_directories(${BOOST_INCLUDE_DIR})

# set the libconfig++ library
set(CONFIG_ROOT ${EXTERNAL_ROOT})
find_package(Config)
include_directories(${CONFIG_INCLUDE_DIR})

# set up Berkley DB
set(BDB_ROOT ${EXTERNAL_ROOT})
find_package(BerkeleyDB)
include_directories(${BDB_INCLUDE_DIR})


#set the ApMon library
set(APMON_ROOT ${APMON_ROOT})
find_package(Apmon)
include_directories(${APMON_INCLUDE_DIR})



# set the global link dependencies and include dirs
set(LIB_DEPENDENCIES 
	${CONFIG_LIBRARIES} 
	${BOOST_LIBRARIES}  
	${APMON_LIBRARIES} 
	${CRYPTO_LIBRARIES}  
	${BDB_LIBRARIES}
)

include_directories(${MON_SOURCE_DIR})
include_directories(${BLOBSEER_SOURCE_DIR})

# Recurse through the directories
add_subdirectory (provider)
add_subdirectory (pmanager)
add_subdirectory (vmanager)
