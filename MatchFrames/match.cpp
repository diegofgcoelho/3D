/*
 * match.cpp
 *
 *  Created on: May 16, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#include <iostream>
#include <string>
#include <math.h>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#define SUCCESS 0
#define FAIL -1
#define FILE_FAIL -2

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

int readframes(String, unsigned, vector<Mat*>**);
int matchframes(vector<Mat*>**, vector< vector<KeyPoint>* >** veckeypoints, vector< vector<DMatch>* >**);

int main(int argc, char** argv){
	//Create pointer for STL vector containing all the frames
	vector<Mat*>* vecframes = NULL;
	//Defining subsampling constant
	unsigned int stride = 5;
	//Reading frames from video
	int readframes_code = readframes("vid_sample.mp4", stride, &vecframes);
	//Checking if the reading was successful
	if(readframes_code != SUCCESS){
		cout << "Error: problem reading video frames. Exiting ..." << endl;
		return FAIL;
	}

	//Creating the vector of vectors of DMatch. Each matching needs a vector of DMatch.
	vector< vector<DMatch>* >* vecmatches = NULL;
	vector< vector<KeyPoint>* >* veckeypoints = NULL;
	int matchframes_code = matchframes(&vecframes, &veckeypoints, &vecmatches);

	if(matchframes_code != SUCCESS){
		cout << "Error: problem when matching the frames. Exiting ..." << endl;
		return FAIL;
	}

	unsigned int nframematch = (unsigned int) atoi(argv[1]);
	cout << "This video have a total of " << vecframes->size() << " frames after the subsampling." << endl;
	cout << "Displaying the features match for the frames ";
	cout << nframematch;
	cout << " and its right neighbor." << endl;
	if(nframematch >= vecframes->size()){
		cout << "The frame number is out of bound. We will instead use the last frame and its left neighbor." << endl;
		nframematch = vecframes->size()-2;
	}
	Mat imgmatches, imgkeypoints_0, imgkeypoints_1;
	drawKeypoints((*vecframes->at(nframematch)), (*veckeypoints->at(nframematch)), imgkeypoints_0);
	imshow("Features of frame i", imgkeypoints_0);
	drawKeypoints((*vecframes->at(nframematch+1)), (*veckeypoints->at(nframematch+1)), imgkeypoints_1);
	imshow("Features of frame i+1", imgkeypoints_1);

	if(!(veckeypoints->at(nframematch)->empty() || veckeypoints->at(nframematch+1)->empty() || vecmatches->at(nframematch)->empty())){
		drawMatches((*vecframes->at(nframematch)), (*veckeypoints->at(nframematch)), (*vecframes->at(nframematch+1)), (*veckeypoints->at(nframematch+1)), (*vecmatches->at(nframematch)), imgmatches);
		imshow("Matches", imgmatches);
	} else {
		cout << "The matching for these frames is empty, as you can see." << endl;
	}

	waitKey(0);


}

int readframes(String filename, unsigned stride, vector<Mat*>** vecframes){
	/*
	 * Input:
	 * filename is the string representing the file containing the video
	 * stride represent the decimation constant for the frames to be read
	 * Output:
	 * vecframe is a vector or pointers for matrices containing the frames
	 * that will be used for processing
	 * Description:
	 * this function read the frames from a video file with name filename and
	 * undersample the frames according the constant stride (eg, if stride=2,
	 * it will take only every other frame, if stride=3, it will take only one
	 * in every 3 frames) and return in the vector of pointers for matrices that
	 * store the frames. The memory for the pointer is allocated inside the function
	 * and must be released by the calling function.
	 */

	//Allocating memory for vecframes
	*vecframes = new vector<Mat*>();

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
	//Resizing the vector
	(*vecframes)->resize(floor(frame_count/stride));

	//Frame index
	unsigned int frame_index = 0;

	for(unsigned int i = 0; i < floor(frame_count/stride); i++){
		//Updating the frame index
		frame_index += stride;
		//Setting frame index to be read
		vid.set(CV_CAP_PROP_POS_FRAMES, frame_index);
		//Creating matrix
		Mat* frame = new Mat;
		//Reading frame
		vid >> *frame;
		//Setting elements in the output vector
		(*(*vecframes))[i] = frame;
	}

	return SUCCESS;
}

