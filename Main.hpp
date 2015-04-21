#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Video.hpp"
#include "Sound.hpp"

#define NUMBER_OF_TAGS 15

using namespace std;
using namespace cv;

const int MIN_NUMBER_OF_MATCHES = 1;
const int WIDTH = 640;				// width of capture
const int HEIGHT = 480; 			// height of capture
const int AREA_SCALE_MIN = 300;
const int AREA_SCALE_MAX = 30000;
const int Z_SCALE_MIN = 1;
const int Z_SCALE_MAX = 1000;
const int SCALE_FACTOR = (Z_SCALE_MAX - Z_SCALE_MIN) / (AREA_SCALE_MAX - AREA_SCALE_MIN);

struct FileRecord {
	int tagID;
	string item;
	static int numberOfRecords;
};

struct Tag {
	int tagID;
	int numberOfVertices[3];
	vector<Point> contours[3];
	float area[3];
};

int loadfile(FileRecord *);
int identifyTag(int, int);
int appendToFile(int, string);
Point2f getTagCenter(Tag &);
Point2f normalizeXY(Point2f);
float findDepth(float);
int getTagID(string, int, FileRecord *);
#endif