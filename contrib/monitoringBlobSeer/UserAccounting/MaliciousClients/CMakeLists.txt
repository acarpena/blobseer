# The name of this project is "UserAccountingBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${MON_SOURCE_DIR} 
cmake_minimum_required (VERSION 2.6)
project (USER_ACC_BS)


#set the path to the MonALISA repository
set(REPOS_ROOT "~/Work/BlobMonitoring/MLrepository")

#set the path to JAVA_HOME, if it is not set as an environment variable
#set(JAVA_HOME "")



# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#find the path to the JAVA commands
find_package(JAVA)
find_package(Repos)
find_package(BSMonitoring)


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
set(USERACC_CLASSPATH "${REPOS_LIBRARIES}:${POSTGRES_LIBRARIES}:${BSMON_LIBRARIES}:lib/log4j-1.2.15.jar:lib/postgresql-8.4-701.jdbc3.jar:build/")

# source directories
set (SRC_USER_ACC	 "src/feedback/userAccounting/*.java")
set (SRC_BLACKLIST	 "src/feedback/userAccounting/blackList/*.java")
set (SRC_UPDATE_TIMER    "src/feedback/userAccounting/listUpdateTimer/*.java")
set (SRC_MAL_CLIENTS	 "src/feedback/userAccounting/maliciousClients/*.java")
set (SRC_USER_UTIL	"src/util/db/*.java")

ADD_CUSTOM_COMMAND(
	    OUTPUT build_user_acc
	    COMMAND ${JAVA_COMPILE}  
			-classpath ${USERACC_CLASSPATH}
			-d build
			${SRC_USER_UTIL}		
			${SRC_USER_ACC} 
			${SRC_BLACKLIST} 
			${SRC_UPDATE_TIMER}  
			${SRC_MAL_CLIENTS}
		)
	 
ADD_CUSTOM_TARGET(userAccounting 
	    DEPENDS dirs build_user_acc)



#######################################################
##-----Create the .jar Malicious Clients archives----##
#######################################################
set (CLASS_USER_ACC	 "feedback/userAccounting")
#set (CLASS_BLACKLIST	 "feedback/userAccounting/blackList")
#set (CLASS_UPDATE_TIMER  "feedback/userAccounting/listUpdateTimer")
#set (CLASS_MAL_CLIENTS	 "feedback/userAccounting/maliciousClients")

ADD_CUSTOM_COMMAND(
	    OUTPUT createUserAccJar
	    COMMAND ${JAVA_ARCHIVE}
			-cf lib/UserAcc.jar  
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
	    	lib
		)
ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)


	  
