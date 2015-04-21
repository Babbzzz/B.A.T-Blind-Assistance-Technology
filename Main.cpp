#include "Main.hpp"

int FileRecord::numberOfRecords = 0;

int trackTag(Tag &identifiedTag, int tagID) {
	Mat frame, gray;
	int hierarchySize;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int tagLevel[3];
	float ratio[3];
	Point2f tagPosition, normalizedXY;
	float depth;
	if (loadBeep() == -1) {
		cerr << "Failed to initialize sound buffer!" << endl;
		return -1;
	}
	VideoCapture capture(1);
	if (!capture.isOpened()) {
		cerr << "Could not open video stream!" << endl;
		return -1;
	} else {
		for (;;) {
			capture >> frame;
			cvtColor(frame, gray, CV_RGB2GRAY);
			adaptiveThreshold(gray, gray, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, BLOCK_SIZE, THRESHOLD);
			//Canny(frame, frame, MIN_THRESHOLD, MAX_THRESHOLD);
			findContours(gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
			hierarchySize = hierarchy.size();
			for (int i = 1; i < hierarchySize - 1; i++) {
				if (hierarchy[i][0] == -1 && hierarchy[i][1] == -1) {
				 	if (hierarchy[i][2] == i + 1 && hierarchy[i][3] == i - 1) {
						if (hierarchy[i + 1][0] == -1 && hierarchy[i + 1][1] == -1 && hierarchy[i + 1][2] == -1) {
							tagLevel[0] = i - 1;
							tagLevel[1] = i;
							tagLevel[2] = i + 1;
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
								identifiedTag.tagID = identifyTag(identifiedTag.numberOfVertices[1], identifiedTag.numberOfVertices[2]);
								if (identifiedTag.tagID == -1) {
									cerr << "Unknown Tag!" << endl;
								}
								if (tagID == identifiedTag.tagID) { 
									cout << "Tag ID: " << identifiedTag.tagID << endl;
									tagPosition = getTagCenter(identifiedTag);
									normalizedXY = normalizeXY(tagPosition);
									depth = findDepth(identifiedTag.area[1]);
									//cout << normalizedXY.x << " " << normalizedXY.y << " " << depth << endl;
									if (generateBeep(normalizedXY.x, normalizedXY.y, depth) == -1) {
										cerr << "Error opening file!" << endl;
										return -1;
									}
								}
							} else {
								cerr << "Nested contour but does not look like a Tag!" << endl;
							}
							i++;
						}
					}
				}
			}
			/*
			if ((char)waitKey(100) == '0'){
				return 0;
			}
			*/
		}
	}
}

int recordSearch(int identifiedTagID, string item, FileRecord *records) {
	int tagID;
	int recordIndex = -1;
	for (int i = 0; i < FileRecord::numberOfRecords; i++) {
		if (item.compare(records[i].item) == 0) {
			return -2;
		}
		if (records[i].tagID == identifiedTagID) {
			recordIndex = i;
		}
	}
	return recordIndex;
}

int updateFile(FileRecord *records) {
	ofstream mapping ("Mapping.txt");
	if (!mapping.is_open())
		return -1;
	for (int i = 0; i < FileRecord::numberOfRecords; i++) {
		mapping << records[i].tagID << " " << records[i].item << endl; 
	}
	mapping.close();
	return 0;
}

