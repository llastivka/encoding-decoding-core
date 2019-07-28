#include "coder.hpp"

struct Coder::color
{
	float r, g, b;
};

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

const std::string Coder::HTTP_PROTOCOL = "http://";
const std::string Coder::HTTPS_PROTOCOL = "https://";
const std::string Coder::DEFAULT_URL_SHORTENER = "u.nu/";
const std::string Coder::DEFAULT_LINK_MARKER = ":";

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
/*
void Coder::setErrorCorrector(ErrorCorrector errorCorrector)
{
	Coder::errorCorrector = errorCorrector;
}

ErrorCorrector Coder::getErrorCorrector()
{
	return Coder::errorCorrector;
}
*/
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

std::string toLowerCase(std::string text)
{
	for (int i = 0; text[i] != 0; i++)
	{
		if (text[i] <= 'Z' && text[i] >= 'A')
		{
			text[i] += 32;
		}
	}
	return text;
}

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
				return "ERROR";
			}
			currentPosition += SHORT_BIT_NUM;
		}
		i++;
	}
	
	/*
	vector<int> errorCorrectionCodeword;
	string errorCorrection = bitStream.substr(currentPosition, length);
	currentPosition = 0;
	for (int i = 0; i < getErrorCorrector().getLengthOfCodeword(); i++)
	{
		int value = getDecimalFromBinaryString(errorCorrection.substr(currentPosition, BYTE_BIT_NUM));
		errorCorrectionCodeword.push_back(value);
		currentPosition += BYTE_BIT_NUM;
	}
	*/

	if (isLink(decoded)) {
		decoded = decoded.substr(1, length);
		decoded = HTTPS_PROTOCOL + DEFAULT_URL_SHORTENER + decoded;
		decoded = toLowerCase(decoded);
	}

	return decoded;
}

bool Coder::isLink(string decoded)
{
	return decoded.find(Coder::DEFAULT_LINK_MARKER) != std::string::npos;
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

int Coder::getPaletteIndex(std::vector<cv::Vec3b> palette, cv::Vec3b currentPixelHSV)
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

string Coder::getBitStreamFrom2DCode(cv::Mat code)
{
	cv::Mat codeHSV;
	cvtColor(code, codeHSV, cv::COLOR_BGR2HSV);
	//imwrite("cohsv.png", codeHSV);

	int threshold = 10;
	int thresholdHSV = 10;
	std::vector<color> moduleColorSums(getModulesNumber());
	std::vector<int> moduleColorNums(getModulesNumber());

	std::vector<cv::Vec3i> moduleHSVSums(getModulesNumber());
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
	std::vector<cv::Vec3b> moduleHSVAverages(getModulesNumber());
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
	cv::Mat image = cv::Mat::zeros(code.rows, code.cols, CV_8UC3);
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
		cv::imwrite("average.png", image);
	}
	*/

	bool allPaletteInFirstLine = COLOR_NUMBER + 1 < codeSide;
	int lastPaletteModuleIndex = COLOR_NUMBER + (allPaletteInFirstLine ? 1 : 2);
	std::vector<color> paletteInCode;
	std::vector<cv::Vec3b> paletteHSV;
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
			cv::Vec3b currentPixelHSV = moduleHSVAverages[i];
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
cv::Mat Coder::perspectiveTransform(cv::Mat input, vector<cv::Point2f> inputQuad)
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
//	cv::Mat grayMat;
//	cv::cvtColor(input, grayMat, cv::COLOR_BGR2GRAY);
//	grayMat = threasholdImage(grayMat);
//
//	inputQuad = getAnglesFromImage(input, grayMat);

	cv::Mat lambda(2, 4, CV_32FC1);
	cv::Mat output = cv::Mat::zeros(codeSideSize, codeSideSize, input.type());
	cv::Point2f outputQuad[4];
    cv::Point2f inputQuad1[4];
	outputQuad[0] = cv::Point2f(0, 0);
	outputQuad[1] = cv::Point2f(output.cols - 1, 0);
	outputQuad[2] = cv::Point2f(output.cols - 1, output.rows - 1);
	outputQuad[3] = cv::Point2f(0, output.rows - 1);

    inputQuad1[0] = inputQuad.at(0);
    inputQuad1[1] = inputQuad.at(1);
    inputQuad1[2] = inputQuad.at(2);
    inputQuad1[3] = inputQuad.at(3);

	lambda = cv::Mat::zeros(input.rows, input.cols, input.type());
	lambda = getPerspectiveTransform(inputQuad1, outputQuad);
	warpPerspective(input, output, lambda, output.size());
	return output;
}

