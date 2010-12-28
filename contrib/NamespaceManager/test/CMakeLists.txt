

# test for the fblobseer libraries
add_executable (testFBlobseer testFBlobseer.cpp)

add_executable (testFBlobseerC test_clib/testFBlobseer.c)

# Link the executable to the necessary libraries.
target_link_libraries (testFBlobseer 
		${LIB_DEPENDENCIES}
		${FBLOBSEER_LIBRARIES}
			)

target_link_libraries (testFBlobseerC 
			fbs_namespace-c
			)
