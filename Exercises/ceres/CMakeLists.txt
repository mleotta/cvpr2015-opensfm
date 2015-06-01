CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Make sure we build in Release mode, otherwise the automatic
# differentiation code is extremely slow.
SET(CMAKE_BUILD_TYPE Release CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
  FORCE)

PROJECT(ceres_tutorial_exercises)

FIND_PACKAGE(Ceres REQUIRED)
FIND_PACKAGE(gflags REQUIRED)

INCLUDE_DIRECTORIES(${CERES_INCLUDE_DIRS})

ADD_EXECUTABLE(curve_fitting curve_fitting.cc read_matrix.cc)
TARGET_LINK_LIBRARIES(curve_fitting ${CERES_LIBRARIES} ${GFLAG_LIBRARIES})

ADD_EXECUTABLE(bundle_adjuster ba_file.cc bundle_adjuster.cc)
TARGET_LINK_LIBRARIES(bundle_adjuster ${CERES_LIBRARIES} ${GFLAG_LIBRARIES})
