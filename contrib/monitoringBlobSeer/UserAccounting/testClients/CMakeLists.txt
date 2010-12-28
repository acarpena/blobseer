# The name of this project is "MonitoringBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${MON_SOURCE_DIR} 

cmake_minimum_required (VERSION 2.6)
project (BS_CLIENTS)

# set the root prefix of the library dependencies
set(EXTERNAL_ROOT ~/deploy)	

#set the source directory for blobseer    
set(BLOBSEER_SOURCE_DIR ~/Work/blobseer/tags/release-0.3.1) 

#set the source directory for the logging service   
set(LOGGING_SOURCE_DIR ~/Work/blobseer/contrib/logging_service/logging_client_cpp) 


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
set(Boost_ADDITIONAL_VERSIONS "1.39")
find_package(Boost 1.36 REQUIRED COMPONENTS system thread serialization filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# set the libconfig++ library
set(CONFIG_ROOT ${EXTERNAL_ROOT})
find_package(Config)
include_directories(${CONFIG_INCLUDE_DIR})


# set the global link dependencies and include dirs
set(LIB_DEPENDENCIES 
	${CONFIG_LIBRARIES} 
	${Boost_LIBRARIES}  
	${CRYPTO_LIBRARIES}  
)

include_directories(${BS_CLIENTS_SOURCE_DIR})
include_directories(${BLOBSEER_SOURCE_DIR})
include_directories(${LOGGING_SOURCE_DIR})
add_subdirectory (client_tests)
