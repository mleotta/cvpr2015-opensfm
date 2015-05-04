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

#include "read_matrix.h"

#include <fstream>
#include <iostream>
#include <string>
#include "eigen3/Eigen/Core"
#include "glog/logging.h"

bool ReadMatrix(const std::string& filename, Eigen::MatrixXd* matrix) {
  int num_rows = 0;
  int num_cols = 0;
  int row = -1;

  std::ifstream file(filename);
  while (!file.eof()) {
    std::string line;
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
