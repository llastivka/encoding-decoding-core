#include "coder.hpp"

Coder::color Coder::palette[COLOR_NUMBER] = {
	{ 255, 0, 0 }, //red
	{ 0, 255, 0 }, //green
	{ 0, 0, 255 }, //blue
	{ 240, 50, 230 }, //magenta
	{ 70, 240, 240 }, //cyan
	{ 255, 225, 25 }, //yellow
	{ 145, 30, 180 }, //purple
	{ 245, 130, 49 } //orange
};

const Coder::color Coder::WHITE = { 255, 255, 255 };

Coder::Coder() {}

Coder::Coder(int modulesNumber)
{
	this->modulesNumber = modulesNumber;
	codeSide = sqrt(modulesNumber); //amount of modules in code side
	codeSideSize = codeSide * moduleSideSize; //total amount of pixels in code side
}

void Coder::setModulesNumber(int modulesNumber)
{
	this->modulesNumber = modulesNumber;
	codeSide = sqrt(modulesNumber); //amount of modules in code side
	codeSideSize = codeSide * moduleSideSize; //total amount of pixels in code side
}

int Coder::getModulesNumber()
{
	return this->modulesNumber;
}

void Coder::setErrorCorrector(ErrorCorrector errorCorrector)
{
	Coder::errorCorrector = errorCorrector;
}

ErrorCorrector Coder::getErrorCorrector()
{
	return Coder::errorCorrector;
}

string Coder::getBinaryAsString(int value)
{
	string binary = std::bitset<BIT_NUM>(value).to_string();
	return binary;
}

int Coder::getDecimalFromBinaryString(string value)
{
	int dec = 0, mask = 1;

	for (int i = value.length() - 1; i >= 0; i--) {
		if (value[i] == '1') {
			dec |= mask;
		}
		mask <<= 1;
	}

	return dec;
}






