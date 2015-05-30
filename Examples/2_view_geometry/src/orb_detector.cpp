/**
 * @file SURF_detector
 * @brief SURF keypoint detection + keypoint drawing with OpenCV functions
 * @author A. Huaman
 */

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

void readme();

/**
 * @function main
 * @brief Main function
 */
int main( int argc, char** argv )
{
  if( argc != 2 )
  { readme(); return -1; }

  Mat img = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );

  if( !img.data )
  { std::cout<< " --(!) Error reading image " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using ORB
  ORB detector;

  std::vector<KeyPoint> keypoints;

  detector.detect( img, keypoints );

  //-- Draw keypoints
  Mat img_keypoints;

  drawKeypoints( img, keypoints, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

  //-- Show detected (drawn) keypoints
  imshow("Keypoints", img_keypoints );

  waitKey(0);

  return 0;
}

/**
 * @function readme
 */
void readme()
{ std::cout << " Usage: ./orb_detector <img1>" << std::endl; }
