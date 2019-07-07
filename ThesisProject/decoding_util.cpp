#include "decoding_util.hpp"

DecodingUtil::DecodingUtil() {}

DecodingUtil::DecodingUtil(int modulesNumber)
{
	setModulesNumber(modulesNumber);
}

string DecodingUtil::decode(string bitStream)
{
	int length = getDecimalFromBinaryString(bitStream.substr(0, SHORT_BIT_NUM));
	if (length > 40) //for now it is hard coded
	{
		std::cout << "Decoding error! Length of the encoded message can not be " << length << endl;
		return "";
	}
	int currentPosition = SHORT_BIT_NUM;
	string decoded = "";
	for (int i = 0; i < length; i++)
	{
		if (i != length - 1) // if not last one
		{
			int pair = getDecimalFromBinaryString(bitStream.substr(currentPosition, BIT_NUM));
			int first = pair / 45;
			int second = pair % 45;
			//add here some check later so that it didn't throw error if first or second
			decoded += alphanumericValuesMapDecoding.find(first)->second;
			decoded += alphanumericValuesMapDecoding.find(second)->second;
			currentPosition += BIT_NUM;
		}
		else
		{
			int last = getDecimalFromBinaryString(bitStream.substr(currentPosition, SHORT_BIT_NUM));
			if (last > 0 && last < 45)
			{
				decoded += alphanumericValuesMapDecoding.find(last)->second;
			}
			else
			{
				std::cout << "Decoding error! Alphanumeric symbol code can not be " << last << endl;
			}
			currentPosition += SHORT_BIT_NUM;
		}
		i++;
	}
	
	vector<int> errorCorrectionCodeword;
	int errorCorrectionLength = BYTE_BIT_NUM * getErrorCorrector().getLengthOfCodeword();
	string errorCorrection = bitStream.substr(currentPosition, errorCorrectionLength);
	currentPosition = 0;
	for (int i = 0; i < getErrorCorrector().getLengthOfCodeword(); i++)
	{
		int value = getDecimalFromBinaryString(errorCorrection.substr(currentPosition, BYTE_BIT_NUM));
		errorCorrectionCodeword.push_back(value);
		currentPosition += BYTE_BIT_NUM;
	}

	/*if (!checkErrorCorrection(decoded, errorCorrectionCodeword))
	{
		std::cout << "Error correction check failed";
		return "";
	}*/
	
	return decoded;
}

bool DecodingUtil::checkErrorCorrection(string message, vector<int> codeword)
{
	return getErrorCorrector().decode(message, codeword);
}

int DecodingUtil::getPaletteIndex(std::vector<Vec3b> palette, Vec3b currentPixelHSV)
{
	int index = 0;
	double hsvMinDistance = std::numeric_limits<int>::max();
	int currentColorH = currentPixelHSV[0];
	int currentColorS = currentPixelHSV[1];
	int currentColorV = currentPixelHSV[2];
	for (int i = 0; i < palette.size(); i++)
	{
		double currentDistance = sqrt(pow(palette[i][0] - currentColorH, 2) + pow(palette[i][1] - currentColorS, 2) + pow(palette[i][2] - currentColorV, 2));
		if (currentDistance < hsvMinDistance)
		{
			hsvMinDistance = currentDistance;
			index = i;
		}
	}
	//here add check for the case when the minDiff is still too large to be classified correctly
	return index;
}

