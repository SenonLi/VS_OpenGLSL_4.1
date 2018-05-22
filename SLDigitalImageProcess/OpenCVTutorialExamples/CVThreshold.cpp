#include "CVThreshold.h"


CVThreshold* ptrThreshInstance;

extern "C" void ptrThreshold_Demo(int a, void* b)
{
	ptrThreshInstance->Threshold_Demo(a, b);
}

int CVThreshold::showWidget()
{
	::ptrThreshInstance = this;

	cv::String imageName("../WatchMe/Images/poor_3.bmp"); 
	src = cv::imread(imageName, cv::IMREAD_COLOR); // Load an image

	if (src.empty())
	{
		return -1;
	}

	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY); // Convert the image to Gray
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE); // Create a window to display results

	cv::createTrackbar(trackbar_type,
		window_name, &threshold_type,
		max_type, ptrThreshold_Demo); // Create Trackbar to choose type of Threshold

	cv::createTrackbar(trackbar_value,
		window_name, &threshold_value,
		max_value, ptrThreshold_Demo); // Create Trackbar to choose Threshold value
									//! [trackbar]

	Threshold_Demo(0, 0); // Call the function to initialize

						  /// Wait until user finishes program
	for (;;)
	{
		char c = (char)cv::waitKey(20);
		if (c == 27)
		{
			break;
		}
	}

}

void CVThreshold::Threshold_Demo(int, void*)
{
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/

	cv::threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);
	cv::imshow(window_name, dst);
}