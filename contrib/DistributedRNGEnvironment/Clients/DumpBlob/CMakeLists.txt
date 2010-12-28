cmake_minimum_required(VERSION 2.6)

# Edit this to fit your needs
set(BLOBSEER_ROOT /path/to/blobseer/source)
set(EXTERNAL_ROOT /path/to/deployment/root/directory) # usually it is ~/deploy

####################################################################
# YOU DON'T NEED TO MODIFY ANYTHING BELOW THIS LINE

# To reuse the CMake module for Boost (you may not need to change it)
set(CMAKE_MODULE_PATH ${BLOBSEER_ROOT}/cmake/Modules)

# Some extra paramters to the compiler
add_definitions(-Wall -DSOCK_TYPE=tcp -g)

# Setting OpenSSL dependency
set(OPENSSL_ROOT ${EXTERNAL_ROOT})
find_package(OpenSSL)

# Setting Boost dependency
set(BOOST_ROOT ${EXTERNAL_ROOT})
set(Boost_ADDITIONAL_VERSIONS "1.37" "1.38" "1.39")
find_package(Boost 1.39 REQUIRED COMPONENTS system date_time filesystem)

# Set lib and include dirs
set(TARGET_LIBS ${Boost_SYSTEM_LIBRARY} blobseer ${CRYPTO_LIBRARIES}
	${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
link_directories(${BLOBSEER_ROOT}/lib)
include_directories(${BLOBSEER_ROOT} ${OPENSSL_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS})

# Target definition
add_executable(dump_blob dump_blob.cpp)
target_link_libraries(dump_blob ${TARGET_LIBS})
