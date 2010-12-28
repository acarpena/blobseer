
# Include the JNI headers
include_directories(${JNI_INCLUDE_DIRS})

################################################
##-----Compile the JAVA client classes--------##
################################################
# source directories
set (SRC_JAVACLIENT    	 "fblobseer/*.java")

ADD_CUSTOM_COMMAND(
	    OUTPUT javaNamespaceClient
	    COMMAND ${JAVA_COMPILE}  
			${SRC_JAVACLIENT}
		)
ADD_CUSTOM_TARGET(javaClasses ALL
	    DEPENDS javaNamespaceClient)


################################################
##----------create the JNI headers------------##
################################################

ADD_CUSTOM_COMMAND(
	    OUTPUT JNIheaders
	    COMMAND ${JAVA_HEADERS}  
		-o FileHandlerJNI.h 
		-classpath ${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/bindings/java
		fblobseer.FileHandler
	    COMMAND ${JAVA_HEADERS}  
		-o NamespaceClientJNI.h 
		-classpath ${FBLOBSEER_SOURCE_DIR}/FBlobseerClient/bindings/java
		fblobseer.NamespaceClient
		)	 
ADD_CUSTOM_TARGET(headersJNI ALL 
	    DEPENDS javaNamespaceClient JNIheaders)


################################################
##----------Build the native library----------##
################################################ 
add_library (fbs_namespace-java SHARED  FileHandlerJNI.cpp NamespaceClientJNI.cpp)
add_dependencies(fbs_namespace-java headersJNI)
# Link the executable to the necessary libraries.
target_link_libraries (fbs_namespace-java fbs_namespace)

################################################
##--------------The clean target--------------##
################################################ 
SET(cleanFiles FileHandlerJNI.h 
		NamespaceClientJNI.h 
		fblobseer/ResultValues.class
		fblobseer/FileHandler.class
		fblobseer/NamespaceClient.class
		fblobseer/FileMetadata.class
		fblobseer/NamespaceClient$BlobFileHandler.class)
SET_DIRECTORY_PROPERTIES(PROPERTIES  
		ADDITIONAL_MAKE_CLEAN_FILES "${cleanFiles}")


#######################################################
##-----Create the .jar ----##
#######################################################
# source directories
set (CLASS_JAVACLIENT    	 "fblobseer")

ADD_CUSTOM_COMMAND(
	    OUTPUT createNamespaceJar
	    COMMAND ${JAVA_ARCHIVE}
			-cf jar/NamespaceClient.jar  
			${CLASS_JAVACLIENT} 
	    DEPENDS javaNamespaceClient 
		)
	 
ADD_CUSTOM_TARGET(CreateNamespaceClientLibrary ALL
	    DEPENDS createNamespaceJar)




# the test program
add_subdirectory (test)


