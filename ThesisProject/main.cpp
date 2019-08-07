#include "coder.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{

	cv::Mat image = imread("phone_test.jpg", cv::IMREAD_COLOR);

	int modulesNumber = 49;
	Coder decoder = Coder(modulesNumber);
	string decoded = decoder.decodeStringFromMat(image);
	std::cout << decoded << endl;

	/*
	string message = "https://u.nu/4yn2";
	std::cout << message << endl;
	int modulesNumber = 49;
	Coder encoder = Coder(modulesNumber);
	cv::Mat code = encoder.encodeStringToMat(message);
	imshow("encoded_link", code);
	imwrite("encoded_link.png", code);
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