int main() {
	FileRecord records[NUMBER_OF_TAGS];
	int mode, tagID;
	Tag identifiedTag;
	int recordIndex;
	string item;
	char ch;
	//namedWindow("BAT" ,CV_WINDOW_AUTOSIZE);
	if (loadfile(records) == -1) {
		cerr << "Could not find Mapping.txt file! Maybe first run :)" << endl;
	}
	do {
		cout << "Enter mode: " << endl;
		cin >> mode;
		switch (mode) {
			case 1: cout << "Searching for tag..." << endl;
					if (getTag(identifiedTag) == -1) {
						cerr << "Tag identification failed!" << endl;
						return -1;
					}
					cout << "Tag detected!" << endl;
					cout << "TagID: " << identifiedTag.tagID << endl;
					cout << "Enter name of item: " << endl;
					cin >> item;
					recordIndex = recordSearch(identifiedTag.tagID, item, records);
					if (recordIndex == -2) {
						cerr << "Item already tagged!" << endl;
					} else if (recordIndex == -1) {
						// insert
						records[FileRecord::numberOfRecords].tagID = identifiedTag.tagID;
						records[FileRecord::numberOfRecords].item = item;
						if (appendToFile(records[FileRecord::numberOfRecords].tagID, records[FileRecord::numberOfRecords].item) == -1) {
							cerr << "Storing to file failed!" << endl;
							return -1;
						}
						FileRecord::numberOfRecords++;
						playSavedSuccessfully();
					} else {
						// update
						cout << "Tag already in use for another item! Do you want to use it for this item?(y/n)";
						cin >> ch;
						if (ch == 'y' || ch == 'Y') {
							records[recordIndex].item = item;
							if (updateFile(records) == -1) {
								cerr << "File updation failed!" << endl;
								return -1;
							}
							playSavedSuccessfully();
						}
					}
					break;
			case 2: cout << "Enter name of item to be searched: " << endl;
					cin >> item;
					//cout << "Current number of records: " << FileRecord::numberOfRecords << endl;
					tagID = getTagID(item, FileRecord::numberOfRecords, records);
					if (tagID == -1) {
						cerr << "Item lookup failed!" << endl;
						return -1;
					}
					cout << "Searching for " << item << "..." << endl;
					cout << "Identifying tag..." << endl;
					if (trackTag(identifiedTag, tagID) == -1) {
						cerr << "Tracking failed!" << endl;
						//return -1;
					}
					break;
			case 0: break;			// for exit
			default : cerr << "Invalid choice!" << endl;
		}
	} while (mode != 0);
	return 0;
}

int loadfile(FileRecord *records) {
	int tagID;
	string item;
	cout << "Loading mapping file..." << endl;
	ifstream mapping ("Mapping.txt");
	if (!mapping.is_open())
		return -1;
	while (1) {
		mapping >> tagID >> item;
		if (mapping.eof()) {
			break; 
		} else {
			records[FileRecord::numberOfRecords].tagID = tagID;
			records[FileRecord::numberOfRecords++].item = item;
			/*
			cout << "number of records: " << FileRecord::numberOfRecords << endl;
			cout << "TagID: " << records[FileRecord::numberOfRecords-1].tagID << endl;
			cout << "Item: " << records[FileRecord::numberOfRecords - 1].item << endl;
			*/
		}
	} 
	mapping.close();
	cout << "Loaded mapping file" << endl;
	return 0;
}

int identifyTag(int a, int b) {			//variable names to be corrected if possible
	int TagID = -1;
	if ((a == 3) && (b == 3))
	{
		TagID=1;
	}
	else if ((a==4)&&(b==4))
	{
		TagID=2;
	}
	else if((a==5)&&(b==5))
	{
		TagID=3;
	}
	else if((a==6)&&(b==6))
	{
		TagID=4;
	}
	else if((a==7)&&(b==7))
	{
		TagID=5;
	}
	else if((a==4)&&(b==3))
	{
		TagID=7;
	}
	else if((a==3)&&(b==4))
	{
		TagID=8;
	}
	else if((a==5)&&(b==3))
	{
		TagID=9;
	}
	else if((a==5)&&(b==4))
	{
		TagID=11;
	}
	else if((a==6)&&(b==3))
	{
		TagID=12;
	}
	else if((a==6)&&(b==4))
	{
		TagID=13;
	}
	return TagID;
}

int appendToFile(int tagID, string item) {
	ofstream mapping ("Mapping.txt", ofstream::app);
	if (!mapping.is_open())
		return -1;
	mapping << tagID << " " << item << endl;
	mapping.close();
	return 0;
}

Point2f getTagCenter(Tag &batTag) {
	Moments M = moments(batTag.contours[1]);
	return Point2f(M.m10/M.m00, M.m01/M.m00);
}

Point2f normalizeXY(Point2f tagCenter) {
	Point2f point;
	point.x = tagCenter.x - WIDTH / 2;
	point.y = tagCenter.y - HEIGHT / 2;
	return point;
}

float findDepth(float area) {
	float z = (area - AREA_SCALE_MIN) * SCALE_FACTOR + Z_SCALE_MIN;
	return z;
}

int getTagID(string item, int tagCount, FileRecord *records) {						
	for (int i = 0; i < tagCount; i++) {
		if (item.compare(records[i].item) == 0) {
			return records[i].tagID;
		}
	}
	return -1;
}