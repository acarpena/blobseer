# The name of this project is "BS_SECURITY_POLICIES". CMakeLists files in this project can
# refer to the root source directory of the project as ${BS_SECURITY_POLICIES_SOURCE_DIR}
cmake_minimum_required (VERSION 2.6)
project (BS_SECURITY_POLICIES)

set(JAVA_HOME "/usr")
set(LIBS_PATH "~/Work/utils/xerces-2_9_1")

# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#find the path to the JAVA commands
find_package(JAVA)
find_package(Libs)

###########################################
##-----Create needed directories---------##
###########################################
ADD_CUSTOM_COMMAND(
	    OUTPUT dirs
	    COMMAND rm -drf build
	    COMMAND mkdir build
		)
ADD_CUSTOM_TARGET(CreateDirs 
	    DEPENDS dirs) 




######################################################
##-----Compile the Malicious Clients classes--------##
######################################################
set(MATCHING_CLASSPATH "${PROJECT_LIBRARIES}:src/")

# source directories
set (SRC_UTIL_ALG	 "src/util/alg/*.java")
set (SRC_UTIL_CONF	 "src/util/conf/*.java")
set (SRC_UTIL_DB	 "src/util/db/*.java")
set (SRC_UTIL_LIST	 "src/util/list/*.java")
set (SRC_UTIL_POLICY	 "src/util/policy/*.java")
set (SRC_UTIL_XML	 "src/util/xml/*.java")
set (SRC_INTERF_LIST	 "src/interf/list/*.java")
set (SRC_INTERF_POLICY	 "src/interf/policy/*.java")
set (SRC_INTERF_XML	 "src/interf/xml/*.java")
set (SRC_IMPLEM 	"src/implem/*.java")


ADD_CUSTOM_COMMAND(
	    OUTPUT matching_module
	    COMMAND ${JAVA_COMPILE}  
			-classpath ${MATCHING_CLASSPATH}
			-d build	
			${SRC_INTERF_LIST}
			${SRC_INTERF_POLICY}
			${SRC_INTERF_XML}
			${SRC_UTIL_ALG}
			${SRC_UTIL_CONF}	
			${SRC_UTIL_DB}  
			${SRC_UTIL_LIST}  
			${SRC_UTIL_POLICY} 
			${SRC_UTIL_XML}
			${SRC_IMPLEM}
			
		)
	 
ADD_CUSTOM_TARGET(policyMatching 
	    DEPENDS matching_module)



############################################
##----------The MAKE ALL target-----------##
############################################

ADD_CUSTOM_TARGET(all ALL 
	    DEPENDS dirs matching_module)



############################################
##----------Clean the installation--------##
############################################

ADD_CUSTOM_COMMAND(
	    OUTPUT cl
	    COMMAND rm -drf
	    	build
		)
ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)


	  
