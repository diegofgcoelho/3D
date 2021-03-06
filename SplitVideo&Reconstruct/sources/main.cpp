/*
 * main.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: Diego
 */

#define CERES_FOUND true

#include <opencv2/opencv.hpp>
#include <opencv2/sfm.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#define SUCCESS 0
#define FAIL -1
#define FILE_FAIL -2

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace cv::sfm;

int readFramesSave(String filename, unsigned stride, vector<unsigned int> pos){
	/*
	 * Input:
	 * filename is the string representing the file containing the video
	 * stride represent the decimation constant for the frames to be read
	 * pos is a vector of unsigned integers containing the frames postions that must be saved
	 * Output:
	 * None
	 * Description:
	 * this function read the frames from a video file with name filename and
	 * undersample the frames according the constant stride (eg, if stride=2,
	 * it will take only every other frame, if stride=3, it will take only one
	 * in every 3 frames) and return and saves it in the local directory.
	 */

	//Video object that will be used for opening the video file
	VideoCapture vid(filename);

	//Opening the video
	vid.open(filename);
	//Sanity check
	if(!vid.isOpened()){
		cout << "Error: it is not possible to read the video file " << filename << endl;
		return FILE_FAIL;
	}

	//Reading the total number of frames in the video object
	unsigned int frame_count = vid.get(CV_CAP_PROP_FRAME_COUNT);

	//Frame index
	unsigned int frame_index = 0;

	unsigned int zz = 0;

	for(unsigned int i = 0; i < floor(frame_count/stride); i++){
		//Setting frame index to be read
		vid.set(CV_CAP_PROP_POS_FRAMES, frame_index);

		if(pos[zz] == static_cast<unsigned int>(frame_index/stride) && zz < pos.size()){
			zz++;
			//Creating matrix
			Mat* frame = new Mat;
			//Reading frame
			vid >> *frame;
			stringstream ss;
			ss << zz;
			cout << "Saving frame" << endl;
			imwrite("./"+ss.str()+".jpg", *frame);
		}
		//Updating the frame index
		frame_index += stride;
	}

	cout << "Total number of frames: " << frame_count << endl;

	return SUCCESS;
}

static void help() {
  cout
      << "\n------------------------------------------------------------------------------------\n"
      << " This program saves specified frames of a video in the disk. Usage: \n"
      << "        SplitVideo&Save <path_to_video file> stride pos0 pos1 pos2 ... posN\n"
      << " where: path_to_file is the video file absolute path into your system which\n"
      << "        stride is the decimation constant (stride) \n"
	  << "        and pos0 pos1 ... are the positions of the frames to be saved after the subsampling \n"
      << "------------------------------------------------------------------------------------\n\n"
      << endl;
}

int getdir(const string _filename, vector<String> &files)
{
  ifstream myfile(_filename.c_str());
  if (!myfile.is_open()) {
    cout << "Unable to read file: " << _filename << endl;
    exit(0);
  } else {;
    size_t found = _filename.find_last_of("/\\");
    string line_str, path_to_file = _filename.substr(0, found);
    while ( getline(myfile, line_str) )
      files.push_back(path_to_file+string("/")+line_str);
  }
  return 1;
}

int main(int argc, char* argv[]){

	help();
	vector<unsigned int> pos(argc-6);
	for(unsigned int i = 0; i < pos.size(); i++){
		pos[i] = atoi(argv[i+6]);
	}

	//Reading frames from video
	int readframes_code = readFramesSave(argv[1], atoi(argv[2]), pos);

	// Read input parameters
	if ( argc != 10 )
	{
	help();
	exit(0);
	}
	// Parse the image paths
	vector<String> images_paths(4);
	images_paths[0] = "./1.jpg";
	images_paths[1] = "./2.jpg";
	images_paths[2] = "./3.jpg";
	images_paths[3] = "./4.jpg";
	// Build instrinsics
	float f  = atof(argv[3]),
		cx = atof(argv[4]), cy = atof(argv[5]);
	Matx33d K = Matx33d( f, 0, cx,
					   0, f, cy,
					   0, 0,  1);

	bool is_projective = true;
	vector<Mat> Rs_est, ts_est, points3d_estimated;

	reconstruct(images_paths, Rs_est, ts_est, K, points3d_estimated, is_projective);
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
	for (int i = 0; i < points3d_estimated.size(); ++i)
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


