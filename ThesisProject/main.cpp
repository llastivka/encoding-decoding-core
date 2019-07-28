#include "coder.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{

	cv::Mat image = imread("encoded_link_grid.png", cv::IMREAD_COLOR);

	int modulesNumber = 49;
	Coder decoder = Coder(modulesNumber);
	string decoded = decoder.decodeStringFromMat(image);
	cout << decoded << endl;

	/*
	string message = "https://u.nu/4yn2";
	cout << message << endl;
	int modulesNumber = 49;
	Coder encoder = Coder(modulesNumber);
	cv::Mat code = encoder.encodeStringToMat(message);
	imshow("encoded_link", code);
	imwrite("encoded_link.png", code);
	*/

	cout << "Press any key" << endl;
	cv::waitKey(0);
	
	return 0;
}