string DecodingUtil::getBitStreamFrom2DCode(Mat code)
{
	/*
	//increase saturation
	double saturation = 0;
	double scale = 2;
	// what it does here is dst = (uchar) ((double)src*scale+saturation); 
	Mat saturated;
	code.convertTo(saturated, CV_8UC1, scale, saturation);
	imwrite("increased_saturation.png", saturated);
	code = saturated;
	*/

	Mat codeHSV;
	cvtColor(code, codeHSV, COLOR_BGR2HSV);
	//imwrite("cohsv.png", codeHSV);

	int threshold = 10;
	int thresholdHSV = 10;
	std::vector<color> moduleColorSums(getModulesNumber());
	std::vector<int> moduleColorNums(getModulesNumber());

	std::vector<Vec3i> moduleHSVSums(getModulesNumber());
	std::vector<int> moduleHSVNums(getModulesNumber());

	//std::ofstream file;
	//file.open("values.txt");

	for (int r = 0; r < code.rows; r++)
	{
		for (int c = 0; c < code.cols; c++)
		{
			int index = r / moduleSideSize * codeSide + c / moduleSideSize;
			int currentB = code.at<cv::Vec3b>(r, c)[0];
			int currentG = code.at<cv::Vec3b>(r, c)[1];
			int currentR = code.at<cv::Vec3b>(r, c)[2];
			if (abs(currentB - currentG) > threshold || abs(currentB - currentR) > threshold || abs(currentR - currentG) > threshold)
			{
				moduleColorSums[index].b += currentB;
				moduleColorSums[index].g += currentG;
				moduleColorSums[index].r += currentR;
				moduleColorNums[index]++;
				//file << index << ": (" << currentB << ", " << currentG << ", " << currentR << ")" << endl;
			}

			//hsv version
			int currentH = codeHSV.at<cv::Vec3b>(r, c)[0];
			int currentS = codeHSV.at<cv::Vec3b>(r, c)[1];
			int currentV = codeHSV.at<cv::Vec3b>(r, c)[2];
			if (currentS > thresholdHSV)
			{
				moduleHSVSums[index][0] += currentH;
				moduleHSVSums[index][1] += currentS;
				moduleHSVSums[index][2] += currentV;
				moduleHSVNums[index]++;
				//file << index << "(" << r << "," << c << "): (" << currentH << ", " << currentS << ", " << currentV << ")" << endl;
			}
		}
	}
	//file.close();

	std::vector<color> moduleColorAverages(getModulesNumber());
	std::vector<Vec3b> moduleHSVAverages(getModulesNumber());
	for (int i = 0; i < getModulesNumber(); i++)
	{
		moduleColorAverages[i].b = moduleColorSums[i].b / moduleColorNums[i];
		moduleColorAverages[i].g = moduleColorSums[i].g / moduleColorNums[i];
		moduleColorAverages[i].r = moduleColorSums[i].r / moduleColorNums[i];
		//string b = to_string(moduleColorAverages[i].b);
		//string g = to_string(moduleColorAverages[i].g);
		//string r = to_string(moduleColorAverages[i].r);
		//cout << "(" + b + ", " + g + ", " + r + ")" << endl;

		//hsv version
		if (moduleHSVNums[i] > 0)
		{
			moduleHSVAverages[i][0] = moduleHSVSums[i][0] / moduleHSVNums[i];
			moduleHSVAverages[i][1] = moduleHSVSums[i][1] / moduleHSVNums[i];
			moduleHSVAverages[i][2] = moduleHSVSums[i][2] / moduleHSVNums[i];
		}
	}

	//for testing purposes
	/*
	Mat image = Mat::zeros(code.rows, code.cols, CV_8UC3);
	for (int r = 0; r < code.rows; r++)
	{
		for (int c = 0; c < code.cols; c++)
		{
			int index = r / moduleSideSize * codeSide + c / moduleSideSize;
			color pixelColor = moduleColorAverages[index];
			//cout << index << ": " << pixelColor.b << ", " << pixelColor.g << ", " << pixelColor.r << endl;
			image.at<cv::Vec3b>(r, c)[0] = pixelColor.b;
			image.at<cv::Vec3b>(r, c)[1] = pixelColor.g;
			image.at<cv::Vec3b>(r, c)[2] = pixelColor.r;
		}
		imwrite("average.png", image);
	}
	*/

	bool allPaletteInFirstLine = COLOR_NUMBER + 1 < codeSide;
	int lastPaletteModuleIndex = COLOR_NUMBER + (allPaletteInFirstLine ? 1 : 2);
	std::vector<color> paletteInCode;
	std::vector<Vec3b> paletteHSV;
	std::vector<int> paletteColorAmountInCode(COLOR_NUMBER);
	std::vector<int> colorIndexSequence; //palette index of all encoded modules
	string bitStream = "";
	for (int i = 0; i < getModulesNumber(); i++)
	{
		if (i == 0 || i == codeSide - 1 || i == getModulesNumber() - codeSide || i == getModulesNumber() - 1)
		{
			continue;
		}
		else if (i <= lastPaletteModuleIndex)
		{
			paletteInCode.push_back(moduleColorAverages[i]);
			paletteColorAmountInCode.push_back(1); //bc for now it is only palette color itself (of this color)
			paletteHSV.push_back(moduleHSVAverages[i]);

			//std::cout << "palette color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
		}
		else
		{
			color currentColor = moduleColorAverages[i];
			Vec3b currentPixelHSV = moduleHSVAverages[i];
			//std::cout << "current color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
			int paletteIndex = getPaletteIndex(paletteHSV, currentPixelHSV);
			//cout << paletteIndex + 1 << endl;
			//std::cout << "decided module color: " << paletteIndex << endl;
			colorIndexSequence.push_back(paletteIndex);
			bitStream.append(getBinaryAsString(paletteIndex).substr(BIT_NUM - BITS_IN_MODULE, BITS_IN_MODULE));
			
			//add current color to the average (experiment)
			paletteInCode[paletteIndex].b = (paletteInCode[paletteIndex].b * paletteColorAmountInCode[paletteIndex] + paletteInCode[paletteIndex].b) / (paletteColorAmountInCode[paletteIndex] + 1);
			paletteColorAmountInCode[paletteIndex]++;

			//update average of palette color (work more on this later)
			paletteInCode[paletteIndex].b = (paletteInCode[paletteIndex].b + currentColor.b) / 2;
			paletteInCode[paletteIndex].g = (paletteInCode[paletteIndex].g + currentColor.g) / 2;
			paletteInCode[paletteIndex].r = (paletteInCode[paletteIndex].r + currentColor.r) / 2;
		}
	}
	return bitStream;
}

