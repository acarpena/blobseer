cmake_minimum_required(VERSION 2.6)

# Adding libs and includes to the environment
include_directories(${PYTHON_INCLUDE_PATH})

# Target
add_library(fblobseer SHARED 
		      FileHandlerWrapper.cpp
		      NamespaceClientWrapper.cpp
		      fblobseer_py.cpp)
add_dependencies(fblobseer fbs_namespace)

target_link_libraries(fblobseer  ${Boost_PYTHON_LIBRARY} fbs_namespace )
set_target_properties(fblobseer PROPERTIES PREFIX "")
