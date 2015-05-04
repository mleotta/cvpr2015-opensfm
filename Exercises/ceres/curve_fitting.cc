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

// Fitting a curve using Ceres Solver.
//
// This example shows how to fit the curve
//
//  y = e^(mx + c)
//
//  to a given dataset. It assumes that the noise in the observations
//  is normally distributed, so the sum of squared errors is
//  minimized.
//
// Usage:  curve_fitting <data_file>
//
//
// Exercise 1
//
// Use automatic differentiation instead of numeric differentiation.
// This will involve changing the implementation of
// ExponentialResidual::operator() to use a template parameter T
// instead of doubles.
//
// Reference: http://ceres-solver.org/nnls_tutorial.html#derivatives
//
// Exercise 2: Use a robust loss function.
//
// The data Data/curve_fitting/curve_fitting_data2.txt has outliers in
// it, you can verify that by running the current program with this
// data and observing how the estimated parameters are far away from
// the true parameters. Changing the loss function associated with
// each residual block will reduce the sensitivity to outliers.
//
// Reference: http://ceres-solver.org/nnls_tutorial.html#robust-curve-fitting

#include <iostream>
#include <string>
#include "ceres/ceres.h"
#include "eigen3/Eigen/Core"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "read_matrix.h"

// Cost functor that implements the evaluation of the cost and the
// Jacobian for a single observation.
//
// It stores the observed x and y as data, and given values for m and
// c, evaluates
//
//  residual = y - exp(m * x + c);
//
// This class is not used directly, before being passed to
// ceres::Problem, it is wrapped in a ceres::NumericDiffCostFunction
// object which automatically differentiates the
// ExponentialResidual::operator() method.
class ExponentialResidual {
 public:
  ExponentialResidual(double x, double y)
      : x_(x), y_(y) {}

  bool operator()(const double* const m,
                  const double* const c,
                  double* residual) const {
    residual[0] = y_ - exp(m[0] * x_ + c[0]);
    return true;
  }

  // Factory methods which simplifies and hides the creation of the
  // CostFunction object.
  static ceres::CostFunction* Create(double x, double y) {
    return new ceres::NumericDiffCostFunction<ExponentialResidual,
                                              ceres::CENTRAL, 1, 1, 1>(
        new ExponentialResidual(x, y));
  }

 private:
  const double x_;
  const double y_;
};

int main(int argc, char** argv) {
  // Initialize gflags and glog.
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (argc < 2) {
    std::cerr << "curve_fitting <data_file>\n";
    return 1;
  }

  Eigen::MatrixXd data;
  CHECK(ReadMatrix(argv[1], &data))
      << "Unable to read in data correctly from : " << argv[1];

  // Initial guesses of the parameter values.
  double m = 0.0;
  double c = 0.0;

  // Construct the problem by adding one residual block for every
  // observation.
  ceres::Problem problem;
  for (int i = 0; i < data.rows(); ++i) {
    problem.AddResidualBlock(
        ExponentialResidual::Create(data(i, 0), data(i, 1)),
        NULL, /* Loss Function */
        &m, &c);
  }

  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = true;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  std::cout << summary.BriefReport() << "\n";

  std::cout << "Initial  m: " << 0.0 << " c: " << 0.0 << "\n";
  std::cout << "Expected m: " << 0.3 << " c: "  << 0.1 << "\n";
  std::cout << "Computed m: " << m << " c: " << c << "\n";

  return 0;
}
