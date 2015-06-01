// A simple Structure from motion exercise

/**
 * A structure-from-motion example with landmarks
 *  - The robot rotates around the landmarks, always facing towards the cube
 */

#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/ProjectionFactor.h>
#include <gtsam/geometry/SimpleCamera.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/inference/Symbol.h>

#include <vector>

using namespace std;
using namespace gtsam;

// Small quantities to add to GT initial estimate
const Pose3 kSmallPose(Rot3::rodriguez(-0.1, 0.2, 0.25),
    Point3(0.05, -0.1, 0.2));
const Point3 kSmallDelta(-0.25, 0.20, 0.15);

// Create the set of ground-truth landmarks
// The landmarks form a 10 meter cube
std::vector<Point3> createPoints() {
  std::vector<Point3> points;
  points.push_back(Point3(10.0, 10.0, 10.0));
  points.push_back(Point3(-10.0, 10.0, 10.0));
  points.push_back(Point3(-10.0, -10.0, 10.0));
  points.push_back(Point3(10.0, -10.0, 10.0));
  points.push_back(Point3(10.0, 10.0, -10.0));
  points.push_back(Point3(-10.0, 10.0, -10.0));
  points.push_back(Point3(-10.0, -10.0, -10.0));
  points.push_back(Point3(10.0, -10.0, -10.0));
  return points;
}

// Create the set of ground-truth poses
// They go around the cube created above
std::vector<Pose3> createPoses() {
  std::vector<Pose3> poses;
  double radius = 30.0;
  int i = 0;
  double theta = 0.0;
  Point3 up(0, 0, 1);
  Point3 target(0, 0, 0);
  for (; i < 8; ++i, theta += 2 * M_PI / 8) {
    Point3 position = Point3(radius * cos(theta), radius * sin(theta), 0.0);
    SimpleCamera camera = SimpleCamera::Lookat(position, target, up);
    poses.push_back(camera.pose());
  }
  return poses;
}

// main
int main(int argc, char* argv[]) {
  using noiseModel::Isotropic;

  // Define the camera calibration parameters
  Cal3_S2::shared_ptr K(new Cal3_S2(50.0, 50.0, 0.0, 50.0, 50.0));

  // Define the camera observation noise model
  noiseModel::Isotropic::shared_ptr measurementNoise =
      noiseModel::Isotropic::Sigma(2, 1.0); // one pixel in u and v

  // Create the set of ground-truth landmarks
  vector<Point3> points = createPoints();

  // Create the set of ground-truth poses
  vector<Pose3> poses = createPoses();

  // Create a factor graph
  NonlinearFactorGraph graph;

  // TODO: Add a prior on pose x1. This indirectly specifies where the origin is.
  // The mean should be poses[0], with key Symbol('x', 0)
  // Hint: Use a PriorFactor

  // Simulated measurements from each camera pose, adding them to the factor graph
  for (size_t i = 0; i < poses.size(); ++i) {
    for (size_t j = 0; j < points.size(); ++j) {
      SimpleCamera camera(poses[i], *K);
      Point2 measurement = camera.project(points[j]);
      // TODO: add a ProjectionFactor on the point using Symbol('l', j) as key
      // Hint: use a GenericProjectionFactor
    }
  }

  // Because the structure-from-motion problem has a scale ambiguity, the problem is still under-constrained
  // TODO: add a prior on the position of the first landmark. This fixes the scale by indicating the distance
  // between the first camera and the first landmark. All other landmark positions are interpreted using this scale.
  // Hint: use a PriorFactor on point with mean points[0] and key Symbol('l', 0)
  graph.print("Factor Graph:\n");

  // Create the data structure to hold the initial estimate to the solution
  // Intentionally initialize the variables off from the ground truth
  Values initialEstimate;
  for (size_t i = 0; i < poses.size(); ++i)
    initialEstimate.insert(Symbol('x', i), poses[i].compose(kSmallPose));
  for (size_t j = 0; j < points.size(); ++j)
    initialEstimate.insert(Symbol('l', j), points[j].compose(kSmallDelta));
  initialEstimate.print("Initial Estimates:\n");

  /* Optimize the graph and print results */
  Values result = GaussNewtonOptimizer(graph, initialEstimate).optimize();
  result.print("Final results:\n");

  return 0;
}
/* ************************************************************************* */

