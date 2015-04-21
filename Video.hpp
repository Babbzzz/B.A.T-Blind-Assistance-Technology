#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <opencv2/opencv.hpp>
#include <iostream>
#include "Main.hpp"

using namespace cv;
using namespace std;

const int THRESHOLD = 19;
const int BLOCK_SIZE = 31;
const int MIN_THRESHOLD = 136;
const int MAX_THRESHOLD = 248;
const float RATIO_1_LOWER = 0.35; 	//0.38
const float RATIO_1_UPPER =	0.80;	//0.47 	//.76
const float RATIO_2_LOWER =	0.05; 	//0.06
const float RATIO_2_UPPER =	0.15; 	//0.12
const float EPSILON_FACTOR = 0.043;

int getTag(struct Tag &, int tagID = 0);

#endif