// Copyright 2015 Google Inc. All rights reserved.
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

#ifndef EXERCISES_CERES_BA_FILE_H_
#define EXERCISES_CERES_BA_FILE_H_

#include <string>
#include <vector>

namespace openMVG {
struct Observation {
  int intrinsics_id;
  int pose_id;
  double x;
  double y;
};

// A parse for OpenMVG's BAF file format.
class BAFile {
 public:
  explicit BAFile(const std::string& filename);

  // TODO(sameeragarwal): Implement these functions.
  // void WriteToFile(const std::string& filename) const;
  // void WriteToPLYFile(const std::string& filename) const;

  // Move the "center" of the reconstruction to the origin, where the
  // center is determined by computing the marginal median of the
  // points. The reconstruction is then scaled so that the median
  // absolute deviation of the points measured from the origin is
  // 100.0.
  //
  // The reprojection error of the problem remains the same.
  // void Normalize();

  // Perturb the camera pose and the geometry with random normal
  // numbers with corresponding standard deviations.

  // void Perturb(const double rotation_sigma,
  //              const double translation_sigma,
  //              const double point_sigma);

  const std::vector<Observation>& ObservationsForPoint(int point_id) {
    return observations_[point_id];
  }

  double* GetPoint(int point_id) { return &points_[point_id * 3]; }
  double* GetPose(int pose_id)   { return &poses_[pose_id * 6]; }
  double* GetIntrinsics(int intrinsics_id) {
    return &intrinsics_[intrinsics_id * 6];
  }

  int num_poses()  const { return num_poses_;  }
  int num_points() const { return num_points_; }
  int num_intrinsics() const { return num_intrinsics_; }
  int num_observations() const { return num_observations_; }

 private:
  int num_intrinsics_;
  int num_poses_;
  int num_points_;
  int num_observations_;

  std::vector<double> intrinsics_;
  std::vector<double> poses_;
  std::vector<double> points_;
  std::vector<std::vector<Observation> > observations_;
};

}  // namespace openMVG

#endif  // EXERCISES_CERES_BA_FILE_H_
