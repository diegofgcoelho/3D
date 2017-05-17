/*
 * read.cpp
 *
 *  Created on: May 15, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#define SUCCESS 0
#define FAIL -1
#define FILE_FAIL -2

using namespace cv;
using namespace std;

int readframes(String, unsigned, vector<Mat*>**);


int main(int argc, char** argv){

	vector<Mat*>* vecframes = new vector<Mat*>();
	unsigned int stride = 10;
	readframes("vid_sample.mp4", stride, &vecframes);

	cout<<"After the subsampling, we have a total of "<<vecframes->size()<<" frames."<<endl;

	namedWindow("The first frame.");
	imshow("The first frame.", (*(*vecframes)[0]));

	namedWindow("The 100th frame.");
	imshow("The 100th frame.", (*(*vecframes)[99]));

	waitKey(0);

	vecframes->~vector();

	return 0;
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
	 * store the frames
	 */

	//Video object that will be used for opening the video file
	VideoCapture vid(filename);

	//Opening the video
	vid.open(filename);
	//Sanity check
	if(!vid.isOpened()){
		cout << "Error: no possible to read the video file " << filename << endl;
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

