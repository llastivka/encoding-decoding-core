#include "decoding_util.hpp"

DecodingUtil::DecodingUtil() {}

DecodingUtil::DecodingUtil(int modulesNumber)
{
	setModulesNumber(modulesNumber);
}

string DecodingUtil::decode(string bitStream)
{
	int length = getDecimalFromBinaryString(bitStream.substr(0, SHORT_BIT_NUM));
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
			decoded += alphanumericValuesMap.find(first)->second;
			decoded += alphanumericValuesMap.find(second)->second;
			currentPosition += BIT_NUM;
		}
		else
		{
			int last = getDecimalFromBinaryString(bitStream.substr(currentPosition, SHORT_BIT_NUM));
			if (last > 0 && last < 45)
			{
				decoded += alphanumericValuesMap.find(last)->second;
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

int DecodingUtil::getPaletteIndex(std::vector<Coder::color> palette, std::vector<vector<double>> paletteBgrProportions, Coder::color currentColor)
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

	int threshold = 10;
	std::vector<color> moduleColorSums(getModulesNumber());
	std::vector<int> moduleColorNums(getModulesNumber());

	std::ofstream file;
	file.open("values.txt");

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
		}
	}
	file.close();

	std::vector<color> moduleColorAverages(getModulesNumber());
	for (int i = 0; i < getModulesNumber(); i++)
	{
		vector<double> bgrProportions(3); //proportions between bgr values
		moduleColorAverages[i].b = moduleColorSums[i].b / moduleColorNums[i];
		moduleColorAverages[i].g = moduleColorSums[i].g / moduleColorNums[i];
		moduleColorAverages[i].r = moduleColorSums[i].r / moduleColorNums[i];
		//string b = to_string(moduleColorAverages[i].b);
		//string g = to_string(moduleColorAverages[i].g);
		//string r = to_string(moduleColorAverages[i].r);
		//cout << "(" + b + ", " + g + ", " + r + ")" << endl;
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
	std::vector<vector<double>> paletteBgrProportions;
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
			vector<double> bgrProportions(3); //proportions between bgr values
			bgrProportions[0] = std::min(moduleColorAverages[i].b, moduleColorAverages[i].g) / std::max(moduleColorAverages[i].b, moduleColorAverages[i].g);
			bgrProportions[1] = std::min(moduleColorAverages[i].g, moduleColorAverages[i].r) / std::max(moduleColorAverages[i].g, moduleColorAverages[i].r);
			bgrProportions[2] = std::min(moduleColorAverages[i].b, moduleColorAverages[i].r) / std::max(moduleColorAverages[i].b, moduleColorAverages[i].r);
			paletteBgrProportions.push_back(bgrProportions);

			//std::cout << "palette color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
		}
		else
		{
			color currentColor = moduleColorAverages[i];
			//std::cout << "current color: b - " << moduleColorAverages[i].b << ", g - " << moduleColorAverages[i].g <<
			//	", r - " << moduleColorAverages[i].r << endl;
			int paletteIndex = getPaletteIndex(paletteInCode, paletteBgrProportions, currentColor);
			cout << paletteIndex + 1 << endl;
			//std::cout << "decided module color: " << paletteIndex << endl;
			colorIndexSequence.push_back(paletteIndex);
			bitStream.append(getBinaryAsString(paletteIndex).substr(BIT_NUM - BITS_IN_MODULE, BITS_IN_MODULE));
			
			//add current color to the average (experiment)
			//paletteInCode[paletteIndex].b = (paletteInCode[paletteIndex].b * paletteColorAmountInCode[paletteIndex] + paletteInCode[paletteIndex].b) / (paletteColorAmountInCode[paletteIndex] + 1);
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