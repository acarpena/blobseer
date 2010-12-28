# - Try to find the BlobSeer library
# Once done this will define
#
#  FBLOBSEER_FOUND - system has the FBLOBSEER library
#  FBLOBSEER_LIBRARIES - The libraries needed to use FBLOBSEER

if (FBLOBSEER_LIBRARIES)

  # in cache already
  SET(FBLOBSEER_FOUND TRUE)

else (FBLOBSEER_LIBRARIES)
  
  FIND_LIBRARY(FBLOBSEER_NS   
			NAMES fbs_namespace          
			PATHS ${FBLOBSEER_ROOT}/lib 
			/usr/lib
			)

  if(WIN32 AND MSVC)
  else(WIN32 AND MSVC)
    FIND_LIBRARY(FBLOBSEER_NS NAMES fbs_namespace  
      PATHS
      ${FBLOBSEER_ROOT}/lib
      /sw/lib
      /sw/local/lib
      /usr/lib
      /usr/local/lib
    )

  endif(WIN32 AND MSVC)

  if (FBLOBSEER_NS)
		set (FBLOBSEER_LIBRARIES ${FBLOBSEER_NS})
  endif (FBLOBSEER_NS)


  if (FBLOBSEER_LIBRARIES)
     set(FBLOBSEER_FOUND TRUE)
  endif (FBLOBSEER_LIBRARIES)

  if (FBLOBSEER_FOUND)
     if (NOT FBLOBSEER_FIND_QUIETLY)
        message(STATUS "Found FBLOBSEER: ${FBLOBSEER_LIBRARIES}")
     endif (NOT FBLOBSEER_FIND_QUIETLY)
  else (FBLOBSEER_FOUND)
     if (FBLOBSEER_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find FBLOBSEER")
     endif (FBLOBSEER_FIND_REQUIRED)
  endif (FBLOBSEER_FOUND)

  MARK_AS_ADVANCED(FBLOBSEER_LIBRARIES)

endif (FBLOBSEER_LIBRARIES)