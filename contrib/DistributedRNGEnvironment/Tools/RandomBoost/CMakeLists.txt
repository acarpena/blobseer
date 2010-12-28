cmake_minimum_required(VERSION 2.6)

# Edit this to fit your needs
set(EXTERNAL_ROOT /path/to/deployment/root/directory) # usually it is ~/deploy

####################################################################
# YOU DON'T NEED TO MODIFY ANYTHING BELOW THIS LINE

# To reuse the CMake module for Boost (you may not need to change it)
set(CMAKE_MODULE_PATH ${BLOBSEER_ROOT}/cmake/Modules)

# Some extra paramters to the compiler
add_definitions(-O2 -Wall)

# Setting OpenSSL dependency
#set(OPENSSL_ROOT ${EXTERNAL_ROOT})
#find_package(OpenSSL)

# Setting Boost dependency
set(BOOST_ROOT ${EXTERNAL_ROOT})
set(Boost_ADDITIONAL_VERSIONS "1.37" "1.38" "1.39")
find_package(Boost 1.36)

# Set lib and include dirs
include_directories(${Boost_INCLUDE_DIRS})

# Target definition
add_executable(randomBoost randomBoost.cpp)
