cmake_minimum_required (VERSION 2.6)
project (BS_PROXY)
# set the root prefix of the library dependencies
set(EXTERNAL_ROOT ~/deploy/depend)
#set the source directory for blobseer    
set(BLOBSEER_SOURCE_DIR ~/deploy/release-0.3) # set your own !!!!
add_definitions(-O2 -Wall -D__ASSERT -D__INFO -D__BENCHMARK -DSOCK_TYPE=tcp)
# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# Now set up the libraries
# -----------------------------------------------------------------------------------

# find the OpenSSL library
#set(OPENSSL_ROOT ${EXTERNAL_ROOT})
#find_package(OpenSSL)
#include_directories(${OPENSSL_INCLUDE_DIR})

# the elegant way to set up boost
set(BOOST_ROOT ${EXTERNAL_ROOT})
set(Boost_ADDITIONAL_VERSIONS "1.37" "1.38" "1.39")
find_package(Boost 1.36 REQUIRED COMPONENTS system thread serialization filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# set up the libconfig++ library
set(CONFIG_ROOT ${EXTERNAL_ROOT})
find_package(Config)
include_directories(${CONFIG_INCLUDE_DIR})

# set up Berkley DB
#set(BDB_ROOT ${EXTERNAL_ROOT})
#find_package(BerkeleyDB)
#include_directories(${BDB_INCLUDE_DIR})


# -----------------------------------------------------------------------------------

# set the global link dependencies and include dirs
set(LIB_DEPENDENCIES ${CONFIG_LIBRARIES} ${Boost_LIBRARIES})
include_directories(${BLOBSEER_SOURCE_DIR})
# Recurse through the directories
add_subdirectory (proxy)