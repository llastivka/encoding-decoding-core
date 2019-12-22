#include "coder.hpp"

#define CV_PI   3.1415926535897932384626433832795

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

int Coder::getPaletteIndex(std::vector<Coder::color> palette, Coder::color currentColor)
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

	//for testing purposes
	/*if (wasNotChanged)
	{
		int i = 5;
		int y = i + 1;
	}*/
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
	int thresholdHSV = 20;
	int lightThreshold = 70;
	int darkThreshold = 70;
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
			bool currentColorHasEnoughChrominance = abs(currentB - currentG) > threshold || abs(currentB - currentR) > threshold || abs(currentR - currentG);
			bool currentColorIsNotTooDark = currentB > darkThreshold || currentG > darkThreshold || currentR > darkThreshold;
			bool currentColorIsNotTooLight = currentB < (255 - lightThreshold) || currentG < (255 - lightThreshold) || currentR < (255 - lightThreshold);
			if (currentColorHasEnoughChrominance && currentColorIsNotTooDark && currentColorIsNotTooLight)
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
			if (currentS > thresholdHSV /*&& currentV > thresholdHSV && currentV < 100 - thresholdHSV*/)
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
		//std::cout << "(" + b + ", " + g + ", " + r + ")" << endl;

		//hsv version
		if (moduleHSVNums[i] > 0)
		{
			moduleHSVAverages[i][0] = moduleHSVSums[i][0] / moduleHSVNums[i];
			moduleHSVAverages[i][1] = moduleHSVSums[i][1] / moduleHSVNums[i];
			moduleHSVAverages[i][2] = moduleHSVSums[i][2] / moduleHSVNums[i];
		}
	}

	//for testing purposes
	cv::Mat image = cv::Mat::zeros(code.rows, code.cols, CV_8UC3);
	for (int r = 0; r < code.rows; r++)
	{
		for (int c = 0; c < code.cols; c++)
			{
			int index = r / moduleSideSize * codeSide + c / moduleSideSize;
			color pixelColor = moduleColorAverages[index];
			//std::cout << index << ": " << pixelColor.b << ", " << pixelColor.g << ", " << pixelColor.r << endl;
			image.at<cv::Vec3b>(r, c)[0] = pixelColor.b;
			image.at<cv::Vec3b>(r, c)[1] = pixelColor.g;
			image.at<cv::Vec3b>(r, c)[2] = pixelColor.r;
			}
		cv::imwrite("average.png", image);
	}

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

			//std::std::cout << "palette color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
		}
		else
		{
			Coder::color currentColor = moduleColorAverages[i];
			cv::Vec3b currentPixelHSV = moduleHSVAverages[i];
			//std::std::cout << "current color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
			//int paletteIndex = getPaletteIndex(paletteHSV, currentPixelHSV);
			int paletteIndex = getPaletteIndex(paletteInCode, currentColor);
			//next line is for testing purposes (comment it out if not needed)
			std::cout << paletteIndex + 1 << endl;
			//std::std::cout << "decided module color: " << paletteIndex << endl;
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
cv::Mat Coder::perspectiveTransform(cv::Mat input, vector<cv::Point2i> inputQuad)
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
	outputQuad[0] = cv::Point2f((float) 0, (float) 0);
	outputQuad[1] = cv::Point2f((float) (output.cols - 1), (float) 0);
	outputQuad[2] = cv::Point2f((float) (output.cols - 1), (float) (output.rows - 1));
	outputQuad[3] = cv::Point2f((float) 0, (float) (output.rows - 1));

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

cv::Mat increaseContrast(cv::Mat image)
{
	cv::Mat newImage = cv::Mat::zeros(image.size(), image.type());
	double alpha = 1.7;
	int beta = 0;
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				newImage.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(alpha*image.at<cv::Vec3b>(y, x)[c] + beta);
			}
		}
	}
	return newImage;
}

cv::Mat Coder::perspectiveTransform(cv::Mat input)
{
	vector<cv::Point2i> inputQuad = getAnglesFromImageAlternatively(input);
	//cv::imwrite("circled.png", input);
	return perspectiveTransform(input, inputQuad);
}

