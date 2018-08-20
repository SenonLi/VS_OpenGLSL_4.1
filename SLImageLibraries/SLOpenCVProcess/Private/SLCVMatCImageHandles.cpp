#include "../stdafx.h"

#include "SLCVMatCImageHandles.h"
#include "StaticConstBasics\SLGeneralImageBasics.h"

namespace slopencv
{

	/// <summary>Bad solution to Get cv::Mat from CImage</summary>
	/// <param name="src">CImage [IN]</param>
	/// <param name="dst">cv::Mat [OUT]</param>
	void ConvertCImageToCVMat(CImage& src, cv::Mat& dst)
	{
		assert(!src.IsNull());
		dst.release();

		switch (src.GetBPP())
		{
		case 8:
			dst.create(src.GetHeight(), src.GetWidth(), CV_8UC1);
			for (int row = 0; row < src.GetHeight(); row++) {
				for (int col = 0; col < src.GetWidth(); col++) {
					dst.at<unsigned char>(row, col) = static_cast<BYTE*>(src.GetPixelAddress(col, row))[0];
				}
			}
			break;

		case 24:
			dst.create(src.GetHeight(), src.GetWidth(), CV_8UC3);
			for (int row = 0; row < src.GetHeight(); row++) {
				for (int col = 0; col < src.GetWidth(); col++) {
					dst.at<cv::Vec3b>(row, col)[0] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[0];
					dst.at<cv::Vec3b>(row, col)[1] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[1];
					dst.at<cv::Vec3b>(row, col)[2] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[2];
				}
			}
			break;

		case 32:
			dst.create(src.GetHeight(), src.GetWidth(), CV_8UC4);
			for (int row = 0; row < src.GetHeight(); row++) {
				for (int col = 0; col < src.GetWidth(); col++) {
					dst.at<cv::Vec4b>(row, col)[0] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[0];
					dst.at<cv::Vec4b>(row, col)[1] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[1];
					dst.at<cv::Vec4b>(row, col)[2] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[2];
					dst.at<cv::Vec4b>(row, col)[3] = static_cast<BYTE*>(src.GetPixelAddress(col, row))[3];
				}
			}
			break;

		}// End of switch (src.GetBPP())
	}// End of ConvertCImageToCVMat(CImage& src, cv::Mat& dst)




	 /// <summary>cv::imwrite doesn't support Unicode, we need to use CImage to save </summary>
	 /// <param name="in">cv::Mat image [IN]</param>
	 /// <param name="out">CImage image [OUT]</param>
	 /// <remarks>This function only accept BYTE image (8bits per single channel), doesn't support double value image </remarks>
	void ConvertCVMatToCImage(const cv::Mat& in, CImage& out)
	{
		assert(!in.empty());
		assert(in.depth() == CV_8U); // Only support BYTE image
		if (!out.IsNull())
			out.Destroy();
		if (in.type() == CV_8UC1) // single channel
			out.Create(in.cols, in.rows, in.channels() * slutil::BYTE_IMAGE_SINGLE_CHANNEL_BITS * 3);//CImage doesn't support 8bit single pixel color change
		else
			out.Create(in.cols, in.rows, in.channels() * slutil::BYTE_IMAGE_SINGLE_CHANNEL_BITS);

		switch (in.channels())
		{
		case 1:
		{
			for (int row = 0; row < in.rows; row++) {
				for (int col = 0; col < in.cols; col++) {
					BYTE intensity = in.at<BYTE>(row, col);
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[0] = intensity;
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[1] = intensity;
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[2] = intensity;
				}
			}
		}break;

		case 3:
		{
			for (int row = 0; row < in.rows; row++) {
				for (int col = 0; col < in.cols; col++) {
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[0] = in.at<cv::Vec3b>(row, col)[0];
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[1] = in.at<cv::Vec3b>(row, col)[1];
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[2] = in.at<cv::Vec3b>(row, col)[2];
				}
			}
		}break;

		case 4:
		{
			for (int row = 0; row < in.rows; row++) {
				for (int col = 0; col < in.cols; col++) {
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[0] = in.at<cv::Vec4b>(row, col)[0];
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[1] = in.at<cv::Vec4b>(row, col)[1];
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[2] = in.at<cv::Vec4b>(row, col)[2];
					static_cast<BYTE*>(out.GetPixelAddress(col, row))[3] = in.at<cv::Vec4b>(row, col)[3];
				}
			}
		}break;

		default:
			assert(true);// Doesn't support cases other than 8bits/24bits/32bits image
		} // End of switch (in.channels())
	}// End of ConvertCVMatToCImage(const cv::Mat& in, CImage& out)


} // End of namespace slopencv