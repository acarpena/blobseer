add_executable (provider provider_mon_listener.cpp 
                         ${BLOBSEER_SOURCE_DIR}/provider/page_manager.hpp 
                         ${BLOBSEER_SOURCE_DIR}/provider/pmgr_listener.cpp 
			 ${BLOBSEER_SOURCE_DIR}/provider/bdb_bw_map.cpp
			 ${BLOBSEER_SOURCE_DIR}/provider/null_bw_map.cpp
                         provider.cpp)

add_executable (sdht	metadata_mon_listener.cpp 
			${BLOBSEER_SOURCE_DIR}/provider/bdb_bw_map.cpp
			${BLOBSEER_SOURCE_DIR}/provider/null_bw_map.cpp	
			sdht.cpp)

# Link the executable to the necessary libraries.
target_link_libraries (provider ${LIB_DEPENDENCIES})
target_link_libraries (sdht ${LIB_DEPENDENCIES} ${BDB_LIBRARIES})

