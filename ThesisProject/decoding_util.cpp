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
			decoded += alphanumericValuesMap.find(first)->second;
			decoded += alphanumericValuesMap.find(second)->second;
			currentPosition += BIT_NUM;
		}
		else
		{
			int last = getDecimalFromBinaryString(bitStream.substr(currentPosition, SHORT_BIT_NUM));
			decoded += alphanumericValuesMap.find(last)->second;
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

int DecodingUtil::getPaletteIndex(std::vector<Coder::color> palette, Coder::color currentColor)
{
	int index = 0;
	int bgrMinDiff = 256;
	for (int i = 0; i < palette.size(); i++)
	{
		double averageDiff = (abs(palette[i].b - currentColor.b) + abs(palette[i].g - currentColor.g) + abs(palette[i].r - currentColor.r)) / 3;
		if (averageDiff < bgrMinDiff)
		{
			bgrMinDiff = averageDiff;
			index = i;
		}
	}
	return index;
}

string DecodingUtil::getBitStreamFrom2DCode(Mat code)
{
	int threshold = 50;
	int upThreshold = 255 - threshold;
	std::vector<color> moduleColorSums(getModulesNumber());
	for (int r = 0; r < code.rows; r++)
	{
		for (int c = 0; c < code.cols; c++)
		{
			int index = r / moduleSideSize * codeSide + c / moduleSideSize;
			int currentB = code.at<cv::Vec3b>(r, c)[0];
			int currentG = code.at<cv::Vec3b>(r, c)[1];
			int currentR = code.at<cv::Vec3b>(r, c)[2];
			if (currentB > threshold && currentB < upThreshold && currentG > threshold && currentG < upThreshold &&
				currentR > threshold && currentR < upThreshold)
			{
				moduleColorSums.at(index).b += code.at<cv::Vec3b>(r, c)[0];
				moduleColorSums.at(index).g += code.at<cv::Vec3b>(r, c)[1];
				moduleColorSums.at(index).r += code.at<cv::Vec3b>(r, c)[2];
			}
		}
	}

	std::vector<color> moduleColorAverages(getModulesNumber());
	for (int i = 0; i < getModulesNumber(); i++)
	{
		moduleColorAverages[i].b = moduleColorSums[i].b / moduleColorAverages.size();
		moduleColorAverages[i].g = moduleColorSums[i].g / moduleColorAverages.size();
		moduleColorAverages[i].r = moduleColorSums[i].r / moduleColorAverages.size();
	}

	bool allPaletteInFirstLine = COLOR_NUMBER + 1 < codeSide;
	int lastPaletteModuleIndex = COLOR_NUMBER + (allPaletteInFirstLine ? 1 : 2);
	std::vector<color> paletteInCode(COLOR_NUMBER);
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
		}
		else
		{
			color currentColor = moduleColorAverages[i];
			int paletteIndex = getPaletteIndex(paletteInCode, currentColor);
			colorIndexSequence.push_back(paletteIndex);
			bitStream.append(getBinaryAsString(paletteIndex).substr(BIT_NUM - BITS_IN_MODULE, BITS_IN_MODULE));
			paletteColorAmountInCode[paletteIndex]++;
			//update average of palette color (work more on this later)
			paletteInCode[paletteIndex].b = (paletteInCode[paletteIndex].b + currentColor.b) / 2;
			paletteInCode[paletteIndex].g = (paletteInCode[paletteIndex].g + currentColor.g) / 2;
			paletteInCode[paletteIndex].r = (paletteInCode[paletteIndex].r + currentColor.r) / 2;
		}
	}
	return bitStream;
}

Mat DecodingUtil::perspectiveTransform(Mat input, Point2f* inputQuad, Point2f* outputQuad)
{
	Mat lambda(2, 4, CV_32FC1);
	Mat output;
	lambda = Mat::zeros(input.rows, input.cols, input.type());
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	warpPerspective(input, output, lambda, output.size());
	return output;
}