int matchframes(vector<Mat*>** vecframes, vector< vector<KeyPoint>* >** veckeypoints, vector< vector<DMatch>* >** vecmatches){
	/*
	 * Input:
	 * vecframes contains the frames from the video that was read and will be processed.
	 * Output:
	 * vecmatches contains the matches between the subsequent frames from the video
	 * to be processed.
	 * veckeypoints contains the keypoints for each frame
	 * Description:
	 * this function performs the feature description, extraction and matching between the
	 * frames from the video to be processed. Currently, it used brute force matching
	 * (it needs to be changed) between the features on neighbor frames. This function also
	 * eliminates bad matches by removing matches whose Euclidean distances are greater than a
	 * minimum (see code to check the treshold). This function also removes bad matches whose
	 * angle formed by the features in the query and train keypoints exceed a maximum angle. This
	 * agle is defined by setting both frames side by side and connecting the corresponding keypoints
	 * and measuring the angle formed with the horizontal.
	 */

	//Allocating and resizing the vecmatches and veckeypoints
	*vecmatches = new vector< vector<DMatch>* >();
	*veckeypoints = new vector< vector<KeyPoint>* >();
	(*vecmatches)->resize((*vecframes)->size()-1);
	(*veckeypoints)->resize((*vecframes)->size());

	//Defining detector that will be used for detecting the features in all frames. Using default values
	Ptr<SURF> detector = SURF::create(400, 4, 3, false, false);

	//Defining the descriptors for the frames that will be used inside the for loop
	Mat descriptors_0, descriptors_1;

	//Allocating memory for the keypoints of the first frame
	(*veckeypoints)->at(0) = new vector<KeyPoint>();
	//Computing the keypoints for the first frame
	detector->detect((*(*vecframes)->at(0)), (*(*veckeypoints)->at(0)));
	//Computing the descriptors for the first frame
	detector->compute((*(*vecframes)->at(0)), (*(*veckeypoints)->at(0)), descriptors_1);

	for(unsigned int i = 0; i < (*vecframes)->size()-1; i++){
		//Allocating memory for the keypoints of the (i+1)th frame
		(*veckeypoints)->at(i+1) = new vector<KeyPoint>();

		//Shifting the descriptors for performance
		descriptors_0 = descriptors_1;

		//Defining the descriptor matcher
		Ptr<DescriptorMatcher> matcher = BFMatcher::create("BruteForce");

		//Detecting keypoints for frame i+1
		detector->detect((*(*vecframes)->at(i+1)), (*(*veckeypoints)->at(i+1)));

		//Computing descriptors for frame i+1
		detector->compute((*(*vecframes)->at(i+1)), (*(*veckeypoints)->at(i+1)), descriptors_1);

		//Matching the features for frames i and i+1
		(*vecmatches)->at(i) = new vector<DMatch>();
		matcher->match(descriptors_0, descriptors_1, (*(*vecmatches)->at(i)));

		//Removing bad matches
		vector<DMatch>* pvecm = (*vecmatches)->at(i);
		vector<KeyPoint>* pveck0 = (*veckeypoints)->at(i);
		vector<KeyPoint>* pveck1 = (*veckeypoints)->at(i+1);

		//Specifying a treshold to delete bad matches. Any matches whose distance is greater than treshold will be deleted.
		double dtreshold = 500;
		double atreshold = cos(5*M_PI/180);//When changing, remember that the argument have to be in the first quadrant!
		unsigned int j = 0;
		while (j < pvecm->size()){//Note how the index j is incremented
			//Getting the positions of the query and train keypoints
			Point2f kp0 = pveck0->at(pvecm->at(j).queryIdx).pt;
			Point2f kp1 = pveck1->at(pvecm->at(j).trainIdx).pt;
			Point2f dkp = kp1-kp0;
			//Computing the Euclidean distance between the pixels of the matching keypoints
			double dist = norm(dkp);
			//Computing the angle between the keypoints when images are side by side
			Point2f framedim = Point2f((*vecframes)->at(0)->rows, 0.0);
			//Shift only kp1
			kp1 = kp1+framedim;
			//Updating dkp (the difference between the keypoint of frame i and the shifted keypoint of frame i+1)
			dkp = kp1-kp0;
			double ang = dkp.x/norm(dkp);
			//Eliminating matches whose distance is greater than the minimum
			if((dist > dtreshold) || (fabs(ang) < atreshold)) pvecm->erase(pvecm->begin()+j); else j++;
			//if((dtreshold < dist)) pvecm->erase(pvecm->begin()+j); else j++;
		}
	}
	return SUCCESS;
}