cv::Mat makeWhiteWhiter(cv::Mat image) {
	cv::Mat imgHSL;
	cv::cvtColor(image, imgHSL, cv::COLOR_BGR2HLS);
	//vector<cv::Mat> channels;
	//split(imgHLS, channels);

	int thresholdBright = 100;
	int thresholdMono = 10;
	int thresholdL = 60;
	int thresholdS = 60;
	for (int r = 0; r < image.rows; r++)
	{
		for (int c = 0; c < image.cols; c++)
		{
			int currentB = image.at<uint8_t>(r, c);
			int currentG = image.at<cv::Vec3b>(r, c)[1];
			int currentR = image.at<cv::Vec3b>(r, c)[2];

			int currentH = imgHSL.at<cv::Vec3b>(r, c)[0];
			int currentL = imgHSL.at<cv::Vec3b>(r, c)[1];
			int currentS = imgHSL.at<cv::Vec3b>(r, c)[2];

			if (currentL > thresholdL && currentS < thresholdS)
			{
				image.at<cv::Vec3b>(r, c)[0] = 255;
				image.at<cv::Vec3b>(r, c)[1] = 255;
				image.at<cv::Vec3b>(r, c)[2] = 255;
				//imageGray.at<uint8_t>(r, c) = 255;
			}

//			bool bright = currentB > thresholdBright && currentG > thresholdBright && currentR > thresholdBright;
//			bool mono = abs(currentB - currentG) < thresholdMono && abs(currentB - currentR) < thresholdMono && abs(currentR - currentG) < thresholdMono;
//			if (bright && mono)
//			{
//				image.at<cv::Vec3b>(r, c)[0] = 255;
//				image.at<cv::Vec3b>(r, c)[1] = 255;
//				image.at<cv::Vec3b>(r, c)[2] = 255;
//			}
		}
	}
	return image;
}

cv::Mat Coder::perspectiveTransform(cv::Mat input)
{
	//input = makeWhiteWhiter(input);
	//cv::imwrite("anglesFromImage_Before.png", input);
	input = makeWhiteWhiter(input);
	
	//gray scaling and thresholding for further angles search
	cv::Mat grayMat;
	cv::cvtColor(input, grayMat, cv::COLOR_BGR2GRAY);
	grayMat = threasholdImage(grayMat);

	//cv::imwrite("anglesFromImage_AfterColor.png", input);
	//cv::imwrite("anglesFromImage_AfterGray.png", grayMat);

    vector<cv::Point2f> inputQuad = getAnglesFromImage(input, grayMat);
	//cv::imwrite("circled.png", input);
	return perspectiveTransform(input, inputQuad);
}

cv::Mat Coder::threasholdImage(cv::Mat img)
{
	medianBlur(img, img, 5);

	double thres = 240;
	double color = 255;
	//threshold(img, img, thres, color, cv::THRESH_BINARY);
	threshold(img, img, thres, color, cv::THRESH_BINARY);
	//cv::adaptiveThreshold(img, img, color, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 2);

	// Execute erosion to improve the detection
	int erosion_size = 2;
	cv::Mat element = getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));
	//erode(img, img, element);
	return img;
}

vector<cv::Point2f> Coder::getAnglesFromImage(cv::Mat image, cv::Mat imageGray)
{

    vector<cv::Point2f> inputQuad;
	vector<cv::Vec2i> angles = {};

	vector<vector<cv::Point>> contours; // Vector for storing contour
	vector<cv::Vec4i> hierarchy;
	findContours(imageGray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); // Find the contours in the image

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
	vector<vector<cv::Vec2i>> countoursImportantPointsCoordinates = {}; //final size should be 4. For now it is not really used, maybe remove later

	cv::Vec2i originUpperLeft = cv::Vec2i(0, 0);
	cv::Vec2i originUpperRight = cv::Vec2i(image.cols - 1, 0);
	cv::Vec2i originLowerRight = cv::Vec2i(image.cols - 1, image.rows - 1);
	cv::Vec2i originLowerLeft = cv::Vec2i(0, image.rows - 1);

	//here are the opposite values in order to at least one of the real values could be set
	cv::Point2f generalUpperLeft = cv::Point2f(image.cols - 1, image.rows - 1);
	cv::Point2f generalUpperRight = cv::Point2f(0, image.rows - 1);
	cv::Point2f generalLowerRight = cv::Point2f(0, 0);
	cv::Point2f generalLowerLeft = cv::Point2f(image.cols - 1, 0);
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
		//cout << i << "(" << largestContourIndexes[i] << "): minX = " << minX << ", maxX = " << maxX << ", minY = " << minY << ", maxY = " << maxY << endl;
		cv::Vec2i upperLeft = cv::Vec2i(minX, minY);
		cv::Vec2i upperRight = cv::Vec2i(maxX, minY);
		cv::Vec2i lowerRight = cv::Vec2i(maxX, maxY);
		cv::Vec2i lowerLeft = cv::Vec2i(minX, maxY);
		cv::Vec2i centroid = cv::Vec2i((minX + maxX) / 2, (minY + maxY) / 2); //not used for now, maybe delete later
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
	inputQuad.push_back(cv::Point2f(generalUpperLeft.x, generalUpperLeft.y));
	inputQuad.push_back(cv::Point2f(generalUpperRight.x, generalUpperRight.y));
	inputQuad.push_back(cv::Point2f(generalLowerRight.x, generalLowerRight.y));
	inputQuad.push_back(cv::Point2f(generalLowerLeft.x, generalLowerLeft.y));

	cv::Point2f inputQuad1[4];
	inputQuad1[0] = cv::Point2f(generalUpperLeft.x, generalUpperLeft.y);
	inputQuad1[1] = cv::Point2f(generalUpperRight.x, generalUpperRight.y);
	inputQuad1[2] = cv::Point2f(generalLowerRight.x, generalLowerRight.y);
	inputQuad1[3] = cv::Point2f(generalLowerLeft.x, generalLowerLeft.y);

	for (int i = 0; i < 4; i++)
	{
		circle(image, inputQuad1[i], 5, cv::Scalar(0, 0, 255), 3, 8, 0);
	}

	return inputQuad;
}

