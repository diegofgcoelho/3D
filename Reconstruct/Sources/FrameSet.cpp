/*
 * FrameSet.cpp
 *
 *  Created on: Sep 11, 2017
 *      Author: diego
 */

#include "FrameSet.h"

FrameSet::FrameSet() {
	// TODO Auto-generated constructor stub
}

FrameSet::FrameSet(string videoName, unsigned int stride, unsigned int start){
	this->setVideo(videoName, stride, start);
}

FrameSet::~FrameSet() {
	// TODO Auto-generated destructor stub
}

int FrameSet::setVideo(string videoName, unsigned int stride, unsigned int start){
	/*
	 * Input:
	 * videoName is the string representing the file containing the video
	 * stride represent the decimation constant for the frames to be read
	 * start represents the fisrt frame to be read
	 * Output:
	 * Description:
	 * This method read the frames from a video file with name videoName and
	 * undersample the frames according the constant stride (eg, if stride=2,
	 * it will take only every other frame, if stride=3, it will take only one
	 * in every 3 frames) and return in the vector of pointers for matrices that
	 * store the frames. The frames attribute of this class is set with the frames read.
	 * If the input arguments are not valid--for example, stride is bigger than the
	 * number of total frame in he video, it will return FAIL code.
	 */

	//Video object that will be used for opening the video file
	VideoCapture vid(videoName);

	//Opening the video
	vid.open(videoName);
	//Sanity check
	if(!vid.isOpened()){
		cout << "Error: it is not possible to read the video file " << videoName << endl;
		return FILE_FAIL;
	}

	//Reading the total number of frames in the video object
	unsigned int frame_count = vid.get(CV_CAP_PROP_FRAME_COUNT);


	//Sanity check
	if(stride >= frame_count){
		cout << "Error: the total number of frames in the video is smaller than the stride." << endl;
		return FAIL;
	}

	//Resizing the vector
	this->frames.resize(floor(frame_count/stride));

	//Frame index
	unsigned int frame_index = start;

	for(unsigned int i = 0; i < floor(frame_count/stride); i++){
		//Updating the frame index
		frame_index += stride;
		//Setting frame index to be read
		vid.set(CV_CAP_PROP_POS_FRAMES, frame_index);
		//Creating matrix
		Mat frame;
		//Reading frame
		vid >> frame;
		//Setting elements in the output vector
		this->frames[i] = frame;
	}

	return SUCCESS;
}