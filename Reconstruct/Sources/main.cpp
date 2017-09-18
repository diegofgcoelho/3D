/*
 * main.cpp
 *
 *  Created on: Sep 11, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#include "Includes.h"

#include "FrameSet.h"
#include "MatchSet.h"
#include "Organizer2D.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace cv::sfm;

static void help() {
  cout
      << "\n------------------------------------------------------------------------------------\n"
      << " This program shows the multiview reconstruction capabilities in the \n"
      << " OpenCV Structure From Motion (SFM) module.\n"
      << " It reconstruct a scene from the samples of a video frames \n"
      << " Usage:\n"
      << "        Reconstruct <path_to_file> <stride> <neighbors> <f> <cx> <cy>\n"
      << " where: path_to_file is the file absolute path into your system which contains\n"
      << "        the video to be used for reconstruction. \n"
	  << "        stride is the decimation constant, the stride between frames taken into account \n"
	  << "        neighbors is the the number of neighbors to be used for matching \n"
      << "        f  is the focal length in pixels. \n"
      << "        cx is the image principal point x coordinates in pixels. \n"
      << "        cy is the image principal point y coordinates in pixels. \n"
      << "------------------------------------------------------------------------------------\n\n"
      << endl;
}

int main(int argc, char* argv[])
{
  // Read input parameters
  if ( argc != 7 )
  {
    help();
    exit(0);
  }

  //Printing the video name
  cout << argv[1] << endl;


  // Build instrinsics
	float f  = atof(argv[4]),
		cx = atof(argv[5]), cy = atof(argv[6]);
  Matx33d K = Matx33d( f, 0, cx,
                       0, f, cy,
                       0, 0,  1);
  bool is_projective = true;
  vector<Mat> Rs_est, ts_est, points3d_estimated;

  //Create pointer for STL vector containing all the frames
  //Defining subsampling constant
  unsigned int stride = atoi(argv[2]);
  //Reading frames from video
  FrameSet frames = FrameSet(argv[1], stride);

  cout << "We have a total of " << frames.getNFrames() << " frames." << endl;

  MatchSet matches(frames);
  matches.generateKeyPointsAndDescriptors();
  unsigned int neighbors = atoi(argv[3]);
  matches.generateMatches(neighbors);

  cout << "Feature detection and matching completed." << endl;

  Organizer2D organizer;
  organizer.organizePoints(matches.getMatches(), matches.getKeypoints());
  organizer.savePoints("2Dpoints.txt");

  cout << "2D Point reorganization completed." << endl;

  cout << " nrows = " << organizer.getPoints().at(0).rows << " ncols = " << organizer.getPoints().at(0).cols << endl;

  reconstruct(organizer.getPoints(), Rs_est, ts_est, K, points3d_estimated, is_projective);

  // Print output
  cout << "\n----------------------------\n" << endl;
  cout << "Reconstruction: " << endl;
  cout << "============================" << endl;
  cout << "Estimated 3D points: " << points3d_estimated.size() << endl;
  cout << "Estimated cameras: " << Rs_est.size() << endl;
  cout << "Refined intrinsics: " << endl << K << endl << endl;
  cout << "3D Visualization: " << endl;
  cout << "============================" << endl;
  viz::Viz3d window("Coordinate Frame");
             window.setWindowSize(Size(500,500));
             window.setWindowPosition(Point(150,150));
             window.setBackgroundColor(); // black by default
  // Create the pointcloud
  cout << "Recovering points  ... ";
  // recover estimated points3d
  vector<Vec3f> point_cloud_est;
  for (unsigned int i = 0; i < points3d_estimated.size(); ++i)
    point_cloud_est.push_back(Vec3f(points3d_estimated[i]));
  cout << "[DONE]" << endl;
  cout << "Recovering cameras ... ";
  vector<Affine3d> path;
  for (size_t i = 0; i < Rs_est.size(); ++i)
    path.push_back(Affine3d(Rs_est[i],ts_est[i]));
  cout << "[DONE]" << endl;
  if ( point_cloud_est.size() > 0 )
  {
    cout << "Rendering points   ... ";
    viz::WCloud cloud_widget(point_cloud_est, viz::Color::green());
    window.showWidget("point_cloud", cloud_widget);
    cout << "[DONE]" << endl;
  }
  else
  {
    cout << "Cannot render points: Empty pointcloud" << endl;
  }
  if ( path.size() > 0 )
  {
    cout << "Rendering Cameras  ... ";
    window.showWidget("cameras_frames_and_lines", viz::WTrajectory(path, viz::WTrajectory::BOTH, 0.1, viz::Color::green()));
    window.showWidget("cameras_frustums", viz::WTrajectoryFrustums(path, K, 0.1, viz::Color::yellow()));
    window.setViewerPose(path[0]);
    cout << "[DONE]" << endl;
  }
  else
  {
    cout << "Cannot render the cameras: Empty path" << endl;
  }
  cout << endl << "Press 'q' to close each windows ... " << endl;
  window.spin();
  return 0;
}
