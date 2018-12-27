#include "decoding_util.hpp"
#include "encoding_util.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{
	Mat image = imread("lena_saturation.png", IMREAD_COLOR);

	ErrorCorrector errorCorrector = ErrorCorrector();
	int modulesNumber = 100;
	/*string message = "IRENA";
	cout << message << endl;
	int modulesNumber = 100;
	EncodingUtil encoder = EncodingUtil(modulesNumber);
	encoder.setErrorCorrector(errorCorrector);
	string encoded = encoder.encode(message);
	cout << encoded << endl;

	Mat code = encoder.create2DCode(encoded, 100);
	imshow("encoded", code);
	imwrite("encoded.png", code);*/

	DecodingUtil decoder = DecodingUtil(modulesNumber);
	decoder.setErrorCorrector(errorCorrector);
	string bitStream = decoder.getBitStreamFrom2DCode(image);
	cout << bitStream << endl;
	string decoded = decoder.decode(bitStream);
	cout << decoded << endl;

	/*
	//inputQuad is what will be passed from the app (at least at this stage)
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(142, 6);
	inputQuad[1] = Point2f(458, 62);
	inputQuad[2] = Point2f(457, 365);
	inputQuad[3] = Point2f(172, 477);

	//outputQuad is set and constant
	Point2f outputQuad[4];
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f(image.cols - 1, 0);
	outputQuad[2] = Point2f(image.cols - 1, image.rows - 1);
	outputQuad[3] = Point2f(0, image.rows - 1);

	Mat output = decoder.perspectiveTransform(image, inputQuad, outputQuad);
	
	imshow("output", output);
	imwrite("out.png", output);
	*/
	waitKey(0);
	
	return 0;
}