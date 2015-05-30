CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ceres_tutorial_exercises)

FIND_PACKAGE(Ceres REQUIRED)
INCLUDE_DIRECTORIES(${CERES_INCLUDE_DIRS})

ADD_EXECUTABLE(curve_fitting curve_fitting.cc read_matrix.cc)
TARGET_LINK_LIBRARIES(curve_fitting ${CERES_LIBRARIES} gflags)

ADD_EXECUTABLE(bundle_adjuster ba_file.cc bundle_adjuster.cc)
TARGET_LINK_LIBRARIES(bundle_adjuster ${CERES_LIBRARIES} gflags)
