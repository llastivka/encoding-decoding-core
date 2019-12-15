#include "coder.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{

	//cv::Mat image = imread("for_test2.jpg", cv::IMREAD_COLOR);
	cv::Mat image = imread("hoseok.png", cv::IMREAD_COLOR);

	int modulesNumber = 49;
	Coder decoder = Coder(modulesNumber);
	string decoded = decoder.decodeStringFromMat(image);
	std::cout << decoded << endl;

	/*
	string message = "jungkook";
	std::cout << message << endl;
	int modulesNumber = 49;
	Coder encoder = Coder(modulesNumber);
	cv::Mat code = encoder.encodeStringToMat(message);
	imshow("jungkook", code);
	imwrite("jungkook.png", code);
	*/

	/*
	vector<int> corners = decoder.getCorners(image);
	string decoded = decoder.decodeStringFromMatWithCorners(image, corners);
	std::cout << decoded << endl;
	*/

	std::cout << "Press any key" << endl;
	cv::waitKey(0);
	
	return 0;
}