# - Try to find the MITIE package -
# If found the following will be defined
#
#  MITIE_FOUND - MITIE package found on the system
#  MITIE_INCLUDE_DIR - Directory of the MITIE package include files
#  MITIE_LIBRARY - Where libmitie resides
#

FIND_PATH(MITIE_INCLUDE_DIR mitie.h
  PATHS
  ${MITIE_INSTALL_PREFIX}/include
  ${INCLUDE_SEARCH_PATHES}
)

FIND_LIBRARY(MITIE_LIBRARY NAMES mitie
  PATHS
  ${MITIE_INSTALL_PREFIX}/lib
  ${LIB_SEARCH_PATHES}
)

IF(MITIE_INCLUDE_DIR AND MITIE_LIBRARY)
   SET(MITIE_FOUND TRUE)
ENDIF(MITIE_INCLUDE_DIR AND MITIE_LIBRARY)

IF(MITIE_FOUND)
  IF(NOT MITIE_FIND_QUIETLY)
    MESSAGE(STATUS "Found MITIE package: ${MITIE_LIBRARY}")
  ENDIF(NOT MITIE_FIND_QUIETLY)
ELSE(MITIE_FOUND)
  IF(MITIE_FIND_REQUIRED)
	  MESSAGE(FATAL_ERROR "Could not find MITIE package! Please download and install MITIE from https://github.com/mit-nlp/MITIE")
  ENDIF(MITIE_FIND_REQUIRED)
ENDIF(MITIE_FOUND)

