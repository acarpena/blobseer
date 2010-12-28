
# - Try to find the ApMon library
# Once done this will define
#
#  APMON_FOUND - system has the Apmon library
#  APMON_LIBRARIES - The libraries needed to use ApMon

SET(APMON_FIND_REQUIRED "TRUE")

if (APMON_LIBRARIES)

  # in cache already
  SET(APMON_FOUND TRUE)

else (APMON_LIBRARIES)
  if(WIN32 AND MSVC)
  else(WIN32 AND MSVC)
    FIND_FILE(APMON_LIBRARIES NAMES apmon.jar
      PATHS
      ${APMON_ROOT}/lib
      /sw/lib
      /sw/local/lib
      /usr/lib
      /usr/local/lib

      ${APMON_ROOT}/lib/apmon
      /sw/lib/apmon
      /sw/local/lib/apmon
      /usr/lib/apmon
      /usr/local/lib/apmon
    )
  endif(WIN32 AND MSVC)

  if (APMON_LIBRARIES)
     set(APMON_FOUND TRUE)
  endif (APMON_LIBRARIES)

  if (APMON_FOUND)
     if (NOT APMON_FIND_QUIETLY)
        message(STATUS "Found apmon: ${APMON_LIBRARIES}")
     endif (NOT APMON_FIND_QUIETLY)
  else (APMON_FOUND)
     if (APMON_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find ")
     endif (APMON_FIND_REQUIRED)
  endif (APMON_FOUND)

  MARK_AS_ADVANCED(APMON_LIBRARIES)

endif (APMON_LIBRARIES)
