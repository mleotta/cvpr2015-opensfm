CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(gtsam_examples)

# Boost
find_package(Boost COMPONENTS thread filesystem date_time system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# GTSAM
FIND_PACKAGE(GTSAM REQUIRED)
INCLUDE_DIRECTORIES(${GTSAM_INCLUDE_DIR})

# Create targets for all examples
file(GLOB scripts "*.cpp")

foreach(script ${scripts})
  get_filename_component(target ${script} NAME_WE)
  add_executable(${target} ${script})
  target_link_libraries(${target} gtsam ${Boost_LIBRARIES})
  add_custom_target(${target}.run ${PROJECT_BINARY_DIR}/${target} ${ARGN} DEPENDS ${target})
  message(STATUS "Adding ${target}")
endforeach(script)


