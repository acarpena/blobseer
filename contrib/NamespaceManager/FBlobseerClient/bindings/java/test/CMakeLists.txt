# source directories
set (SRC_TEST	    	 "*.java")
set (TEST_CLASSPATH	 ".:${FBLOBSEER_ROOT}/FBlobseerClient/bindings/java")

# build the java classes
ADD_CUSTOM_COMMAND(
	    OUTPUT javaBindingsTest
	    COMMAND ${JAVA_COMPILE}  
		-classpath ${TEST_CLASSPATH}
			*.java
		)

ADD_CUSTOM_TARGET(testJavaBindings ALL
	    DEPENDS javaBindingsTest)



################################################
##--------------The clean target--------------##
################################################ 
SET(cleanFiles Test.class)
SET_DIRECTORY_PROPERTIES(PROPERTIES  
		ADDITIONAL_MAKE_CLEAN_FILES "${cleanFiles}")


