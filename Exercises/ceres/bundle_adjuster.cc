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
//
// Bundle adjusting an openMVG reconstruction using Ceres Solver.
//
// This example reads a BAF file produced by openMVG and minimizes the
// sum squared reprojection error for the camera model defined by
// openMVG.

#include <string>

#include "ba_file.h"
#include "ceres/ceres.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "reprojection_error.h"

using openMVG::BAFile;
using openMVG::Observation;
using openMVG::ReprojectionError;

int main(int argc, char** argv) {
  // Initialize gflags and glog.
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (argc < 2) {
    std::cerr << "bundle_adjuster <baf_file>\n";
    return 1;
  }

  // Read in the OpenMVG BAF file.
  BAFile baf(argv[1]);

  // Construct the ceres Problem, adding one residual block for each
  // observation.
  ceres::Problem problem;
  for (int point_id = 0; point_id < baf.num_points(); ++point_id) {
    // BAF files store all observations for a 3d point/landmark
    // together.
    const std::vector<Observation>& observations =
        baf.ObservationsForPoint(point_id);
    double* point = baf.GetPoint(point_id);
    for (int i = 0; i < observations.size(); ++i) {
      const Observation& obs = observations[i];
      // Add a residual block per observation, with the intrinsics and
      // pose for the camera obtained from the BAFile object.
      //
      // Notice that we are not doing anything special to deal with
      // the fact that intrinsics may or may not be shared across
      // cameras.
      //
      // Ceres will automatically account for it by looking at the
      // parameter blocks passed to it.
      problem.AddResidualBlock(ReprojectionError::Create(obs.x, obs.y),
                               NULL,
                               baf.GetIntrinsics(obs.intrinsics_id),
                               baf.GetPose(obs.pose_id),
                               point);
    }
  }

  ceres::Solver::Options options;

  // DENSE_SCHUR constructs the schur complement and solves the
  // reduced linear system using a dense Cholesky factorization. For
  // small to medium sized problem this is a perfectly suitable
  // solver.
  //
  // For larger problem one can use SPARSE_SCHUR, or if the problem is
  // so large that factoring the Schur complement is not an option,
  // then ITERATIVE_SCHUR can be used with a suitable preconditioner.
  options.linear_solver_type = ceres::DENSE_SCHUR;
  options.minimizer_progress_to_stdout = true;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);
  std::cout << summary.FullReport() << "\n";

  return 0;
}
