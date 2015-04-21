int trackTag(Tag &identifiedTag, int tagID) {
	Mat frame;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int contourLevel, tagLevel[3], tagLevelIndex;
	float ratio[3];
	int previousPolygonVertexCount[2], match;
	for (;;) {
		capture >> frame;
		Canny(frame, frame, MIN_THRESHOLD, MAX_THRESHOLD);
		findContours(frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for(int i = 0; i < hierarchy.size(); i++) {
			if (hierarchy[i][2] != -1) {
				if (hierarchy[i][3] == -1) {
					contourLevel = 0;
					tagLevelIndex = 0; 
					tagLevel[tagLevelIndex] = i;
				} else {
					contourLevel++;
					tagLevelIndex = contourLevel % 3;
					tagLevel[tagLevelIndex] = i;
					
					if (contourLevel >= 2) {
						identifiedTag.area[0] = contourArea(contours[tagLevel[(tagLevelIndex + 1) % 3]]);
						identifiedTag.area[1] = contourArea(contours[tagLevel[(tagLevelIndex + 2) % 3]]);
						identifiedTag.area[2] = contourArea(contours[tagLevel[(tagLevelIndex + 3) % 3]]);
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
									if (tagID == identifiedTag.tagID) { 
										cout << "Tag ID: " << identifiedTag.tagID << endl;
										tagPosition = getTagCenter(identifiedTag);
										normalizedXY = normalizeXY(tagPosition);
										depth = findDepth(identifiedTag.area[1]);
										//cout << normalizedXY.x << " " << normalizedXY.y << " " << depth << endl;
										if (generateBeep(normalizedXY.x, normalizedXY.y, depth) == -1) {
											cerr << "Error opening file!" << endl;
										}
										return 0;
									}
								}
							} else {
								match = 0;
								previousPolygonVertexCount[0] = identifiedTag.numberOfVertices[1];
								previousPolygonVertexCount[1] = identifiedTag.numberOfVertices[2];
							}
						}
					}
				}
				i++;
			}
		}
	}
}