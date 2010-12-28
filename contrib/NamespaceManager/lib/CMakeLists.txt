add_library (fbs_namespace-c SHARED 
			${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/clib/NamespaceClientC.cpp
			${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/clib/FileHandlerC.cpp
			)


add_library (fbs_namespace SHARED 
			${BLOBSEER_SOURCE_DIR}/client/range_query.cpp 
			${BLOBSEER_SOURCE_DIR}/client/object_handler.cpp
			${FBLOBSEER_SOURCE_DIR}/common/NamespaceManConstants.hpp 
 			${FBLOBSEER_SOURCE_DIR}/common/FileMetadata.cpp 
			${FBLOBSEER_SOURCE_DIR}/common/ClientRequest.cpp 
			${FBLOBSEER_SOURCE_DIR}/common/Reply.cpp 

			${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/FileHandler.cpp
			${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/NamespaceClient.cpp
		)

# Link the executable to the necessary libraries.
target_link_libraries (fbs_namespace 	
			${LIB_DEPENDENCIES} 
			)

target_link_libraries (fbs_namespace-c 
			${LIB_DEPENDENCIES}
			fbs_namespace)

