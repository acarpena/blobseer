
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

