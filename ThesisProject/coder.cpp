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
	moduleSideSize = codeSideSize / codeSide; //amount of pixels in module side
}

void Coder::setModulesNumber(int modulesNumber)
{
	this->modulesNumber = modulesNumber;
	codeSide = sqrt(modulesNumber); //amount of modules in code side
	moduleSideSize = codeSideSize / codeSide; //amount of pixels in module side
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






//decoding specific methods
string Coder::decode(string bitStream)
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

string Coder::getBitStreamFrom2DCode(Mat code)
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
Mat Coder::perspectiveTransform(Mat input, Point2f* inputQuad)
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








//encoding specific methods
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

Mat Coder::create2DCode(string bitString, int moduleSideSize)
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
		else if (i <= COLOR_NUMBER + (allPaletteInFirstLine ? 1 : 2))
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