cv::Mat Coder::threasholdImage(cv::Mat img)
{
	medianBlur(img, img, 5);
	cv::imwrite("1_after_blur.png", img);


	double thres = 240;
	double color = 255;
	//threshold(img, img, thres, color, cv::THRESH_BINARY);
	threshold(img, img, thres, color, cv::THRESH_BINARY);
	//cv::adaptiveThreshold(img, img, color, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 2);
	cv::imwrite("2_after_threshold.png", img);


	// Execute erosion to improve the detection
	int erosion_size = 3;
	cv::Mat element = getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));
	//erode(img, img, element);
	cv::imwrite("3_after_erosion.png", img);
	return img;
}

vector<cv::Vec2f> calculateHorizontalHoughLines(cv::Mat src, bool needHorizontalLines)
{
	cv::Mat dst, cdst;
	cv::Canny(src, dst, 50, 200, 3);
	cv::cvtColor(dst, cdst, cv::COLOR_GRAY2BGR);
	vector<cv::Vec2f> result;
	cv::HoughLines(dst, result, 1, CV_PI / 180, 130, 0, 0);

	vector<cv::Vec2f> horizontalLines;
	vector<cv::Vec2f> verticalLines;
	for (int i = 0; i < result.size(); i++)
	{
		float rho = result[i][0], theta = result[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 10000 * (-b));
		pt1.y = cvRound(y0 + 10000 * (a));
		pt2.x = cvRound(x0 - 10000 * (-b));
		pt2.y = cvRound(y0 - 10000 * (a));
		double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x) * 180.0 / CV_PI;
		bool horizontal = abs(angle) < 45.0 || abs(angle) > 135.0;
		if (needHorizontalLines && horizontal)
		{
			horizontalLines.push_back(result[i]);
		}
		else if (!needHorizontalLines && !horizontal)
		{
			verticalLines.push_back(result[i]);
		}
	}

	return needHorizontalLines ? horizontalLines : verticalLines;
}

void drawLines(cv::Mat img, vector<cv::Vec2f> lines) {
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 10000 * (-b));
		pt1.y = cvRound(y0 + 10000 * (a));
		pt2.x = cvRound(x0 - 10000 * (-b));
		pt2.y = cvRound(y0 - 10000 * (a));
		cv::line(img, pt1, pt2, cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
	}
}

bool pointOnTheLine(cv::Vec2f line, int x, int y)
{
	float rho = line[0], theta = line[1];
	cv::Point pt1, pt2;
	double a = cos(theta), b = sin(theta);
	double x0 = a * rho, y0 = b * rho;
	pt1.x = cvRound(x0 + 10000 * (-b));
	pt1.y = cvRound(y0 + 10000 * (a));
	pt2.x = cvRound(x0 - 10000 * (-b));
	pt2.y = cvRound(y0 - 10000 * (a));

	std::vector<int> vector1 = { pt2.x - pt1.x, pt2.y - pt1.y };
	std::vector<int> vector2 = { pt2.x - x, pt2.y - y };
	int crossProdut = vector1[0] * vector2[1] - vector1[1] * vector2[0];
	return crossProdut > -500000 && crossProdut < 500000;
	/*
	double yDiff = pt1.y - pt2.y;
	double xDiff = pt2.x - pt1.x;
	double c = pt1.x * pt2.y - pt2.x * pt1.y;
	return abs(yDiff * x + xDiff * y + c) / sqrt(a * yDiff + xDiff * xDiff) < 20;
	*/
}

cv::Point2i getMostCornerPoint(cv::Point2i originCornerPoint, vector<cv::Point> cornerArea)
{
	double minDistance = DBL_MAX; //first setting it to a big number so at least one point was chosen
	cv::Point2i cornerPoint;

	for (int i = 0; i < cornerArea.size(); i++)
	{
		cv::Point2i currentPoint = cornerArea[i];
		double currentDistance = pow(currentPoint.x - originCornerPoint.x, 2) + pow(currentPoint.y - originCornerPoint.y, 2);
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
			cornerPoint = currentPoint;
		}
	}

	return cornerPoint;
}

