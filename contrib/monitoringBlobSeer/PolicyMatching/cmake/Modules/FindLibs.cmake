
# - Try to find the libraries in the PROJECT_LIBS
# Once done this will define
#
#  PROJECT_LIBS_FOUND - the system found the libraries
#  PROJECT_LIBRARIES - the path to the libraries

SET(PROJECT_LIBS_FIND_REQUIRED "TRUE")
SET(PROJECT_LIBS_FOUND TRUE)
SET(PROJECT_ROOT ${BS_SECURITY_POLICIES_SOURCE_DIR})

if (NOT XERCES_LIBRARIES)	#not in cache
    FIND_FILE(XERCES_LIBRARIES 
      NAMES 
	resolver.jar
	serializer.jar
	xercesImpl.jar
	xercesSamples.jar
	xml-apis.jar
      PATHS
      ${PROJECT_ROOT}/lib
      ${LIBRARY_PATH}
      ${LIBS_PATH}
    )

    if (NOT XERCES_LIBRARIES)
     	set(PROJECT_LIBS_FOUND FALSE)
     	set (LIBRARIES_NOT_FOUND xerces)
    endif (NOT XERCES_LIBRARIES)
endif (NOT XERCES_LIBRARIES)



if (NOT LOG4J_LIBRARIES)	#not in cache
    FIND_FILE(LOG4J_LIBRARIES 
      NAMES 
	log4j-1.2.15.jar
      PATHS
      ${PROJECT_ROOT}/lib
      ${LIBRARY_PATH}
    )

    if (NOT LOG4J_LIBRARIES)
    	set(PROJECT_LIBS_FOUND FALSE)
    	set (LIBRARIES_NOT_FOUND log4j-1.2.15.jar)
    endif (NOT LOG4J_LIBRARIES)
endif (NOT LOG4J_LIBRARIES)


if (NOT POSTGRES_LIBRARIES)	#not in cache
    FIND_FILE(POSTGRES_LIBRARIES 
      NAMES 
	postgresql-8.4-701.jdbc4.jar
      PATHS
      ${PROJECT_ROOT}/lib
      ${LIBRARY_PATH}
    )

    if (NOT POSTGRES_LIBRARIES)
    	set(PROJECT_LIBS_FOUND FALSE)
    	set (LIBRARIES_NOT_FOUND postgresql-8.4-701.jdbc4.jar)
    endif (NOT POSTGRES_LIBRARIES)
endif (NOT POSTGRES_LIBRARIES)

 if (PROJECT_LIBS_FOUND)
     if (NOT PROJECT_LIBS_FIND_QUIETLY)
        message(STATUS "Found PROJECT_LIBS libraries")
     endif (NOT PROJECT_LIBS_FIND_QUIETLY)
  else (PROJECT_LIBS_FOUND)
     if (PROJECT_LIBS_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find: ${LIBRARIES_NOT_FOUND}")
     endif (PROJECT_LIBS_FIND_REQUIRED)
  endif (PROJECT_LIBS_FOUND)
  
  set (PROJECT_LIBRARIES "${XERCES_LIBRARIES}:${LOG4J_LIBRARIES}:${POSTGRES_LIBRARIES}")

  MARK_AS_ADVANCED(PROJECT_LIBRARIES)