//decoding methods
string Coder::decode(string bitStream)
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
			decoded += decodingAlphanumericValuesMap.find(first)->second;
			decoded += decodingAlphanumericValuesMap.find(second)->second;
			currentPosition += BIT_NUM;
		}
		else
		{
			int last = getDecimalFromBinaryString(bitStream.substr(currentPosition, SHORT_BIT_NUM));
			if (last > 0 && last < 45)
			{
				decoded += decodingAlphanumericValuesMap.find(last)->second;
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

bool Coder::checkErrorCorrection(string message, vector<int> codeword)
{
	return getErrorCorrector().decode(message, codeword);
}

int Coder::getPaletteIndex(std::vector<Coder::color> palette, std::vector<vector<double>> paletteBgrProportions, Coder::color currentColor)
{
	int index = 0;
	int bgrMinDiff = 256;
	vector<double> bgrProportions(3); //proportions between bgr values
	bgrProportions[0] = std::min(currentColor.b, currentColor.g) / std::max(currentColor.b, currentColor.g);
	bgrProportions[1] = std::min(currentColor.g, currentColor.r) / std::max(currentColor.g, currentColor.r);
	bgrProportions[2] = std::min(currentColor.b, currentColor.r) / std::max(currentColor.b, currentColor.r);
	double proportionThreshold = 0.1;
	//variable for test purposes
	bool wasNotChanged = true;
	for (int i = 0; i < palette.size(); i++)
	{
		//if (abs(paletteBgrProportions[i][0] - bgrProportions[0]) < proportionThreshold && abs(paletteBgrProportions[i][1] - bgrProportions[1]) < proportionThreshold &&
		//	abs(paletteBgrProportions[i][2] - bgrProportions[2]) < proportionThreshold) {
		double averageDiff = (abs(palette[i].b - currentColor.b) + abs(palette[i].g - currentColor.g) + abs(palette[i].r - currentColor.r)) / 3;
		if (averageDiff < bgrMinDiff)
		{
			bgrMinDiff = averageDiff;
			index = i;
			wasNotChanged = false;
		}
		//}
	}

	if (wasNotChanged)
	{
		int i = 5;
		int y = i + 1;
	}
	//here add check for the case when the minDiff is still too large to be classified correctly
	return index;
}

int Coder::getPaletteIndex(std::vector<Vec3b> palette, Vec3b currentPixelHSV)
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

string Coder::getBitStreamFrom2DCode(Mat code)
{
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
		else if (i < lastPaletteModuleIndex)
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

//perspective transformation and resizing to the square
Mat Coder::perspectiveTransform(Mat input, Point2f* inputQuad)
{
	/*
	//increase saturation
	double saturation = 0;
	double scale = 2;
	// what it does here is dst = (uchar) ((double)src*scale+saturation);
	Mat saturated;
	input.convertTo(saturated, CV_8UC1, scale, saturation);
	imwrite("increased_saturation.png", saturated);
	input = saturated;
	*/

	//gray scaling and thresholding for further angles search
	cv::Mat grayMat;
	cv::cvtColor(input, grayMat, COLOR_BGR2GRAY);
	imwrite("step1.png", grayMat);
	grayMat = threasholdImage(grayMat);

	inputQuad = getAnglesFromImage(input, grayMat);

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

Mat Coder::threasholdImage(Mat img)
{
	medianBlur(img, img, 5);
	imwrite("step2.png", img);

	double thres = 240;
	double color = 255;
	threshold(img, img, thres, color, THRESH_BINARY);
	imwrite("step3.png", img);

	// Execute erosion to improve the detection
	int erosion_size = 4;
	Mat element = getStructuringElement(MORPH_CROSS,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(img, img, element);
	imwrite("step4.png", img);
	return img;
}

Point2f* Coder::getAnglesFromImage(Mat image, Mat imageGray)
{
	Point2f inputQuad[4];
	vector<Vec2i> angles = {};

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;
	imshow("imageGray", imageGray);
	imwrite("imageGray.png", imageGray);
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
	vector<vector<Vec2i>> countoursImportantPointsCoordinates = {}; //final size should be 4. For now it is not really used, maybe remove later

	Vec2i originUpperLeft = Vec2i(0, 0);
	Vec2i originUpperRight = Vec2i(image.cols - 1, 0);
	Vec2i originLowerRight = Vec2i(image.cols - 1, image.rows - 1);
	Vec2i originLowerLeft = Vec2i(0, image.rows - 1);

	//here are the opposite values in order to at least one of the real values could be set
	Point2f generalUpperLeft = Point2f(image.cols - 1, image.rows - 1);
	Point2f generalUpperRight = Point2f(0, image.rows - 1);
	Point2f generalLowerRight = Point2f(0, 0);
	Point2f generalLowerLeft = Point2f(image.cols - 1, 0);
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
			//cout << "x: " << x << "     y: " << y << endl;
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
		Vec2i centroid = Vec2i((minX + maxX) / 2, (minY + maxY) / 2); //not used for now, maybe delete later
		countoursImportantPointsCoordinates.push_back({ upperLeft, upperRight, lowerRight, lowerLeft, centroid });
		
		if (pow(upperLeft[0] - originUpperLeft[0], 2) + pow(upperLeft[1] - originUpperLeft[1], 2) < pow(generalUpperLeft.x - originUpperLeft[0], 2) + pow(generalUpperLeft.y - originUpperLeft[1], 2))
		{
			generalUpperLeft.x = upperLeft[0];
			generalUpperLeft.y = upperLeft[1];
		}
		if (pow(upperRight[0] - originUpperRight[0], 2) + pow(upperRight[1] - originUpperRight[1], 2) < pow(generalUpperRight.x - originUpperRight[0], 2) + pow(generalUpperRight.y - originUpperRight[1], 2))
		{
			generalUpperRight.x = upperRight[0];
			generalUpperRight.y = upperRight[1];
		}
		if (pow(lowerRight[0] - originLowerRight[0], 2) + pow(lowerRight[1] - originLowerRight[1], 2) < pow(generalLowerRight.x - originLowerRight[0], 2) + pow(generalLowerRight.y - originLowerRight[1], 2))
		{
			generalLowerRight.x = lowerRight[0];
			generalLowerRight.y = lowerRight[1];
		}
		if (pow(lowerLeft[0] - originLowerLeft[0], 2) + pow(lowerLeft[1] - originLowerLeft[1], 2) < pow(generalLowerLeft.x - originLowerLeft[0], 2) + pow(generalLowerLeft.y - originLowerLeft[1], 2))
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
	}

	//for now i'll just assign it but i'll have to rewrite it eventually
	inputQuad[0] = Point2f(generalUpperLeft.x, generalUpperLeft.y);
	inputQuad[1] = Point2f(generalUpperRight.x, generalUpperRight.y);
	inputQuad[2] = Point2f(generalLowerRight.x, generalLowerRight.y);
	inputQuad[3] = Point2f(generalLowerLeft.x, generalLowerLeft.y);

	for (int i = 0; i < 4; i++)
	{
		circle(image, inputQuad[i], 5, Scalar(0, 0, 255), 3, 8, 0);
	}
	imshow("points", image);
	cout << "Press any key" << endl;
	waitKey(0);

	return inputQuad;
}

Mat Coder::createMat(int8_t* image, int32_t rows, int32_t cols) {
	cv::Mat mat(rows, cols, CV_8UC3, image);
	return mat;
}

std::string Coder::decodeMessageFromImage(int8_t* image, int32_t rows, int32_t cols, vector<int32_t> xInputQuad, vector<int32_t> yInputQuad) {
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(xInputQuad[0], yInputQuad[0]);
	inputQuad[1] = Point2f(xInputQuad[1], yInputQuad[1]);
	inputQuad[2] = Point2f(xInputQuad[2], yInputQuad[2]);
	inputQuad[3] = Point2f(xInputQuad[3], yInputQuad[3]);

	//int8_t* imageArr = &image[0];

	cv::Mat imageMat;
	try {
		imageMat = createMat(image, rows, cols);
	}
	catch (exception& e) {
		cout << e.what() << '\n';
		throw;
	}

	cv::Mat transformed = perspectiveTransform(imageMat, inputQuad);
	std::string bitStream = getBitStreamFrom2DCode(transformed);
	std::string decoded = decode(bitStream);
	//return decoded;
	return "result";
}








//encoding methods
string Coder::encode(string text)
{
	string encoded = getBinaryAsString(text.size()).substr(BIT_NUM - SHORT_BIT_NUM, SHORT_BIT_NUM); //only 6 bits needed bc max number of encoded bits is 35
	for (int i = 0; i < text.size(); i++)
	{
		if (i != text.size() - 1)
		{
			int pairFirst = encodingAlphanumericValuesMap.find(text[i])->second;
			cout << "pairFirst: " << pairFirst << endl;
			int pairSecond = encodingAlphanumericValuesMap.find(text[++i])->second;
			cout << "pairSecond: " << pairSecond << endl;
			encoded.append(getBinaryAsString(45 * pairFirst + pairSecond));
			cout << "encoded together: " << getBinaryAsString(45 * pairFirst + pairSecond) << endl;
		}
		else
		{
			int last = encodingAlphanumericValuesMap.find(text[i])->second;
			string binary = getBinaryAsString(last);
			cout << "last: " << last << " - " << getBinaryAsString(last) << endl;
			encoded.append(binary.substr(BIT_NUM - SHORT_BIT_NUM, SHORT_BIT_NUM));
		}
	}

	string errorCorrectionEncoded = encodeErrorCorrectionCodeword(getErrorCorrector().encode(text));
	int neededLength = (getModulesNumber() - PALETTE_NUM - ANGLES_NUM) * BITS_IN_MODULE;
	do {
		if (neededLength - encoded.size() >= errorCorrectionEncoded.size())
		{
			encoded.append(errorCorrectionEncoded);
		}
		else
		{
			int leftoverSpaceLength = errorCorrectionEncoded.size() - (neededLength - encoded.size());
			encoded.append(errorCorrectionEncoded.substr(0, leftoverSpaceLength));
		}
	} while (encoded.size() < neededLength);
	return encoded;
}

string Coder::encodeErrorCorrectionCodeword(vector<int> codeword)
{
	string encodedErrorCorrection = "";
	for (int value : codeword)
	{
		encodedErrorCorrection.append(getBinaryAsString(value).substr(BIT_NUM - BYTE_BIT_NUM, BYTE_BIT_NUM));
		cout << "code correction: " << getBinaryAsString(value).substr(BIT_NUM - BYTE_BIT_NUM, BYTE_BIT_NUM) << endl;
	}
	return encodedErrorCorrection;
}

Mat Coder::create2DCode(string bitString)
{
	//defining color sequence
	int codeSide = sqrt(getModulesNumber());
	vector<color> colorSequence;
	bool allPaletteInFirstLine = COLOR_NUMBER + 1 < codeSide;
	int counter = 0;
	for (int i = 0; i < getModulesNumber(); i++)
	{
		if (i == 0 || i == codeSide - 1 || i == getModulesNumber() - codeSide || i == getModulesNumber() - 1)
		{
			colorSequence.push_back(WHITE);
		}
		else if (i < COLOR_NUMBER + (allPaletteInFirstLine ? 1 : 2))
		{
			int index;
			if (i < codeSide)
			{
				index = i - 1;
			}
			else
			{
				index = i - 2;
			}
			colorSequence.push_back(palette[index]);
		}
		else
		{
			int position = BITS_IN_MODULE * counter;
			string bitSet = bitString.substr(position, BITS_IN_MODULE);
			colorSequence.push_back(palette[getDecimalFromBinaryString(bitSet)]);
			counter++;
		}
	}

	int rows = codeSide * moduleSideSize;
	int cols = codeSide * moduleSideSize;
	Mat image = Mat::zeros(rows, cols, CV_8UC3);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int index = r / moduleSideSize * codeSide + c / moduleSideSize;
			color pixelColor = colorSequence[index];
			//cout << index << ": " << pixelColor.b << ", " << pixelColor.g << ", " << pixelColor.r << endl;
			image.at<cv::Vec3b>(r, c)[0] = pixelColor.b;
			image.at<cv::Vec3b>(r, c)[1] = pixelColor.g;
			image.at<cv::Vec3b>(r, c)[2] = pixelColor.r;
		}
		imshow("encoded", image);
	}
	return image;
}