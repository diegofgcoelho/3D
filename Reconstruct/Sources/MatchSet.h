/*
 * MatchSet.h
 *
 *  Created on: Sep 11, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#ifndef MATCHSET_H_
#define MATCHSET_H_

#include "Includes.h"
#include "FrameSet.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

class MatchSet {
public:
	MatchSet();
	MatchSet(string videoName, unsigned int stride = 5, unsigned int start = 0);
	MatchSet(FrameSet frames);
	virtual ~MatchSet();
	int generateKeyPointsAndDescriptors();
	int generateMatches(unsigned int nNeighbors);
	inline vector<vector<vector<DMatch> > > getMatches(){
		return this->matches;
	}
	inline vector<vector<KeyPoint> > getKeypoints(){
		return this->keypoints;
	}
	inline void setFrameSet(FrameSet frame){
		this->frameSet = frame;
	}

private:
	FrameSet frameSet;
	vector<vector<KeyPoint> > keypoints;
	vector<Mat> descriptos;
	vector<vector<vector<DMatch> > > matches;
};

#endif /* MATCHSET_H_ */