Mat DecodingUtil::threasholdImage(Mat img)
{
	medianBlur(img, img, 5);

	double thres = 240;
	double color = 255;
	threshold(img, img, thres, color, THRESH_BINARY);

	// Execute erosion to improve the detection
	int erosion_size = 4;
	Mat element = getStructuringElement(MORPH_CROSS,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(img, img, element);
	return img;
}

Point2i* DecodingUtil::getAnglesFromImage(Mat image, Mat imageGray)
{
	Point2i inputQuad[4];
	vector<Vec2i> angles = {};

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;
	findContours(imageGray, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE); // Find the contours in the image

	vector<double> largestArea = { 0, 0, 0, 0 };
	vector<int> largestContourIndexes = { 0, 0, 0, 0 };
	for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		for (int j = 0; j < 4; j++)
		{
			if (a > largestArea[j]) {
				largestArea.insert(largestArea.begin() + j, a);
				//for inserting with shift of other elements, refactor later
				for (int k = largestContourIndexes.size() - 1; k > j; k--)
				{
					largestContourIndexes[k] = largestContourIndexes[k - 1];
				}
				largestContourIndexes[j] = i; //Store the index of largest contour
				break;
			}
		}
	}

	//x and y for each of 5 points (upper-left, upper-right, lower-right, lower-left, centroid) for each of 4 largest conrours
	vector<vector<Vec2i>> countoursImportantPointsCoordinates = {}; //final size should be 4
	Point2i generalUpperLeft = Point2i(image.cols, image.rows);
	Point2i generalUpperRight = Point2i(0, image.rows);
	Point2i generalLowerRight = Point2i(0, 0);
	Point2i generalLowerLeft = Point2i(image.cols, 0);
	for (int i = 0; i < largestContourIndexes.size(); i++)
	{
		int maxX = 0;
		int maxY = 0;
		int minX = image.cols;
		int minY = image.rows;

		for (int j = 0; j < contours[largestContourIndexes[i]].size(); j++)
		{
			int x = contours[largestContourIndexes[i]][j].x;
			int y = contours[largestContourIndexes[i]][j].y;
			if (x > maxX)
			{
				maxX = x;
			}
			if (x < minX)
			{
				minX = x;
			}
			if (y > maxY)
			{
				maxY = y;
			}
			if (y < minY)
			{
				minY = y;
			}
		}
		cout << i << "(" << largestContourIndexes[i] << "): minX = " << minX << ", maxX = " << maxX << ", minY = " << minY << ", maxY = " << maxY << endl;
		Vec2i upperLeft = Vec2i(minX, minY);
		Vec2i upperRight = Vec2i(maxX, minY);
		Vec2i lowerRight = Vec2i(maxX, maxY);
		Vec2i lowerLeft = Vec2i(minX, maxY);
		Vec2i centroid = Vec2i((minX + maxX) / 2, (minY + maxY) / 2);
		countoursImportantPointsCoordinates.push_back({ upperLeft, upperRight, lowerRight, lowerLeft, centroid });

		if (pow(upperLeft[0] - 0, 2) + pow(upperLeft[1] - 0, 2) < pow(generalUpperLeft.x - 0, 2) + pow(generalUpperLeft.y - 0, 2))
		{
			generalUpperLeft.x = upperLeft[0];
			generalUpperLeft.y = upperLeft[1];
		}
		if (pow(upperRight[0] - image.cols - 1, 2) + pow(upperRight[1] - 0, 2) < pow(generalUpperRight.x - image.cols - 1, 2) + pow(generalUpperRight.y - 0, 2))
		{
			generalUpperRight.x = upperRight[0];
			generalUpperRight.y = upperRight[1];
		}
		if (pow(lowerRight[0] - image.cols - 1, 2) + pow(lowerRight[1] - image.rows - 1, 2) < pow(generalLowerRight.x - image.cols - 1, 2) + pow(generalLowerRight.y - image.rows - 1, 2))
		{
			generalLowerRight.x = lowerRight[0];
			generalLowerRight.y = lowerRight[1];
		}
		if (pow(lowerLeft[0] - 0, 2) + pow(lowerLeft[1] - image.rows - 1, 2) < pow(generalLowerLeft.x - 0, 2) + pow(generalLowerLeft.y - image.rows - 1, 2))
		{
			generalLowerLeft.x = lowerLeft[0];
			generalLowerLeft.y = lowerLeft[1];
		}

		//for testing purposes
		/*
		inputQuad[0] = Point2f(generalUpperLeft.x, generalUpperLeft.y);
		inputQuad[1] = Point2f(generalUpperRight.x, generalUpperRight.y);
		inputQuad[2] = Point2f(generalLowerRight.x, generalLowerRight.y);
		inputQuad[3] = Point2f(generalUpperLeft.x, generalLowerLeft.y);
		*/
		for (int i = 0; i < 4; i++)
		{
			circle(image, inputQuad[i], 10, Scalar(0, 0, 255), 1, 8, 0);
		}
		imshow("points", image);
		cout << "Press any key" << endl;
		waitKey(0);
	}

	//for now i'll just assign it but i'll have to rewrite it eventually
	inputQuad[0] = Point2i(generalUpperLeft.x, generalUpperLeft.y);
	inputQuad[1] = Point2i(generalUpperRight.x, generalUpperRight.y);
	inputQuad[2] = Point2i(generalLowerRight.x, generalLowerRight.y);
	inputQuad[3] = Point2i(generalUpperLeft.x, generalLowerLeft.y);

	return inputQuad;
}

