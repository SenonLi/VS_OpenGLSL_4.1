// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// TODO: reference additional headers your program requires here

#include <tchar.h>
#include <string>

/// <remarks> atlimage.h is the Header of Windows GDI: Graphics Device Interface 
/// Add for CImage, to load and process image.
/// In GDI, all DIBs are bottom-up. DIB: Device-Independent Bitmap </remarks>
#include<atlimage.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "SLOpenCVBasics.h"


namespace slopencv
{
	static const int           MAX_CPU_SINGLE_CHANNEL_VALUE_INT          = 255;
	static const unsigned int  MAX_CPU_SINGLE_CHANNEL_VALUE_UNSIGNED_INT = 255;

	static const cv::Scalar CV_COLOR_SCALAR_RED = cv::Scalar(0, 0, 255);
	static const cv::Scalar CV_COLOR_SCALAR_BLUE = cv::Scalar(255, 0, 0);
	static const cv::Scalar CV_COLOR_SCALAR_YELLOW = cv::Scalar(0, 255, 255);
	static const cv::Scalar CV_COLOR_SCALAR_CYAN = cv::Scalar(255, 255, 0);
	static const cv::Scalar CV_COLOR_SCALAR_PURPLE = cv::Scalar(255, 0, 255);
	static const cv::Scalar CV_COLOR_SCALAR_WHITE = cv::Scalar(255, 255, 255);
	static const cv::Scalar CV_COLOR_SCALAR_BLACK = cv::Scalar(0, 0, 0);
	static const cv::Scalar CV_COLOR_SCALAR_EyeProtection = cv::Scalar(187, 223, 187);

	static const int           MAX_CANNY_THRESH_RATIO_INT                = 35;

}