cv::Point2i getCornerPoint(vector<vector<cv::Point>> cornerAreas, int cornerOrder, int cols, int rows)
{
	const int UPPER_LEFT_CORNER = 0, UPPER_RIGHT_CORNER = 1, LOWER_RIGHT_CORNER = 2, LOWER_LEFT_CORNER = 3;
	vector<cv::Point> cornerArea;
	for (int i = 0; i < cornerAreas.size(); i++)
	{
		int countOfGreaterXs = 0;
		int countOfGreaterYs = 0;
		for (int j = 0; j < cornerAreas.size(); j++)
		{
			if (cornerAreas[i][0].x < cornerAreas[j][0].x)
			{
				countOfGreaterXs++;
			}
			if (cornerAreas[i][0].y < cornerAreas[j][0].y)
			{
				countOfGreaterYs++;
			}
		}
		if (cornerOrder == UPPER_LEFT_CORNER && countOfGreaterXs >= 2 && countOfGreaterYs >= 2)
		{
			cornerArea = cornerAreas[i];
			break;
		}
		else if (cornerOrder == UPPER_RIGHT_CORNER && countOfGreaterXs <= 1 && countOfGreaterYs >= 2)
		{
			cornerArea = cornerAreas[i];
			break;
		}
		else if (cornerOrder == LOWER_RIGHT_CORNER && countOfGreaterXs <= 1 && countOfGreaterYs <= 1)
		{
			cornerArea = cornerAreas[i];
			break;
		}
		else if (cornerOrder == LOWER_LEFT_CORNER && countOfGreaterXs >=2 && countOfGreaterYs <= 1)
		{
			cornerArea = cornerAreas[i];
			break;
		}
	}

	cv::Point2i originCornerPoint;
	switch (cornerOrder) {
	case UPPER_LEFT_CORNER:
		originCornerPoint = cv::Point2i(0, 0);
		break;
	case UPPER_RIGHT_CORNER:
		originCornerPoint = cv::Point2i(cols - 1, 0);
		break;
	case LOWER_RIGHT_CORNER:
		originCornerPoint = cv::Point2i(cols - 1, rows - 1);
		break;
	case LOWER_LEFT_CORNER:
		originCornerPoint = cv::Point2i(0, rows - 1);
		break;
	default:
		break;
	}
	
	return getMostCornerPoint(originCornerPoint, cornerArea);
}

bool needHorizontalLines(vector<cv::Point> area1, vector<cv::Point> area2, int cols, int rows)
{
	bool left = false, right = false, bottom = false, top = false;
	if (area1[0].x >= cols / 2 && area2[0].x >= cols / 2)
	{
		right = true;
	}
	else if (area1[0].x <= cols / 2 && area2[0].x <= cols / 2)
	{
		left = true;
	}
	else if (area1[0].y >= rows / 2 && area2[0].y >= rows / 2)
	{
		bottom = true;
	}
	else if (area1[0].y <= rows / 2 && area2[0].y <= rows / 2)
	{
		top = true;
	}

	return left || right;
}

