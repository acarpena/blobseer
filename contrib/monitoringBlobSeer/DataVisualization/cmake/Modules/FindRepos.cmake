
# - Try to find the libraries in the MonALISA repository
# Once done this will define
#
#  REPOS_LIB_FOUND - the system found the libraries
#  REPOS_LIBRARIES - the path to the libraries

SET(REPOS_FIND_REQUIRED "TRUE")
SET(REPOSITORY_FOUND TRUE)


if (NOT REPOS_LIBRARIES)	#not in cache
    FIND_FILE(REPOS_LIBRARIES 
      NAMES 
	MSRC_WEB.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT REPOS_LIBRARIES)
     	set(REPOSITORY_FOUND FALSE)
     	set (LIBRARIES_NOT_FOUND MSRC_WEB.jar)
    endif (NOT REPOS_LIBRARIES)
endif (NOT REPOS_LIBRARIES)

  
if (NOT FILTER_LIBRARIES)	#not in cache
    FIND_FILE(FILTER_LIBRARIES 
      NAMES 
	JStoreClient.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT FILTER_LIBRARIES)
	 set(REPOSITORY_FOUND FALSE)
 	 set (LIBRARIES_NOT_FOUND JStoreClient.jar)
    endif (NOT FILTER_LIBRARIES)
endif (NOT FILTER_LIBRARIES)



if (NOT POSTGRES_LIBRARIES)	#not in cache
    FIND_FILE(POSTGRES_LIBRARIES 
      NAMES 
	postgresql.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT POSTGRES_LIBRARIES)
    	set(REPOSITORY_FOUND FALSE)
    	set (LIBRARIES_NOT_FOUND postgresql.jar)
    endif (NOT POSTGRES_LIBRARIES)
endif (NOT POSTGRES_LIBRARIES)



if (NOT JFREECHART_LIBRARIES)	#not in cache
    FIND_FILE(JFREECHART_LIBRARIES 
      NAMES 
	jfreechart-1.0.5.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT JFREECHART_LIBRARIES)     
	set(REPOSITORY_FOUND FALSE)
     	set (LIBRARIES_NOT_FOUND jfreechart-1.0.5.jar)
    endif (NOT JFREECHART_LIBRARIES)
endif (NOT JFREECHART_LIBRARIES)


if (NOT JCOMMON_LIBRARIES)	#not in cache
    FIND_FILE(JCOMMON_LIBRARIES 
      NAMES 
	jcommon-1.0.9.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )     

   if (NOT JCOMMON_LIBRARIES)
	set(REPOSITORY_FOUND FALSE)
     	set (LIBRARIES_NOT_FOUND jcommon-1.0.9.jar)
    endif (NOT JCOMMON_LIBRARIES)
endif (NOT JCOMMON_LIBRARIES)


if (NOT SERVLET_LIBRARIES)	#not in cache
    FIND_FILE(SERVLET_LIBRARIES 
      NAMES 
	servlet-api.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT SERVLET_LIBRARIES)
	set(REPOSITORY_FOUND FALSE)
  	set (LIBRARIES_NOT_FOUND servlet-api.jar)
    endif (NOT SERVLET_LIBRARIES)  
endif (NOT SERVLET_LIBRARIES)  



 if (REPOSITORY_FOUND)
     if (NOT REPOSITORY_FIND_QUIETLY)
        message(STATUS "Found REPOSITORY libraries")
     endif (NOT REPOSITORY_FIND_QUIETLY)
  else (REPOSITORY_FOUND)
     if (REPOS_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find: ${LIBRARIES_NOT_FOUND}")
     endif (REPOS_FIND_REQUIRED)
  endif (REPOSITORY_FOUND)

  MARK_AS_ADVANCED(REPOS_LIBRARIES)

