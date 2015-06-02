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
// ==============================================================
// Bundle adjusting an openMVG reconstruction using Ceres Solver.
// ==============================================================
//
// This example reads a BAF file produced by openMVG and minimizes the
// sum squared reprojection error for the camera model defined by
// openMVG.
//
// Usage:  bundle_adjuster --input <baf_file>
//
// Use Data/MirebeauStHilaireStatue/sfm_data.baf as the data for this
// exercise.
//
// Exercise 1
// ==========
// Partially remove gauge ambiguity by setting the extrinsics of the
// first camera constant.
//
// As implemented, the entire reconstruction is free to move in space
// via a similarity transformation without any change in the
// reprojection error. Fixing the extrinsics of the first camera
// eliminates six out the seven degrees of freedom of the
// reconstruction.
//
// http://ceres-solver.org/nnls_modeling.html#Problem::SetParameterBlockConstant__doubleP
//
// Exercise 2
// ==========
// Hold the principal point of each camera constant.
//
// As implemented, the set of intrinsics optimized by the bundle
// adjustment are the focal length, principal point and the radial
// distortion parameters. Use a SubsetParameterization to hold the
// principal point for all the cameras constant.
//
// http://ceres-solver.org/nnls_modeling.html#SubsetParameterization
//
// Exercise 3
// ==========
// Experiment with different linear solvers.
//
// Change the linear solver being used by the Levenberg-Marquardt
// algorithm to SPARSE_NORMAL_CHOLESKY and observe the difference in
// performance between a generic linear solver and one which exploits
// the sparsity structure of the bundle adjustment problem.
//
// http://ceres-solver.org/nnls_solving.html#linearsolver

#include <string>

#include "ba_file.h"
#include "ceres/ceres.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "reprojection_error.h"

DEFINE_string(input, "", "BAF File containing an openMVG reconstruction.");
DEFINE_double(rotation_sigma, 0.0, "Standard deviation of camera rotation "
              "perturbation.");
DEFINE_double(position_sigma, 0.0, "Standard deviation of the camera "
              "position perturbation.");
DEFINE_double(point_sigma, 0.0, "Standard deviation of the point "
              "perturbation.");
DEFINE_string(initial_ply, "", "Export the BAF file data as a PLY file after "
              "perturbation but before bundle adjustment.");
DEFINE_string(final_ply, "", "Export the refined BAF file data as a PLY "
              "file after bundle adjustment.");
DEFINE_int32(random_seed, 38401, "Random seed used to set the state "
             "of the pseudo random number generator used to generate "
             "the pertubations.");

using openMVG::BAFile;
using openMVG::Observation;
using openMVG::ReprojectionError;

int main(int argc, char** argv) {
  // Initialize gflags and glog.
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

 if (FLAGS_input.empty()) {
   LOG(ERROR) << "Usage: bundle_adjuster --input=baf_file";
    return 1;
  }

  // Read in the OpenMVG BAF file.
  BAFile ba_file(FLAGS_input);
  srand(FLAGS_random_seed);
  ba_file.Normalize();
  ba_file.Perturb(FLAGS_rotation_sigma,
                  FLAGS_position_sigma,
                  FLAGS_point_sigma);

  if (!FLAGS_initial_ply.empty()) {
    ba_file.WriteToPLYFile(FLAGS_initial_ply);
  }

  // Construct the ceres Problem, adding one residual block for each
  // observation.
  ceres::Problem problem;
  for (int point_id = 0; point_id < ba_file.num_points(); ++point_id) {
    // BAF files store all observations for a 3d point/landmark
    // together.
    const std::vector<Observation>& observations =
        ba_file.ObservationsForPoint(point_id);
    double* point = ba_file.GetPoint(point_id);
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
                               ba_file.GetIntrinsics(obs.intrinsics_id),
                               ba_file.GetPose(obs.pose_id),
                               point);
    }
  }

  ceres::Solver::Options options;

  // DENSE_SCHUR constructs the Schur complement and solves the
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

  if (!FLAGS_final_ply.empty()) {
    ba_file.WriteToPLYFile(FLAGS_final_ply);
  }

  return 0;
}