cv::Mat Coder::createMat(int8_t* image, int32_t rows, int32_t cols) {
	cv::Mat mat(rows, cols, CV_8UC3, image);
	return mat;
}

std::string Coder::decodeMessageFromImage(int8_t* image, int32_t rows, int32_t cols, vector<int32_t> xInputQuad, vector<int32_t> yInputQuad) {
//	cv::Point2f inputQuad[4];
//	inputQuad[0] = cv::Point2f(xInputQuad[0], yInputQuad[0]);
//	inputQuad[1] = cv::Point2f(xInputQuad[1], yInputQuad[1]);
//	inputQuad[2] = cv::Point2f(xInputQuad[2], yInputQuad[2]);
//	inputQuad[3] = cv::Point2f(xInputQuad[3], yInputQuad[3]);
//
//	//int8_t* imageArr = &image[0];
//
//	cv::Mat imageMat;
//	try {
//		imageMat = createMat(image, rows, cols);
//	}
//	catch (exception& e) {
//		cout << e.what() << '\n';
//		throw;
//	}
//
//	cv::Mat transformed = perspectiveTransform(imageMat, inputQuad);
//	std::string bitStream = getBitStreamFrom2DCode(transformed);
//	std::string decoded = decode(bitStream);
//	//return decoded;
	return "result";
}

std::string Coder::decodeStringFromMat(cv::Mat mat)
{
	cv::Mat transformed = perspectiveTransform(mat);
	if (transformed.empty()) {
		return "ERROR";
	}
	cv::imwrite("transformed.png", transformed);
	string bitStream = getBitStreamFrom2DCode(transformed);
	cout << bitStream << endl;
	string decoded = decode(bitStream);
	return decoded;
}













//encoding methods

bool Coder::isURL(string text) {
	return text.find(Coder::HTTP_PROTOCOL) != std::string::npos || text.find(Coder::HTTPS_PROTOCOL) != std::string::npos || text.find(Coder::DEFAULT_URL_SHORTENER) != std::string::npos;
}

std::string toUpperCase(std::string text)
{
	for (int i = 0; text[i] != 0; i++)
	{
		if (text[i] <= 'z' && text[i] >= 'a')
		{
			text[i] -= 32;
		}
	}
	return text;
}

std::string getStringInReversedOrder(std::string str) {
	std::string reversed = "";
	for (int i = str.length() - 1; i >= 0; --i) {
		reversed += str[i];
	}
	return reversed;
}

std::string Coder::encode(std::string text)
{
	if (isURL(text)) {
		std:string delimiter = "/";
		size_t pos = 0;
		while ((pos = text.find(delimiter)) != std::string::npos) {
			text.erase(0, pos + delimiter.length());
		}
		text = toUpperCase(text);
		text = DEFAULT_LINK_MARKER + text;
	}

	string encoded = getBinaryAsString(text.size()).substr(BIT_NUM - SHORT_BIT_NUM, SHORT_BIT_NUM); //only 6 bits needed bc max number of encoded bits is 35
	string encodedMessageOnly = "";
	for (int i = 0; i < text.size(); i++)
	{
		if (i != text.size() - 1)
		{
			int pairFirst = encodingAlphanumericValuesMap.find(text[i])->second;
			cout << "pairFirst: " << pairFirst << endl;
			int pairSecond = encodingAlphanumericValuesMap.find(text[++i])->second;
			cout << "pairSecond: " << pairSecond << endl;
			string encodedPair = getBinaryAsString(45 * pairFirst + pairSecond);
			encoded.append(encodedPair);
			encodedMessageOnly.append(encodedPair);
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

	//error correction (kinda)
	int neededLength = (getModulesNumber() - PALETTE_NUM - ANGLES_NUM) * BITS_IN_MODULE;
	do {
		encodedMessageOnly = getStringInReversedOrder(encodedMessageOnly);
		if (neededLength - encoded.size() >= encodedMessageOnly.size())
		{
			encoded.append(encodedMessageOnly);
		}
		else
		{
			int leftoverSpaceLength = encodedMessageOnly.size() - (neededLength - encoded.size());
			encoded.append(encodedMessageOnly.substr(0, leftoverSpaceLength));
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

cv::Mat Coder::create2DCode(string bitString)
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
	cv::Mat image = cv::Mat::zeros(rows, cols, CV_8UC3);
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
	}
	return image;
}

cv::Mat Coder::encodeStringToMat(std::string text)
{
	string encoded = encode(text);
	return create2DCode(encoded);
}