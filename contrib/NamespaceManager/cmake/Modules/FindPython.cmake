

# Python bindings
find_package(PythonLibs)
if (PYTHON_INCLUDE_PATH)
  FIND_LIBRARY(Boost_PYTHON_LIBRARY   NAMES boost_python         PATHS ${BOOST_ROOT}/lib /usr/lib)
  if (Boost_PYTHON_LIBRARY)
    message(STATUS "Building Python support. Python lib: ${Boost_PYTHON_LIBRARY}")
  else (Boost_PYTHON_LIBRARY)
    message(STATUS "Python library not found. Python support will not be built.")
  endif (Boost_PYTHON_LIBRARY)
else (PYTHON_INCLUDE_PATH)
  message(STATUS "Python headers not found. Python support will not be built.")
endif (PYTHON_INCLUDE_PATH)

