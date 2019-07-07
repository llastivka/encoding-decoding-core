#include "coder.hpp"
#include <iostream>
#include <stdint.h>

using namespace std;

int main(int argc, char const *argv[])
{
	
	cv::Mat image = imread("app_test4.png", cv::IMREAD_COLOR);

	int modulesNumber = 49;
	Coder decoder = Coder(modulesNumber);
	string decoded = decoder.decodeStringFromMat(image);
	cout << decoded << endl;

	cout << "Press any key" << endl;
	cv::waitKey(0);
	
	return 0;
}