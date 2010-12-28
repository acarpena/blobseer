
# - Try to find the ApMon library
# Once done this will define
#
#  APMON_JAVA_FOUND - system has the Apmon library
#  APMON_JAVA_LIBRARIES - The libraries needed to use ApMon

SET(APMON_JAVA_FIND_REQUIRED "TRUE")

if (APMON_JAVA_LIBRARIES)

  # in cache already
  SET(APMON_JAVA_FOUND TRUE)

else (APMON_JAVA_LIBRARIES)
  if(WIN32 AND MSVC)
  else(WIN32 AND MSVC)
    FIND_FILE(APMON_JAVA_LIBRARIES NAMES apmon.jar
      PATHS
      ${APMON_JAVA_ROOT}/lib
      /sw/lib
      /sw/local/lib
      /usr/lib
      /usr/local/lib

      ${APMON_JAVA_ROOT}/lib/apmon
      /sw/lib/apmon
      /sw/local/lib/apmon
      /usr/lib/apmon
      /usr/local/lib/apmon
    )
  endif(WIN32 AND MSVC)

  if (APMON_JAVA_LIBRARIES)
     set(APMON_JAVA_FOUND TRUE)
  endif (APMON_JAVA_LIBRARIES)

  if (APMON_JAVA_FOUND)
     if (NOT APMON_JAVA_FIND_QUIETLY)
        message(STATUS "Found apmon: ${APMON_JAVA_LIBRARIES}")
     endif (NOT APMON_JAVA_FIND_QUIETLY)
  else (APMON_JAVA_FOUND)
     if (APMON_JAVA_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find ")
     endif (APMON_JAVA_FIND_REQUIRED)
  endif (APMON_JAVA_FOUND)

  MARK_AS_ADVANCED(APMON_JAVA_LIBRARIES)

endif (APMON_JAVA_LIBRARIES)
