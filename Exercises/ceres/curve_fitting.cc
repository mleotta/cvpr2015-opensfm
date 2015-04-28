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

#include <fstream>
#include <iostream>
#include <string>
#include "ceres/ceres.h"
#include "eigen3/Eigen/Core"
#include "glog/logging.h"

// Simple, not so robust function to read in the curve fitting data.
//
// Comment lines start with #. The first non-comment line contains the
// number of rows and columns.  The remaining file is a matrix in
// column major form.
bool ReadMatrix(const std::string& filename, Eigen::MatrixXd* matrix) {
  int num_rows = 0;
  int num_cols = 0;
  int row = -1;

  std::ifstream file(filename);
  std::string line;
  std::getline(file, line);
  while(!file.eof()) {
    std::getline(file, line);
    if (row >= num_rows) {
      break;
    }

    if (line.substr(0, 1) == "#") {
      continue;
    }

    std::istringstream iss(line);
    if (num_rows < 1) {
      if (!(iss >> num_rows >> num_cols)) {
        return false;
      }
      matrix->resize(num_rows, num_cols);
      row = 0;
      continue;
    }

    for (int col = 0; col < num_cols; ++col) {
      double entry;
      if (!(iss >> entry)) {
        return false;
      }
      (*matrix)(row, col) = entry;
    }

    ++row;
  }
  return true;
}

// Templated cost functor that implements the evaluation of the cost
// and the Jacobian for a single observation.
//
// It stores the observed x and y as data, and given values for m and
// c, evaluates
//
//  residual = y - exp(m * x + c);
//
// This class is not used directly, before being passed to
// ceres::Problem, it is wrapped in a ceres::AutoDiffCostFunction
// object which automatically differentiates the
// ExponentialResidual::operator() method.
class ExponentialResidual {
 public:
  ExponentialResidual(double x, double y)
      : x_(x), y_(y) {}

  // Templated functor evaluating the residual.
  //
  // Notice that x_ and y_ are doubles which are explicitly converted
  // to type T before being used.
  template <typename T>
  bool operator()(const T* const m, const T* const c, T* residual) const {
    residual[0] = T(y_) - exp(m[0] * T(x_) + c[0]);
    return true;
  }

  // Factory methods which simplifies and hides the creation of the
  // CostFunction object.
  static ceres::CostFunction* Create(double x, double y) {
    return new ceres::AutoDiffCostFunction<ExponentialResidual, 1, 1, 1>(
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
        ExponentialResidual::Create(data(i, 0),data(i, 1)),
        NULL,
        &m, &c);
  }

  ceres::Solver::Options options;
  options.max_num_iterations = 25;
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
