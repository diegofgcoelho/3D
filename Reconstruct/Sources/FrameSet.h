/*
 * FrameSet.h
 *
 *  Created on: Sep 11, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#ifndef FRAMESET_H_
#define FRAMESET_H_

#include "Includes.h"

using namespace std;
using namespace cv;

class FrameSet {
public:
	FrameSet();
	FrameSet(string videoName, unsigned int stride = 5, unsigned int start = 0);
	virtual ~FrameSet();
	int setVideo(string videoName, unsigned int stride = 5, unsigned int start = 0);
	inline vector<Mat> getFrames(){
		return this->frames;
	}
	inline int getNFrames(){
		return this->frames.size();
	}
private:
	vector<Mat> frames;
};

#endif /* FRAMESET_H_ */
