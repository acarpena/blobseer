
# - Try to find the BlobSeer Monitoring library in the MonALISA repository
# Once done this will define
#
#  BSMON_LIB_FOUND - the system found the libraries
#  BSMON_LIBRARIES - the path to the libraries

SET(BSMON_FIND_REQUIRED "TRUE")
SET(BSMON_FOUND TRUE)


if (NOT BSMON_LIBRARIES)	#not in cache
    FIND_FILE(BSMON_LIBRARIES 
      NAMES 
	BSMonitoring.jar
      PATHS
      ${REPOS_ROOT}/lib
      ${REPOS_ROOT}/tomcat/common/lib
      ${LIBRARY_PATH}
    )

    if (NOT BSMON_LIBRARIES)
     	set(BSMON_FOUND FALSE)
     	set (LIBRARIES_NOT_FOUND BSMonitoring.jar)
    endif (NOT BSMON_LIBRARIES)
endif (NOT BSMON_LIBRARIES)


 if (BSMON_FOUND)
     if (NOT BSMON_FIND_QUIETLY)
        message(STATUS "Found BlobSeer Monitoring library")
     endif (NOT BSMON_FIND_QUIETLY)
  else (BSMON_FOUND)
     if (BSMON_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find: ${LIBRARIES_NOT_FOUND}")
     endif (BSMON_FIND_REQUIRED)
  endif (BSMON_FOUND)

  MARK_AS_ADVANCED(BSMON_LIBRARIES)

