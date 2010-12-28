# - Find Java
# This module finds if Java is installed and determines where the
# include files and libraries are. This code sets the following
# variables:
#  JAVA_RUNTIME    = the full path to the Java runtime
#  JAVA_COMPILE    = the full path to the Java compiler
#  JAVA_ARCHIVE    = the full path to the Java archiver
#


SET(JAVA_BIN_PATH
	${JAVA_HOME}/bin
	$ENV{JAVA_HOME}/bin
	/usr/bin
	/usr/lib/java/bin
	/usr/share/java/bin
	/usr/local/bin
	/usr/local/java/bin
	/usr/local/java/share/bin
)

FIND_PROGRAM(JAVA_RUNTIME
 	java
 	${JAVA_BIN_PATH}
	NO_DEFAULT_PATH
	)

FIND_PROGRAM(JAVA_ARCHIVE
	jar
	${JAVA_BIN_PATH}
	NO_DEFAULT_PATH
	)

FIND_PROGRAM(JAVA_COMPILE
	javac
	${JAVA_BIN_PATH}
	NO_DEFAULT_PATH
)


if (JAVA_RUNTIME)
     set(JAVA_FOUND TRUE)
  endif (JAVA_RUNTIME)

  if (JAVA_FOUND)
     if (NOT JAVA_FIND_QUIETLY)
        message(STATUS "Found JAVA: ${JAVA_COMPILE}")
     endif (NOT JAVA_FIND_QUIETLY)
  else (JAVA_FOUND)
     if (JAVA_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find ")
     endif (JAVA_FIND_REQUIRED)
  endif (JAVA_FOUND)

MARK_AS_ADVANCED(
	JAVA_RUNTIME
	JAVA_ARCHIVE
	JAVA_COMPILE
	)
