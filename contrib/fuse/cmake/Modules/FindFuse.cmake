# via http://fresh.t-systems-sfr.com/unix/src/privat2/wzdftpd-0.8.2.tar.gz:a/wzdftpd-0.8.2/cmake/FindOpenSSL.cmake
# - Try to find the OpenSSL encryption library
# Once done this will define
#
#  CONFIG_FOUND - system has the config++ library
#  CONFIG_INCLUDE_DIR - the config++ include directory
#  CONFIG_LIBRARIES - The libraries needed to use config++

if (FUSE_INCLUDE_DIR AND FUSE_LIBRARIES)

  # in cache already
  SET(FUSE_FOUND TRUE)

else (FUSE_INCLUDE_DIR AND FUSE_LIBRARIES)
  FIND_PATH(FUSE_INCLUDE_DIR fuse.h
     ${FUSE_ROOT}/include
     /usr/include/
     /usr/local/include/
     /sw/lib
     /sw/local/lib
     $ENV{ProgramFiles}/CONFIG/include/
     $ENV{SystemDrive}/CONFIG/include/
  )

  if(WIN32 AND MSVC)
  else(WIN32 AND MSVC)
    FIND_LIBRARY(FUSE_LIBRARIES NAMES fuse
      PATHS
      ${FUSE_ROOT}/lib
      /sw/lib
      /sw/local/lib
      /usr/lib
      /usr/local/lib
    )
  endif(WIN32 AND MSVC)

  if (FUSE_INCLUDE_DIR AND FUSE_LIBRARIES)
     set(FUSE_FOUND TRUE)
  endif (FUSE_INCLUDE_DIR AND FUSE_LIBRARIES)

  if (FUSE_FOUND)
     if (NOT FUSE_FIND_QUIETLY)
        message(STATUS "Found fuse: ${FUSE_LIBRARIES}")
     endif (NOT FUSE_FIND_QUIETLY)
  else (FUSE_FOUND)
     if (FUSE_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find fuse")
     endif (FUSE_FIND_REQUIRED)
  endif (FUSE_FOUND)

  MARK_AS_ADVANCED(FUSE_INCLUDE_DIR FUSE_LIBRARIES)

endif (FUSE_INCLUDE_DIR AND FUSE_LIBRARIES)