Mat DecodingUtil::createMat(int8_t* image) {
	int rows = 100;
	int cols = 100;
	cv::Mat mat(rows, cols, CV_8UC3, image);
	return mat;
}

//perspective transformation and resizing to the square
Mat DecodingUtil::perspectiveTransform(Mat input, Point2f* inputQuad)
{
	Mat lambda(2, 4, CV_32FC1);
	Mat output = Mat::zeros(codeSideSize, codeSideSize, input.type());
	Point2f outputQuad[4];
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f(output.cols - 1, 0);
	outputQuad[2] = Point2f(output.cols - 1, output.rows - 1);
	outputQuad[3] = Point2f(0, output.rows - 1);
	lambda = Mat::zeros(input.rows, input.cols, input.type());
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	warpPerspective(input, output, lambda, output.size());
	return output;
}

std::string DecodingUtil::decodeMessageFromImage(int8_t* image) {
	//gray scaling and thresholding for further angles search
	cv::Mat imageMat = createMat(image);
	cv::Mat grayMat;
	cv::cvtColor(imageMat, grayMat, COLOR_BGR2GRAY);
	grayMat = threasholdImage(grayMat);

	Point2i* inputQuad = getAnglesFromImage(imageMat, grayMat);

	vector<int32_t> xInputQuad = { inputQuad[0].x, inputQuad[1].x, inputQuad[2].x, inputQuad[3].x };
	vector<int32_t> yInputQuad = { inputQuad[0].y, inputQuad[1].y, inputQuad[2].y, inputQuad[3].y };

	return decodeMessageFromImage(image, xInputQuad, yInputQuad);
}

std::string DecodingUtil::decodeMessageFromImage(int8_t* image, vector<int32_t> xInputQuad, vector<int32_t> yInputQuad) {
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(xInputQuad[0], xInputQuad[0]);
	inputQuad[1] = Point2f(xInputQuad[1], xInputQuad[1]);
	inputQuad[2] = Point2f(xInputQuad[2], xInputQuad[2]);
	inputQuad[3] = Point2f(xInputQuad[3], xInputQuad[3]);

	cv::Mat imageMat = createMat(image);
	cv::Mat transformed = perspectiveTransform(imageMat, inputQuad);
	std::string bitStream = getBitStreamFrom2DCode(transformed);
	std::string decoded = decode(bitStream);
	return decoded;
}