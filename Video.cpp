#include "Video.hpp"

int getTag(Tag &identifiedTag, int tagID) {
	Mat frame;
	int hierarchySize;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int contourLevel, tagLevel[3], tagLevelIndex;
	float ratio[3];
	int previousPolygonVertexCount[2], match;
	VideoCapture capture(1);
	if (!capture.isOpened()) {
		cerr << "Could not open video stream!" << endl;
		return -1;
	} else {
		for (;;) {
			capture >> frame;
			Canny(frame, frame, MIN_THRESHOLD, MAX_THRESHOLD);
			findContours(frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
			hierarchySize = hierarchy.size();
			for(int i = 0; i < hierarchySize - 3; i++) {
				if (hierarchy[i][0] == -1 && hierarchy[i][1] == -1) {
			 		if (hierarchy[i][2] == i + 1 && hierarchy[i][3] == i - 1) {
						if (hierarchy[i + 1][0] == -1 && hierarchy[i + 1][2] == i + 2) {
							if (hierarchy[i + 2][0] == -1 && hierarchy[i + 2][2] == i + 3) {
								if (hierarchy[i + 3][0] == -1 && hierarchy[i + 3][2] == -1) {
									tagLevel[0] = i - 1;
									tagLevel[1] = i + 1;
									tagLevel[2] = i + 3;
									identifiedTag.area[0] = contourArea(contours[tagLevel[0]]);
									identifiedTag.area[1] = contourArea(contours[tagLevel[1]]);
									identifiedTag.area[2] = contourArea(contours[tagLevel[2]]);
									ratio[0] = 1;
									ratio[1] = identifiedTag.area[1] / identifiedTag.area[0];
									ratio[2] = identifiedTag.area[2] / identifiedTag.area[0];
									if (RATIO_1_LOWER < ratio[1] && ratio[1] < RATIO_1_UPPER && RATIO_2_LOWER < ratio[2] && ratio[2] < RATIO_2_UPPER) {
										approxPolyDP(contours[tagLevel[1]], identifiedTag.contours[1], arcLength(contours[tagLevel[2]], true) * EPSILON_FACTOR, true);
										approxPolyDP(contours[tagLevel[2]], identifiedTag.contours[2], arcLength(contours[tagLevel[2]], true) * EPSILON_FACTOR, true);
										identifiedTag.numberOfVertices[1] = identifiedTag.contours[1].size();
										identifiedTag.numberOfVertices[2] = identifiedTag.contours[2].size();
										if (identifiedTag.numberOfVertices[1] == previousPolygonVertexCount[0] && identifiedTag.numberOfVertices[2] == previousPolygonVertexCount[1]) {
											match++;
											if (match >= MIN_NUMBER_OF_MATCHES) {
												identifiedTag.tagID = identifyTag(identifiedTag.numberOfVertices[1], identifiedTag.numberOfVertices[2]);
												if (identifiedTag.tagID == -1) {
													cerr << "Invalid tag!" << endl;
													return -1;
												}
												if (tagID == 0 || tagID == identifiedTag.tagID) 
													return 0;
											}
										} else {
											match = 0;
											previousPolygonVertexCount[0] = identifiedTag.numberOfVertices[1];
											previousPolygonVertexCount[1] = identifiedTag.numberOfVertices[2];
										}
									}
									i += 3;
								}
							}
						}
					}
				}
			}
		}
	}
}