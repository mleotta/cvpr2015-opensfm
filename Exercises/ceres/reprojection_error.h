// Copyright (c) 2015 Pierre Moulon.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EXERCISES_CERES_REPROJECTION_ERROR_H_
#define EXERCISES_CERES_REPROJECTION_ERROR_H_

#include "ceres/ceres.h"
#include "ceres/rotation.h"

namespace openMVG {

/*Ceres functor for openMVG Pinhole_Intrinsic_Radial_K3 camera
 *
 *Data parameter blocks are the following <2,6,6,3>
 *- 2 => residuals dimension,
 *- 6 => intrinsic data block [focal, principal point [x,y], disto [K1, K2, K3]],
 *- 6 => camera extrinsic data block (camera orientation and translation) [R;t],
 *       - rotation(angle axis), and translation [rX,rY,rZ,tx,ty,tz].
 *- 3 => a 3D point data block.
 *
 */
class ReprojectionError {
 public:
  ReprojectionError(const double x, const double y)
      : x_(x), y_(y) {}

  // Map intrinsics params between openMVG & ceres camera data
  // parameter block.
  enum {
    OFFSET_FOCAL_LENGTH = 0,
    OFFSET_PRINCIPAL_POINT_X = 1,
    OFFSET_PRINCIPAL_POINT_Y = 2,
    OFFSET_DISTO_K1 = 3,
    OFFSET_DISTO_K2 = 4,
    OFFSET_DISTO_K3 = 5,
  };

  /**
   * @param[in] intrinsics: Camera intrinsics data block
   * @param[in] extrinsics: Camera pose data block
   *   - 3 for rotation(angle axis), 3 for translation
   * @param[in] point: 3d point.
   * @param[out] residuals
   */
  template <typename T> bool operator()(const T* const intrinsics,
                                        const T* const pose,
                                        const T* const point,
                                        T* residuals) const {
    // Apply external parameters (Pose)
    const T* rotation = pose;
    const T* center = pose + 3;

    T translated_point[3];
    translated_point[0] = point[0] - center[0];
    translated_point[1] = point[1] - center[1];
    translated_point[2] = point[2] - center[2];

    T rotated_translated_point[3];
    // Rotate the point according the camera rotation
    ceres::AngleAxisRotatePoint(rotation,
                                translated_point,
                                rotated_translated_point);

    // Transform the point from homogeneous to euclidean (undistorted point)
    const T x_u = rotated_translated_point[0] / rotated_translated_point[2];
    const T y_u = rotated_translated_point[1] / rotated_translated_point[2];

    // Apply intrinsic parameters
    const T& focal = intrinsics[OFFSET_FOCAL_LENGTH];
    const T& principal_point_x = intrinsics[OFFSET_PRINCIPAL_POINT_X];
    const T& principal_point_y = intrinsics[OFFSET_PRINCIPAL_POINT_Y];
    const T& k1 = intrinsics[OFFSET_DISTO_K1];
    const T& k2 = intrinsics[OFFSET_DISTO_K2];
    const T& k3 = intrinsics[OFFSET_DISTO_K3];

    // Apply distortion (xd,yd) = disto(x_u,y_u)
    const T r2 = x_u * x_u + y_u * y_u;
    const T r4 = r2 * r2;
    const T r6 = r4 * r2;
    const T r_coeff = (T(1.0) + k1 * r2 + k2 * r4 + k3 * r6);
    const T x_d = x_u * r_coeff;
    const T y_d = y_u * r_coeff;

    // Apply focal length and principal point to get the final image
    // coordinates
    const T projected_x = principal_point_x + focal * x_d;
    const T projected_y = principal_point_y + focal * y_d;

    // Compute and return the error is the difference between the
    // predicted and observed position
    residuals[0] = projected_x - T(x_);
    residuals[1] = projected_y - T(y_);
    return true;
  }

  static ceres::CostFunction* Create(const double x, const double y) {
    return new ceres::AutoDiffCostFunction<ReprojectionError, 2, 6, 6, 3>(
        new ReprojectionError(x, y));
  }

  double x_;
  double y_;
};

}  // namespace openMVG

#endif  // EXERCISES_CERES_REPROJECTION_ERROR_H_
