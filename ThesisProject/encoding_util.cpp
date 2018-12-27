#include "encoding_util.hpp"

EncodingUtil::EncodingUtil() {}

EncodingUtil::EncodingUtil(int modulesNumber)
{
	setModulesNumber(modulesNumber);
}

string EncodingUtil::encode(string text)
{
	string encoded = getBinaryAsString(text.size()).substr(BIT_NUM - SHORT_BIT_NUM, SHORT_BIT_NUM); //only 6 bits needed bc max number of encoded bits is 35
	for (int i = 0; i < text.size(); i++)
	{
		if (i != text.size() - 1)
		{
			int pairFirst = alphanumericValuesMap.find(text[i])->second;
			cout << "pairFirst: " << pairFirst << endl;
			int pairSecond = alphanumericValuesMap.find(text[++i])->second;
			cout << "pairSecond: " << pairSecond << endl;
			encoded.append(getBinaryAsString(45 * pairFirst + pairSecond));
			cout << "encoded together: " << getBinaryAsString(45 * pairFirst + pairSecond) << endl;
		}
		else
		{
			int last = alphanumericValuesMap.find(text[i])->second;
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

string EncodingUtil::encodeErrorCorrectionCodeword(vector<int> codeword)
{
	string encodedErrorCorrection = "";
	for (int value : codeword)
	{
		encodedErrorCorrection.append(getBinaryAsString(value).substr(BIT_NUM - BYTE_BIT_NUM, BYTE_BIT_NUM));
		cout << "code correction: " << getBinaryAsString(value).substr(BIT_NUM - BYTE_BIT_NUM, BYTE_BIT_NUM) << endl;
	}
	return encodedErrorCorrection;
}

Mat EncodingUtil::create2DCode(string bitString, int moduleSideSize)
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