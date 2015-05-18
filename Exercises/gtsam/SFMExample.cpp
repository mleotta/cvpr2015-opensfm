// A simple Structure from motion exercise

/**
 * A structure-from-motion example with landmarks
 *  - The robot rotates around the landmarks, always facing towards the cube
 */

#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/ProjectionFactor.h>
#include <gtsam/geometry/SimpleCamera.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/inference/Symbol.h>

#include <vector>

using namespace std;
using namespace gtsam;

// The landmarks form a 10 meter cube
std::vector<gtsam::Point3> createPoints() {

  // Create the set of ground-truth landmarks
  std::vector<gtsam::Point3> points;
  points.push_back(gtsam::Point3(10.0, 10.0, 10.0));
  points.push_back(gtsam::Point3(-10.0, 10.0, 10.0));
  points.push_back(gtsam::Point3(-10.0, -10.0, 10.0));
  points.push_back(gtsam::Point3(10.0, -10.0, 10.0));
  points.push_back(gtsam::Point3(10.0, 10.0, -10.0));
  points.push_back(gtsam::Point3(-10.0, 10.0, -10.0));
  points.push_back(gtsam::Point3(-10.0, -10.0, -10.0));
  points.push_back(gtsam::Point3(10.0, -10.0, -10.0));

  return points;
}

// Poses that go around the object
std::vector<gtsam::Pose3> createPoses() {

  // Create the set of ground-truth poses
  std::vector<gtsam::Pose3> poses;
  double radius = 30.0;
  int i = 0;
  double theta = 0.0;
  gtsam::Point3 up(0, 0, 1);
  gtsam::Point3 target(0, 0, 0);
  for (; i < 8; ++i, theta += 2 * M_PI / 8) {
    gtsam::Point3 position = gtsam::Point3(radius * cos(theta),
        radius * sin(theta), 0.0);
    gtsam::SimpleCamera camera = gtsam::SimpleCamera::Lookat(position, target,
        up);
    poses.push_back(camera.pose());
  }
  return poses;
}

// main
int main(int argc, char* argv[]) {

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

  // Add a prior on pose x1. This indirectly specifies where the origin is.
  noiseModel::Diagonal::shared_ptr poseNoise = noiseModel::Diagonal::Sigmas(
      (Vector(6) << Vector3::Constant(0.3), Vector3::Constant(0.1))); // 30cm std on x,y,z 0.1 rad on roll,pitch,yaw
  graph.push_back(PriorFactor<Pose3>(Symbol('x', 0), poses[0], poseNoise)); // add directly to graph

  // Simulated measurements from each camera pose, adding them to the factor graph
  for (size_t i = 0; i < poses.size(); ++i) {
    for (size_t j = 0; j < points.size(); ++j) {
      SimpleCamera camera(poses[i], *K);
      Point2 measurement = camera.project(points[j]);
      graph.push_back(
          GenericProjectionFactor<Pose3, Point3, Cal3_S2>(measurement,
              measurementNoise, Symbol('x', i), Symbol('l', j), K));
    }
  }

  // Because the structure-from-motion problem has a scale ambiguity, the problem is still under-constrained
  // Here we add a prior on the position of the first landmark. This fixes the scale by indicating the distance
  // between the first camera and the first landmark. All other landmark positions are interpreted using this scale.
  noiseModel::Isotropic::shared_ptr pointNoise = noiseModel::Isotropic::Sigma(3,
      0.1);
  graph.push_back(PriorFactor<Point3>(Symbol('l', 0), points[0], pointNoise)); // add directly to graph
  graph.print("Factor Graph:\n");

  // Create the data structure to hold the initial estimate to the solution
  // Intentionally initialize the variables off from the ground truth
  Values initialEstimate;
  for (size_t i = 0; i < poses.size(); ++i)
    initialEstimate.insert(Symbol('x', i),
        poses[i].compose(
            Pose3(Rot3::rodriguez(-0.1, 0.2, 0.25),
                Point3(0.05, -0.10, 0.20))));
  for (size_t j = 0; j < points.size(); ++j)
    initialEstimate.insert(Symbol('l', j),
        points[j].compose(Point3(-0.25, 0.20, 0.15)));
  initialEstimate.print("Initial Estimates:\n");

  /* Optimize the graph and print results */
  Values result = DoglegOptimizer(graph, initialEstimate).optimize();
  result.print("Final results:\n");

  return 0;
}
/* ************************************************************************* */

