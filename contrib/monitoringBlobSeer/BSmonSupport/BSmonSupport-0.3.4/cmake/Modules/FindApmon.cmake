# from wzdftpd
# via http://fresh.t-systems-sfr.com/unix/src/privat2/wzdftpd-0.8.2.tar.gz:a/wzdftpd-0.8.2/cmake/FindOpenSSL.cmake
# - Try to find the ApMon library
# Once done this will define
#
#  APMON_FOUND - system has the Apmon library
#  APMON_INCLUDE_DIR - the ApMon include directory
#  APMON_LIBRARIES - The libraries needed to use ApMon

if (APMON_INCLUDE_DIR AND APMON_LIBRARIES)

  # in cache already
  SET(APMON_FOUND TRUE)

else (APMON_INCLUDE_DIR AND APMON_LIBRARIES)
  FIND_PATH(APMON_INCLUDE_DIR ApMon.h mon_constants.h monitor_utils.h proc_utils.h utils.h xdr.h
     ${APMON_ROOT}/include
     /usr/include/
     /usr/local/include/
     /sw/lib
     /sw/local/lib
     $ENV{ProgramFiles}/APMON/include/
     $ENV{SystemDrive}/APMON/include/

     ${APMON_ROOT}/include/apmon/
     /usr/include/apmon/
     /usr/local/include/apmon/
     /sw/lib/apmon/
     /sw/local/lib/apmon/
     $ENV{ProgramFiles}/APMON/include/apmon/
     $ENV{SystemDrive}/APMON/include/apmon/
  )

  if(WIN32 AND MSVC)
  else(WIN32 AND MSVC)
    FIND_LIBRARY(APMON_LIBRARIES NAMES apmoncpp
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

  if (APMON_INCLUDE_DIR AND APMON_LIBRARIES)
     set(APMON_FOUND TRUE)
  endif (APMON_INCLUDE_DIR AND APMON_LIBRARIES)

  if (APMON_FOUND)
     if (NOT APMON_FIND_QUIETLY)
        message(STATUS "Found apmon: ${APMON_LIBRARIES}")
     endif (NOT APMON_FIND_QUIETLY)
  else (APMON_FOUND)
     if (APMON_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find ")
     endif (APMON_FIND_REQUIRED)
  endif (APMON_FOUND)

  MARK_AS_ADVANCED(APMON_INCLUDE_DIR APMON_LIBRARIES)

endif (APMON_INCLUDE_DIR AND APMON_LIBRARIES)