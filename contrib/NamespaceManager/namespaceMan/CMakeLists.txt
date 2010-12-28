add_executable (NamespaceManager
            			 ${FBLOBSEER_SOURCE_DIR}/common/FileMetadata.cpp             
 ${FBLOBSEER_SOURCE_DIR}/common/Reply.cpp 
	 ${FBLOBSEER_SOURCE_DIR}/common/ClientRequest.cpp  
 
		storage/NamespaceSimpleStorage.cpp
			 ${FBLOBSEER_SOURCE_DIR}/namespaceMan/NamespaceManager.cpp
			

			)

# Link the executable to the necessary libraries.
target_link_libraries (NamespaceManager ${LIB_DEPENDENCIES})
