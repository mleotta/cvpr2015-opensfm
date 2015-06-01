// Ceres Solver - A fast non-linear least squares minimizer
// Copyright 2015 Google Inc. All rights reserved.
// http://ceres-solver.org/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: sameeragarwal@google.com (Sameer Agarwal)

#include "ba_file.h"

#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "Eigen/Core"
#include "ceres/rotation.h"
#include "glog/logging.h"

namespace openMVG {
namespace {

// Return a random number sampled from a uniform distribution in the range
// [0,1].
inline double RandDouble() {
  double r = static_cast<double>(rand());
  return r / RAND_MAX;
}

// Marsaglia Polar method for generation standard normal (pseudo)
// random numbers http://en.wikipedia.org/wiki/Marsaglia_polar_method
inline double RandNormal() {
  double x1, x2, w;
  do {
    x1 = 2.0 * RandDouble() - 1.0;
    x2 = 2.0 * RandDouble() - 1.0;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1.0 || w == 0.0 );

  w = sqrt((-2.0 * log(w)) / w);
  return x1 * w;
}

void PerturbPoint3(const double sigma, double* point) {
  if (sigma <= 0) return;
  for (int i = 0; i < 3; ++i) {
    point[i] += RandNormal() * sigma;
  }
}

double Median(std::vector<double>* data) {
  int n = data->size();
  std::vector<double>::iterator mid_point = data->begin() + n / 2;
  std::nth_element(data->begin(), mid_point, data->end());
  return *mid_point;
}

}  // namespace

BAFile::BAFile(const std::string& filename) {
  std::ifstream file(filename.c_str());
  if (!file.good()) {
    LOG(FATAL) << "Unable to open file: " << filename;
  }

  file >> num_intrinsics_;
  file >> num_poses_;
  file >> num_points_;

  LOG(INFO) << "Reading BAF file with:"
            << " num_intrinsics: " << num_intrinsics_
            << " num_poses: " << num_poses_
            << " num_points: " << num_points_;

  CHECK_GE(num_intrinsics_, 1);
  CHECK_GE(num_poses_, 1);
  CHECK_GE(num_points_, 1);

  intrinsics_.resize(6 * num_intrinsics_);
  poses_.resize(6 * num_poses_);
  points_.resize(3 * num_points_);
  observations_.resize(num_points_);

  // Read the intrinsics.
  for (int i = 0; i < intrinsics_.size(); ++i) {
    CHECK(file >> intrinsics_[i]);
  }

  for (int i = 0; i < num_poses_; ++i) {
    double* angle_axis = &poses_[6 * i];
    double* center = &poses_[6 * i + 3];
    double rotation_matrix[9];
    for (int j = 0; j < 9; ++j) {
      CHECK(file >> rotation_matrix[j]);
    }

    ceres::RotationMatrixToAngleAxis(
        ceres::ColumnMajorAdapter3x3<const double>(rotation_matrix),
        angle_axis);

    CHECK((file >> center[0] >> center[1] >> center[2]));
  }

  for (int i = 0; i < num_points_; ++i) {
    CHECK((file >> points_[3 * i] >> points_[3 * i + 1] >> points_[3 * i + 2]));
    int num_observations_for_point = 0;
    CHECK(file >> num_observations_for_point);
    CHECK_GT(num_observations_for_point, 0);
    for (int j = 0; j < num_observations_for_point; ++j) {
      Observation observation;
      CHECK(file
            >> observation.intrinsics_id
            >> observation.pose_id
            >> observation.x
            >> observation.y);

      VLOG(2) << "observation:"
              << " " <<  observation.intrinsics_id
              << " " <<  observation.pose_id
              << " " << observation.x << " " << observation.y;

      observations_[i].push_back(observation);
    }
  }
}

void BAFile::WriteToPLYFile(const std::string& filename) const {
  std::ofstream of(filename.c_str());

  of << "ply"
     << '\n' << "format ascii 1.0"
     << '\n' << "element vertex " << num_poses() + num_points()
     << '\n' << "property float x"
     << '\n' << "property float y"
     << '\n' << "property float z"
     << '\n' << "property uchar red"
     << '\n' << "property uchar green"
     << '\n' << "property uchar blue"
     << '\n' << "end_header" << std::endl;

  // Export extrinsic data (i.e. camera centers) as green points.
  double center[3];
  for (int i = 0; i < num_poses(); ++i)  {
    const double* center = GetPose(i) + 3;
    of << center[0] << " " << center[1] << " " << center[2]
       << " 0 255 0" << "\n";
  }

  // Export the structure (i.e. 3D Points) as white points.
  for (int i = 0; i < num_points(); ++i) {
    const double* point = GetPoint(i);
    of << point[0] << " " << point[1] << " " << point[2]
       << " 255 255 255\n";
  }
  of.close();
}

void BAFile::Normalize() {
  // Compute the marginal median of the geometry.
  std::vector<double> tmp(num_points_);
  Eigen::Vector3d median;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < num_points(); ++j) {
      tmp[j] = GetPoint(j)[i];
    }
    median(i) = Median(&tmp);
  }

  for (int i = 0; i < num_points(); ++i) {
    Eigen::Map<Eigen::Vector3d> point(GetPoint(i));
    tmp[i] = (point - median).lpNorm<1>();
  }

  const double median_absolute_deviation = Median(&tmp);

  // Scale so that the median absolute deviation of the resulting
  // reconstruction is 100.
  const double scale = 100.0 / median_absolute_deviation;

  VLOG(2) << "median: " << median.transpose();
  VLOG(2) << "median absolute deviation: " << median_absolute_deviation;
  VLOG(2) << "scale: " << scale;

  // X = scale * (X - median)
  for (int i = 0; i < num_points(); ++i) {
    Eigen::Map<Eigen::Vector3d> point(GetPoint(i));
    point = scale * (point - median);
  }

  // center = scale * (center - median)
  for (int i = 0; i < num_poses(); ++i) {
    Eigen::Map<Eigen::Vector3d> center(GetPose(i) + 3);
    center = scale * (center - median);
  }
}

void BAFile::Perturb(const double rotation_sigma,
                     const double position_sigma,
                     const double point_sigma) {
  CHECK_GE(point_sigma, 0.0);
  CHECK_GE(rotation_sigma, 0.0);
  CHECK_GE(position_sigma, 0.0);

  for (int i = 0; i < num_points(); ++i) {
    PerturbPoint3(point_sigma, GetPoint(i));
  }

  for (int i = 0; i < num_poses(); ++i) {
    PerturbPoint3(rotation_sigma, GetPose(i));
    PerturbPoint3(position_sigma, GetPose(i) + 3);
  }
}

}  // namespace openMVG
