cmake_minimum_required(VERSION 2.6)

# Edit this to fit your needs
set(BLOBSEER_ROOT ~/Softwares/blobseer-0.3)
set(EXTERNAL_ROOT ~/deploy)

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
find_package(Boost 1.39 REQUIRED COMPONENTS system)

# Set lib and include dirs
set(TARGET_LIBS ${Boost_SYSTEM_LIBRARY} blobseer crypto)
link_directories(${BLOBSEER_ROOT}/lib ${EXTERNAL_ROOT}/lib)
include_directories(${BLOBSEER_ROOT} ${OPENSSL_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS})

# Target definition
add_executable(egdBS egdBS.cpp)
target_link_libraries(egdBS ${TARGET_LIBS})
