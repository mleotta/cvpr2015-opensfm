
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(openMVG_external C CXX)

# guard against in-source builds
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed.")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# ==============================================================================
# Setup to use OpenMVG by using cmake find package
# ==============================================================================
FIND_PACKAGE(OpenMVG REQUIRED)
INCLUDE_DIRECTORIES(${OPENMVG_INCLUDE_DIRS})
ADD_EXECUTABLE(main main.cpp)
TARGET_LINK_LIBRARIES(main ${OPENMVG_LIBRARIES})

