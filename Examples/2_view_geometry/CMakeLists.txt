cmake_minimum_required(VERSION 2.8)
project(2_view_geometry)

find_package(OpenCV)

add_executable(calibration ./src/calibration.cpp)
target_link_libraries(calibration ${OpenCV_LIBRARIES})

add_executable(orb_detector ./src/orb_detector.cpp)
target_link_libraries(orb_detector ${OpenCV_LIBRARIES})

add_executable(orb_matcher ./src/orb_matcher.cpp)
target_link_libraries(orb_matcher ${OpenCV_LIBRARIES})
