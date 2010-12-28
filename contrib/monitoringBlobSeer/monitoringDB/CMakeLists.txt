cmake_minimum_required (VERSION 2.6)
project (USER_ACC_DISTR_BS)

set(PROJECT_ROOT "~/Work/blobseer/contrib/monitoringBlobSeer/UserAccounting/DistributedMonitoring")
set(JAVA_HOME "/usr")

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
	    COMMAND rm -drf jar
	    COMMAND mkdir jar
		)
ADD_CUSTOM_TARGET(CreateDirs 
	    DEPENDS dirs) 




######################################################
##-----Compile the Malicious Clients classes--------##
######################################################
set(USERACC_CLASSPATH "${PROJECT_LIBRARIES}:src/")

# source directories
set (SRC_DISTR_MON_DATA	 "src/data/*.java")
set (SRC_DISTR_MON_NIO_CLIENT	 "src/nio/client/*.java")
set (SRC_DISTR_MON_NIO_SERVER    "src/nio/server/*.java")
set (SRC_DISTR_MON_PROCESS	 "src/processing/*.java")
set (SRC_DISTR_MON_PROCESS_FILTERS	 "src/processing/filters/*.java")
set (SRC_DISTR_MON_TABLES	 "src/processing/tables/*.java")
set (SRC_DISTR_MON_UPDATE	 "src/processing/update/*.java")
set (SRC_DISTR_MON_UTIL	 "src/util/*.java")
set (SRC_DISTR_MON_DB	 "src/util/db/*.java")

ADD_CUSTOM_COMMAND(
	    OUTPUT build_user_acc
	    COMMAND ${JAVA_COMPILE}  
			-classpath ${USERACC_CLASSPATH}
			-d build	
			${SRC_DISTR_MON_UTIL}
			${SRC_DISTR_MON_DB}	
			${SRC_DISTR_MON_DATA}  
			${SRC_DISTR_MON_NIO_SERVER}  
			${SRC_DISTR_MON_NIO_CLIENT} 
			${SRC_DISTR_MON_PROCESS_FILTERS}
			${SRC_DISTR_MON_TABLES}
			${SRC_DISTR_MON_UPDATE}
			${SRC_DISTR_MON_PROCESS}
			
		)
	 
ADD_CUSTOM_TARGET(userAccounting 
	    DEPENDS dirs build_user_acc)



#######################################################
##-----Create the .jar ----##
#######################################################
set (CLASS_USER_ACC	 "feedback/userAccounting")
#set (CLASS_BLACKLIST	 "feedback/userAccounting/blackList")
#set (CLASS_UPDATE_TIMER  "feedback/userAccounting/listUpdateTimer")
#set (CLASS_MAL_CLIENTS	 "feedback/userAccounting/maliciousClients")

ADD_CUSTOM_COMMAND(
	    OUTPUT createUserAccJar
	    COMMAND ${JAVA_ARCHIVE}
			-cf jar/UserAcc.jar  
			-C  build ${CLASS_USER_ACC} 
	#		-C  build ${CLASS_BLACKLIST} 
	#		-C  build ${CLASS_UPDATE_TIMER}  
	#		-C  build ${CLASS_MAL_CLIENTS}
	    DEPENDS build_user_acc 
		)
	 
ADD_CUSTOM_TARGET(CreateUserAccountingLibrary
	    DEPENDS build_user_acc  createUserAccJar)


############################################
##----------The MAKE ALL target-----------##
############################################

ADD_CUSTOM_TARGET(all ALL 
	    DEPENDS dirs build_user_acc)



############################################
##----------Clean the installation--------##
############################################

ADD_CUSTOM_COMMAND(
	    OUTPUT cl
	    COMMAND rm -drf
	    	build
	    COMMAND rm -drf
	    	jar
		)
ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)


	  
