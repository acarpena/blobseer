add_executable (server bsServer.cpp)
add_executable (proxy proxy.cpp)

####
target_link_libraries (server ${LIB_DEPENDENCIES})
target_link_libraries (proxy ${LIB_DEPENDENCIES})