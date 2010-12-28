
#add_executable (client_iw 
#                         ${BS_CLIENTS_SOURCE_DIR}/logging_utils/logging_client.cpp
#                         ${BLOBSEER_SOURCE_DIR}/client/range_query.cpp  
#   		 	 ${BS_CLIENTS_SOURCE_DIR}/clients/object_handler.cpp
#                         ${BS_CLIENTS_SOURCE_DIR}/clients/iw_object.cpp 
#                         client_iw.cpp )

#add_executable (client_pnw
#                         ${BS_CLIENTS_SOURCE_DIR}/logging_utils/logging_client.cpp
#                         ${BS_CLIENTS_SOURCE_DIR}/clients/object_handler.cpp
#                         ${BLOBSEER_SOURCE_DIR}/client/range_query.cpp
#                         ${BS_CLIENTS_SOURCE_DIR}/clients/pnw_object.cpp 
#                         client_pnw.cpp )

#add_executable (client_wnp 
#	 		 ${BS_CLIENTS_SOURCE_DIR}/logging_utils/logging_client.cpp
#                         ${BS_CLIENTS_SOURCE_DIR}/clients/object_handler.cpp
#                         ${BLOBSEER_SOURCE_DIR}/client/range_query.cpp
#                         ${BS_CLIENTS_SOURCE_DIR}/clients/wnp_object.cpp 
#                         client_wnp.cpp )

add_executable (malicious_client
	 		 ${LOGGING_SOURCE_DIR}/logging_utils/logging_client.cpp
                         ${BLOBSEER_SOURCE_DIR}/client/range_query.cpp
                         ${BS_CLIENTS_SOURCE_DIR}/clients/object_handler.cpp
                         ${BS_CLIENTS_SOURCE_DIR}/clients/wnp_object.cpp 
			 ${BS_CLIENTS_SOURCE_DIR}/clients/pnw_object.cpp
			 ${BS_CLIENTS_SOURCE_DIR}/clients/iw_object.cpp
                         malicious_client.cpp )

# Link the executable to the necessary libraries.
#target_link_libraries (client_iw ${LIB_DEPENDENCIES})
#target_link_libraries (client_pnw ${LIB_DEPENDENCIES})
#target_link_libraries (client_wnp ${LIB_DEPENDENCIES})
target_link_libraries (malicious_client ${LIB_DEPENDENCIES})