vector<cv::Point2i> Coder::getAnglesFromImageAlternatively(cv::Mat image)
{
	cv::imwrite("anglesFromImage_Before.png", image);
	//image = makeWhiteWhiter(image);
	image = increaseContrast(image);
	cv::imwrite("0_after contrast.png", image);

	//gray scaling and thresholding for further angles search
	cv::Mat grayMat;
	cv::cvtColor(image, grayMat, cv::COLOR_BGR2GRAY);
	grayMat = threasholdImage(grayMat);

	cv::imwrite("anglesFromImage_AfterColor.png", image);
	//cv::imwrite("anglesFromImage_AfterGray.png", grayMat);

	vector<cv::Point2i> inputQuad;
	vector<cv::Vec2i> angles = {};

	vector<vector<cv::Point>> contours; // Vector for storing contour
	vector<cv::Vec4i> hierarchy;
	findContours(grayMat, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); // Find the contours in the image

	if (contours.size() > 4)
	{
		//finding largest white areas (kinda sorting)
		vector<double> largestArea(contours.size(), 0.0);
		vector<int> largestContourIndexes(contours.size(), 0);
		for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			for (int j = 0; j < i + 1; j++)
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

		vector<cv::Vec2f> lines = calculateHorizontalHoughLines(grayMat, needHorizontalLines(contours[largestContourIndexes[0]], 
			contours[largestContourIndexes[1]], image.cols, image.rows));

		//finding all the lines that are connected to the 2 largest white areas
		vector<cv::Vec2f> largestSquaresLines1;

		vector<cv::Point> whiteArea1 = contours.at(largestContourIndexes[0]);
		for (int j = 0; j < whiteArea1.size(); j++)
		{
			int pointX = whiteArea1.at(j).x;
			int pointY = whiteArea1.at(j).y;
			for (int k = 0; k < lines.size(); k++)
			{
				if (std::find(largestSquaresLines1.begin(), largestSquaresLines1.end(), lines[k]) == largestSquaresLines1.end())
				{
					if (pointOnTheLine(lines[k], pointX, pointY))
					{
						largestSquaresLines1.push_back(lines[k]);
						break;
					}
				}
			}
		}

		vector<cv::Vec2f> largestSquaresLines2;
		vector<cv::Point> whiteArea2 = contours.at(largestContourIndexes[1]);
		for (int j = 0; j < whiteArea2.size(); j++)
		{
			int pointX = whiteArea2.at(j).x;
			int pointY = whiteArea2.at(j).y;
			for (int k = 0; k < lines.size(); k++)
			{
				if (std::find(largestSquaresLines2.begin(), largestSquaresLines2.end(), lines[k]) == largestSquaresLines2.end())
				{
					if (pointOnTheLine(lines[k], pointX, pointY))
					{
						largestSquaresLines2.push_back(lines[k]);
						break;
					}
				}
			}
		}

		drawLines(grayMat, largestSquaresLines1);
		drawLines(grayMat, largestSquaresLines2);
		cv::imwrite("lines.png", grayMat);

		/*
		//removing common lines
		for (int i = 0; i < largestSquaresLines1.size(); i++)
		{
			if (std::find(largestSquaresLines2.begin(), largestSquaresLines2.end(), largestSquaresLines1[i]) != largestSquaresLines2.end())
			{
				largestSquaresLines2.erase(std::remove(largestSquaresLines2.begin(), largestSquaresLines2.end(), largestSquaresLines1[i]), largestSquaresLines2.end());
				largestSquaresLines1.erase(largestSquaresLines1.begin() + i);
				i--;
			}
		}
		*/

		vector<vector<cv::Point>> cornerAreas;
		cornerAreas.push_back(contours[largestContourIndexes[0]]);
		cornerAreas.push_back(contours[largestContourIndexes[1]]);
		for (int i = 2; i < largestContourIndexes.size(); i++)
		{
			vector<cv::Point> currentArea = contours[largestContourIndexes[i]];
			bool currentAreaStatus = false;
			for (int j = 0; j < currentArea.size(); j++)
			{
				int pointX = currentArea.at(j).x;
				int pointY = currentArea.at(j).y;
				for (int k = 0; k < largestSquaresLines1.size(); k++)
				{
					if (pointOnTheLine(largestSquaresLines1[k], pointX, pointY))
					{
						cornerAreas.push_back(contours[largestContourIndexes[i]]);
						currentAreaStatus = true;
						break;
					}

				}
				if (currentAreaStatus)
				{
					break;
				}
				for (int k = 0; k < largestSquaresLines2.size(); k++)
				{
					if (pointOnTheLine(largestSquaresLines2[k], pointX, pointY))
					{
						cornerAreas.push_back(contours[largestContourIndexes[i]]);
						currentAreaStatus = true;
						break;
					}

				}
			}
			if (cornerAreas.size() == 4) {
				break;
			}
		}

		inputQuad.push_back(getCornerPoint(cornerAreas, 0, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(cornerAreas, 1, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(cornerAreas, 2, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(cornerAreas, 3, image.cols, image.rows));

	}
	else
	{
		inputQuad.push_back(getCornerPoint(contours, 0, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(contours, 1, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(contours, 2, image.cols, image.rows));
		inputQuad.push_back(getCornerPoint(contours, 3, image.cols, image.rows));
	}
	//for testing purposes
	for (int i = 0; i < 4; i++)
	{
		circle(image, inputQuad[i], 5, cv::Scalar(0, 0, 255), 3, 8, 0);
	}
	cv::imwrite("4_angles.png", image);

	return inputQuad;
}

vector<cv::Point2i> Coder::getAnglesFromImage(cv::Mat image)
{
	cv::imwrite("anglesFromImage_Before.png", image);
	//image = makeWhiteWhiter(image);
	image = increaseContrast(image);
	cv::imwrite("0_after contrast.png", image);

	//gray scaling and thresholding for further angles search
	cv::Mat grayMat;
	cv::cvtColor(image, grayMat, cv::COLOR_BGR2GRAY);
	grayMat = threasholdImage(grayMat);

	cv::imwrite("anglesFromImage_AfterColor.png", image);
	//cv::imwrite("anglesFromImage_AfterGray.png", grayMat);

    vector<cv::Point2i> inputQuad;
	vector<cv::Vec2i> angles = {};

	vector<vector<cv::Point>> contours; // Vector for storing contour
	vector<cv::Vec4i> hierarchy;
	findContours(grayMat, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); // Find the contours in the image

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

	//x and y for each of 5 points (upper-left, upper-right, lower-right, lower-left, centroid) for each of 4 largest contours
	vector<vector<cv::Vec2i>> countoursImportantPointsCoordinates = {}; //final size should be 4. For now it is not really used, maybe remove later

	cv::Vec2i originUpperLeft = cv::Vec2i(0, 0);
	cv::Vec2i originUpperRight = cv::Vec2i(image.cols - 1, 0);
	cv::Vec2i originLowerRight = cv::Vec2i(image.cols - 1, image.rows - 1);
	cv::Vec2i originLowerLeft = cv::Vec2i(0, image.rows - 1);

	//here are the opposite values in order to at least one of the real values could be set
	cv::Point2i generalUpperLeft = cv::Point2i(image.cols - 1, image.rows - 1);
	cv::Point2i generalUpperRight = cv::Point2i(0, image.rows - 1);
	cv::Point2i generalLowerRight = cv::Point2i(0, 0);
	cv::Point2i generalLowerLeft = cv::Point2i(image.cols - 1, 0);
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
			//std::cout << "x: " << x << "     y: " << y << endl;
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
		//std::cout << i << "(" << largestContourIndexes[i] << "): minX = " << minX << ", maxX = " << maxX << ", minY = " << minY << ", maxY = " << maxY << endl;
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
		inputQuad[0] = Point2i(generalUpperLeft.x, generalUpperLeft.y);
		inputQuad[1] = Point2i(generalUpperRight.x, generalUpperRight.y);
		inputQuad[2] = Point2i(generalLowerRight.x, generalLowerRight.y);
		inputQuad[3] = Point2i(generalUpperLeft.x, generalLowerLeft.y);
		*/
	}

	//for now i'll just assign it but i'll have to rewrite it eventually
	inputQuad.push_back(cv::Point2i(generalUpperLeft.x, generalUpperLeft.y));
	inputQuad.push_back(cv::Point2i(generalUpperRight.x, generalUpperRight.y));
	inputQuad.push_back(cv::Point2i(generalLowerRight.x, generalLowerRight.y));
	inputQuad.push_back(cv::Point2i(generalLowerLeft.x, generalLowerLeft.y));

	cv::Point2i inputQuad1[4];
	inputQuad1[0] = cv::Point2i(generalUpperLeft.x, generalUpperLeft.y);
	inputQuad1[1] = cv::Point2i(generalUpperRight.x, generalUpperRight.y);
	inputQuad1[2] = cv::Point2i(generalLowerRight.x, generalLowerRight.y);
	inputQuad1[3] = cv::Point2i(generalLowerLeft.x, generalLowerLeft.y);

	for (int i = 0; i < 4; i++)
	{
		circle(image, inputQuad1[i], 5, cv::Scalar(0, 0, 255), 3, 8, 0);
	}
	cv::imwrite("4_angles.png", image);

	return inputQuad;
}

cv::Mat Coder::createMat(int8_t* image, int32_t rows, int32_t cols) {
	cv::Mat mat(rows, cols, CV_8UC3, image);
	return mat;
}

std::string Coder::decodeMessageFromImage(int8_t* image, int32_t rows, int32_t cols, vector<int32_t> xInputQuad, vector<int32_t> yInputQuad) {
//	cv::Point2i inputQuad[4];
//	inputQuad[0] = cv::Point2i(xInputQuad[0], yInputQuad[0]);
//	inputQuad[1] = cv::Point2i(xInputQuad[1], yInputQuad[1]);
//	inputQuad[2] = cv::Point2i(xInputQuad[2], yInputQuad[2]);
//	inputQuad[3] = cv::Point2i(xInputQuad[3], yInputQuad[3]);
//
//	//int8_t* imageArr = &image[0];
//
//	cv::Mat imageMat;
//	try {
//		imageMat = createMat(image, rows, cols);
//	}
//	catch (exception& e) {
//		std::cout << e.what() << '\n';
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
	transformed = gradientBrightness(transformed);
	cv::imwrite("transformed_brightened.png", transformed);
	string bitStream = getBitStreamFrom2DCode(transformed);
	std::cout << bitStream << endl;
	string decoded = decode(bitStream);
	return decoded;
}

cv::Mat Coder::gradientBrightness(cv::Mat image)
{
	Coder::color cornerModuleAverage[4];
	cornerModuleAverage[0] = getCornerModuleAverage(image, 0, 0);
	cornerModuleAverage[1] = getCornerModuleAverage(image, moduleSideSize * (codeSide - 1), 0);
	cornerModuleAverage[2] = getCornerModuleAverage(image, moduleSideSize * (codeSide - 1), moduleSideSize * (codeSide - 1));
	cornerModuleAverage[3] = getCornerModuleAverage(image, 0, moduleSideSize * (codeSide - 1));
	
	//calculation of average closeness to max luminance of each rgb color
	int averageClosenessToMaxLuminance[4];
	averageClosenessToMaxLuminance[0] = getAverageClosenessToMaxLuminance(cornerModuleAverage[0].b, cornerModuleAverage[0].g, cornerModuleAverage[0].r);
	averageClosenessToMaxLuminance[1] = getAverageClosenessToMaxLuminance(cornerModuleAverage[1].b, cornerModuleAverage[1].g, cornerModuleAverage[1].r);
	averageClosenessToMaxLuminance[2] = getAverageClosenessToMaxLuminance(cornerModuleAverage[2].b, cornerModuleAverage[2].g, cornerModuleAverage[2].r);
	averageClosenessToMaxLuminance[3] = getAverageClosenessToMaxLuminance(cornerModuleAverage[3].b, cornerModuleAverage[3].g, cornerModuleAverage[3].r);

	int leftAverage = (averageClosenessToMaxLuminance[0] + averageClosenessToMaxLuminance[3]) / 2;
	int rightAverage = (averageClosenessToMaxLuminance[1] + averageClosenessToMaxLuminance[2]) / 2;
	int topAverage = (averageClosenessToMaxLuminance[0] + averageClosenessToMaxLuminance[1]) / 2;
	int bottomAverage = (averageClosenessToMaxLuminance[2] + averageClosenessToMaxLuminance[3]) / 2;

	int horizontalDirection = rightAverage > leftAverage ? 1 : -1;
	int horizontalMax = rightAverage > leftAverage ? rightAverage : leftAverage;
	double horizontalDiff = abs(leftAverage - rightAverage) / 255.0;
	int verticalDirection = bottomAverage > topAverage ? 1 : -1;
	int verticalMax = bottomAverage > topAverage ? bottomAverage : topAverage;
	double verticalDiff = abs(topAverage - bottomAverage) / 255.0;

		
	cv::Mat newImage = cv::Mat::zeros(image.size(), image.type());
	double alpha = 1.0;
	double alphaDiff;
	int beta = 0;
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				alphaDiff = 0.5 + 3 * (horizontalDirection * (horizontalDiff * (double) x / (double) codeSideSize) + verticalDirection * (verticalDiff * (double) y / (double) codeSideSize));
				newImage.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar> ((alpha + alphaDiff) * image.at<cv::Vec3b>(y, x)[c] + beta);
			}
		}
	}
	return newImage;
}

int Coder::getAverageClosenessToMaxLuminance(int b, int g, int r)
{
	int max = 255;
	return (max - b + max - g + max - r) / 3;
}

Coder::color Coder::getCornerModuleAverage(cv::Mat image, int startX, int startY)
{
	int bSum = 0, gSum = 0, rSum = 0;
	for (int r = startY; r < startY + moduleSideSize; r++)
	{
		for (int c = startX; c < startX + moduleSideSize; c++)
		{
			bSum += image.at<cv::Vec3b>(r, c)[0];
			gSum += image.at<cv::Vec3b>(r, c)[1];
			rSum += image.at<cv::Vec3b>(r, c)[2];
		}
	}
	Coder::color average;
	average.b = bSum / (moduleSideSize * moduleSideSize);
	average.g = gSum / (moduleSideSize * moduleSideSize);
	average.r = rSum / (moduleSideSize * moduleSideSize);
	return average;
}

std::vector<int> Coder::getCorners(cv::Mat mat)
{
	std::vector<cv::Point2i> inputQuad = getAnglesFromImage(mat);
	std::vector<int> corners;
	for (int i = 0; i < inputQuad.size(); i++)
	{
		corners.push_back(inputQuad.at(i).x);
		corners.push_back(inputQuad.at(i).y);
	}
	return corners;
}

std::string Coder::decodeStringFromMatWithCorners(cv::Mat mat, vector<int> corners)
{
	vector<cv::Point2i> cornersQuad { cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0) };
	cornersQuad[0].x = corners[0];
	cornersQuad[0].y = corners[1];
	cornersQuad[1].x = corners[2];
	cornersQuad[1].y = corners[3];
	cornersQuad[2].x = corners[4];
	cornersQuad[2].y = corners[5];
	cornersQuad[3].x = corners[6];
	cornersQuad[3].y = corners[7];

	//cv::imwrite("circled.png", input);
	cv::Mat transformed = perspectiveTransform(mat, cornersQuad);
	if (transformed.empty()) {
		return "ERROR";
	}
	//cv::imwrite("transformed.png", transformed);
	string bitStream = getBitStreamFrom2DCode(transformed);
	std::cout << bitStream << endl;
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
		text = DEFAULT_LINK_MARKER + text;
	}

	text = toUpperCase(text);
	string encoded = getBinaryAsString(text.size()).substr(BIT_NUM - SHORT_BIT_NUM, SHORT_BIT_NUM); //only 6 bits needed bc max number of encoded bits is 35
	string encodedMessageOnly = "";
	for (int i = 0; i < text.size(); i++)
	{
		if (i != text.size() - 1)
		{
			int pairFirst = encodingAlphanumericValuesMap.find(text[i])->second;
			std::cout << "pairFirst: " << pairFirst << endl;
			int pairSecond = encodingAlphanumericValuesMap.find(text[++i])->second;
			std::cout << "pairSecond: " << pairSecond << endl;
			string encodedPair = getBinaryAsString(45 * pairFirst + pairSecond);
			encoded.append(encodedPair);
			encodedMessageOnly.append(encodedPair);
			std::cout << "encoded together: " << getBinaryAsString(45 * pairFirst + pairSecond) << endl;
		}
		else
		{
			int last = encodingAlphanumericValuesMap.find(text[i])->second;
			string binary = getBinaryAsString(last);
			std::cout << "last: " << last << " - " << getBinaryAsString(last) << endl;
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
		std::cout << "code correction: " << getBinaryAsString(value).substr(BIT_NUM - BYTE_BIT_NUM, BYTE_BIT_NUM) << endl;
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
			//std::cout << index << ": " << pixelColor.b << ", " << pixelColor.g << ", " << pixelColor.r << endl;
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