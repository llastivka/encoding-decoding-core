#include "coder.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{
	/*Mat image = imread("side_pic1.jpg", IMREAD_COLOR);
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(678, 655);
	inputQuad[1] = Point2f(1135, 733);
	inputQuad[2] = Point2f(1043, 1218);
	inputQuad[3] = Point2f(602, 1226);*/

	/*Mat image = imread("side_pic2.jpg", IMREAD_COLOR);
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(360, 530);
	inputQuad[1] = Point2f(1300, 600);
	inputQuad[2] = Point2f(1214, 1456);
	inputQuad[3] = Point2f(368, 1578);*/

	//Mat image = imread("side_pic3.jpg", IMREAD_COLOR);
	Point2f inputQuad[4];
	inputQuad[0] = Point2f(171, 653);
	inputQuad[1] = Point2f(1215, 654);
	inputQuad[2] = Point2f(1081, 1292);
	inputQuad[3] = Point2f(358, 1298);

	Mat image = imread("lena7_real.png", IMREAD_COLOR);
	
	ErrorCorrector errorCorrector = ErrorCorrector();
	int modulesNumber = 49;

	/*string message = "IRENA";
	cout << message << endl;
	int modulesNumber = 100;
	Coder encoder = Coder(modulesNumber);
	encoder.setErrorCorrector(errorCorrector);
	string encoded = encoder.encode(message);
	cout << encoded << endl;

	Mat code = encoder.create2DCode(encoded);
	imshow("encoded", code);
	imwrite("encoded.png", code);*/

	Coder decoder = Coder(modulesNumber);
	decoder.setErrorCorrector(errorCorrector);

	Mat transformed = decoder.perspectiveTransform(image, inputQuad);
	//imshow("transformed", transformed);
	//imwrite("transformed.png", transformed);
	string bitStream = decoder.getBitStreamFrom2DCode(transformed);
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
	imshow("transformed", transformed);
	imwrite("transformed.png", transformed);
	cout << "Press any key" << endl;
	waitKey(0);
	
	return 0;
}