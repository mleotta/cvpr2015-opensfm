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

#include <fstream>
#include <string>
#include <vector>
#include "Eigen/Core"
#include "ceres/rotation.h"
#include "glog/logging.h"

namespace openMVG {

BAFile::BAFile(const std::string& filename) {
  std::ifstream file(filename);
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

}  // namespace openMVG
