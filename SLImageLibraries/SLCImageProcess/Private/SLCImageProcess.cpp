#include "../stdafx.h"
#include "SLCImageProcess.h"

#include <vector>
#include <wincodec.h> // for COM: IWICImagingFactory / IWICBitmapScaler
#include <VersionHelpers.h>

namespace slcimage
{
	//====================================================================================================================
	//--------------------------------------------------------------------------------------------------------------------
	void DuplicateImage(const ATL::CImage& srcImage, ATL::CImage& dstImage)
	{
		assert(!srcImage.IsNull());
		if (srcImage == dstImage)		return;
		if (!dstImage.IsNull())         dstImage.Destroy();

		int srcBitsCount = srcImage.GetBPP();
		if (srcBitsCount == 32)	dstImage.Create(srcImage.GetWidth(), srcImage.GetHeight(), srcBitsCount, CImage::createAlphaChannel);
		else                    dstImage.Create(srcImage.GetWidth(), srcImage.GetHeight(), srcBitsCount, 0);

		if (srcImage.IsIndexed())	{
			// 8-bit: 0x100000000 = 256, in the same way, 4-bit: 0x10000 = 16, 2-bit: 0x100 = 4, 1-bit: 0x10 = 2
			int nColors = srcImage.GetMaxColorTableEntries();
			if (nColors > 0)	{
				std::vector<RGBQUAD> rgbColors(nColors);
				srcImage.GetColorTable(0, nColors, rgbColors.data());
				dstImage.SetColorTable(0, nColors, rgbColors.data());
			}
		}

		srcImage.BitBlt(dstImage.GetDC(), 0, 0, SRCCOPY);
		dstImage.ReleaseDC(); // Remember to dstImage.ReleaseDC() after dstImage.GetDC()
	};// End of DuplicateImage(const CImage& srcImage, CImage& destImage)


	//========================================================================================================================
	//========================================================================================================================
	//------------------------------------------------------------------------------------------------------------------------
	void StretchImageByWidth(const ATL::CImage& srcImage, int widthInPixel, ATL::CImage& dstImage)
	{
		assert(!srcImage.IsNull() && srcImage != dstImage && widthInPixel > 0);
		if (!dstImage.IsNull()) dstImage.Destroy();

		int srcBitsCount = srcImage.GetBPP();
		double imageAspectRatio = static_cast<double>( srcImage.GetHeight() ) / srcImage.GetWidth() ;
		
		// Get valid dstImageWidth and dstImageHeight
		int dstImageWidth = widthInPixel;
		double tmpDstImageHeight = imageAspectRatio * dstImageWidth;
		int dstImageHeight = tmpDstImageHeight > 1.0 ? static_cast<int>(tmpDstImageHeight) : 1;

		// For 8bit CImage, we need to copy the ColorTable after image creation
		// IWICBitmapScaler::CopyPixels will always fail on 8bit image and BPP smaller than 8, can only use StretchBlt
		if (srcImage.IsIndexed()) {
			// If using IWICBitmapScaler::CopyPixels, IWICBitmapScaler will generate Top-Down image, 
			//       so we want to create CImage with input targetHeight < 0 for Top-Down CImage;
			// If Using StretchBlt, both dstImage and srcImage are Bottom-Up image,
			//       so we want to create CImage with input targetHeight > 0 for Bottom-Up CImage;
			dstImage.Create(dstImageWidth, dstImageHeight, srcBitsCount, 0);

			// 8-bit: 0x100000000 = 256, in the same way, 4-bit: 0x10000 = 16, 2-bit: 0x100 = 4, 1-bit: 0x10 = 2
			int nColors = srcImage.GetMaxColorTableEntries();
			if (nColors > 0) {
				std::vector<RGBQUAD> rgbColors(nColors);
				srcImage.GetColorTable(0, nColors, rgbColors.data());
				dstImage.SetColorTable(0, nColors, rgbColors.data());
			}

			HDC dstHDC = dstImage.GetDC();
			SetBrushOrgEx(dstHDC, 0, 0, nullptr);
			SetStretchBltMode(dstHDC, HALFTONE);
			srcImage.StretchBlt(dstHDC, 0, 0, dstImageWidth, dstImageHeight, SRCCOPY);
			dstImage.ReleaseDC();
		} // End of if (srcImage.IsIndexed())
		else
		{
			// Down-Sampling using Bicubic algorithm, COM application
			// COM is run-time system component and it may fail due to all kind of reasons,
			// which means any COM runtime error will lead to a crash, so we must check the return value every step
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				CComPtr<IWICImagingFactory> imagingFactory;
				hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));
				if (SUCCEEDED(hr))	{
					CComPtr<IWICBitmap> srcIWICBitmap;
					hr = imagingFactory->CreateBitmapFromHBITMAP(srcImage, 0, WICBitmapUseAlpha, &srcIWICBitmap);
					if (SUCCEEDED(hr))	{
						CComPtr<IWICBitmapScaler> bitmapScaler;
						hr = imagingFactory->CreateBitmapScaler(&bitmapScaler);
						if (SUCCEEDED(hr)) {
							if (IsWindows10OrGreater())
								hr = bitmapScaler->Initialize(srcIWICBitmap, dstImageWidth, dstImageHeight, WICBitmapInterpolationModeHighQualityCubic);
							else
								hr = bitmapScaler->Initialize(srcIWICBitmap, dstImageWidth, dstImageHeight, WICBitmapInterpolationModeCubic);
							if (SUCCEEDED(hr)) {
								if (srcBitsCount == 32)	dstImage.Create(dstImageWidth, -dstImageHeight, srcBitsCount, CImage::createAlphaChannel);
								else                    dstImage.Create(dstImageWidth, -dstImageHeight, srcBitsCount, 0);

								WICRect rect = { 0, 0, dstImageWidth, dstImageHeight };
								int stride = std::abs(dstImage.GetPitch());
								BYTE* bufferEntry = static_cast<BYTE*>(dstImage.GetBits());
								// Attension! CopyPixels will do resampling everytime it's called, we should not call it line by line
								// Attension! Resampling is not done by Initialize(...), instead, it's done by CopyPixels(...)
								hr = bitmapScaler->CopyPixels(&rect, stride, stride * dstImageHeight, bufferEntry);
							}
						}
					}
				}

				CoUninitialize();
			} // End of if ( SUCCEEDED( CoInitializeEx(..) ))

			assert(hr == S_OK);// COM failure, code should be reviewed !!
			if (FAILED(hr))	{
				const DWORD useAlphaChannel = srcBitsCount == 32 ? CImage::createAlphaChannel : 0;
				dstImage.Create(dstImageWidth, dstImageHeight, srcBitsCount, useAlphaChannel);
				HDC dstHDC = dstImage.GetDC();
				if (srcBitsCount == 32)
					SetStretchBltMode(dstHDC, COLORONCOLOR);// HALFTONE doesn't support alpha channel
				else {
					SetBrushOrgEx(dstHDC, 0, 0, nullptr);
					SetStretchBltMode(dstHDC, HALFTONE);
				}
				srcImage.StretchBlt(dstHDC, 0, 0, dstImageWidth, dstImageHeight, SRCCOPY);
				dstImage.ReleaseDC();
			}
		}
	}// End of GetSmallerImageByWidth(const ATL::CImage& srcImage, int widthInPixel, ATL::CImage& dstImage)


	//====================================================================================================================
	//--------------------------------------------------------------------------------------------------------------------
	/// <summary>Converting 1Bit/4Bit image to 8Bit/24Bit, help low bit image loading for LibreImage</summary>
	void Convert8BitBelowToAbove(const ATL::CImage& srcImage, int dstBitPerPixel, ATL::CImage& dstImage)
	{
		assert(!srcImage.IsNull() && srcImage != dstImage && srcImage.GetBPP() <= 8 && srcImage.GetBPP() < dstBitPerPixel);
		assert(dstBitPerPixel == 1 || dstBitPerPixel == 4 || dstBitPerPixel == 8 // GrayScaled
			|| dstBitPerPixel == 24 || dstBitPerPixel == 32);                    // Colored

		if (!dstImage.IsNull())     dstImage.Destroy();
		dstImage.Create(srcImage.GetWidth(), srcImage.GetHeight(), dstBitPerPixel, 0);

		// For 8bit CImage, we need to copy the ColorTable after image creation
		if (dstImage.IsIndexed()) {
			// 8-bit: 0x100000000 = 256, in the same way, 4-bit: 0x10000 = 16, 2-bit: 0x100 = 4, 1-bit: 0x10 = 2
			int nColors = srcImage.GetMaxColorTableEntries();
			if (nColors > 0) {
				std::vector<RGBQUAD> rgbColors(nColors);
				srcImage.GetColorTable(0, nColors, rgbColors.data());
				dstImage.SetColorTable(0, nColors, rgbColors.data());
			}
		}

		srcImage.BitBlt(dstImage.GetDC(), 0, 0, SRCCOPY);
		dstImage.ReleaseDC(); // Remember to dstImage.ReleaseDC() after dstImage.GetDC()
	}

	void Convert8bitTo32Bit(const ATL::CImage& srcImage, ATL::CImage& dstImage)
	{
		assert(!srcImage.IsNull() && srcImage != dstImage && srcImage.GetBPP() == 8);

		if (!dstImage.IsNull())     dstImage.Destroy();
		dstImage.Create(srcImage.GetWidth(), srcImage.GetHeight(), 32, CImage::createAlphaChannel);
		
		// CImage::BitBlt won't work with alpha channel, we have to set pixel one by one
		for (int row = 0; row < dstImage.GetHeight(); row++)
		{
			for (int col = 0; col < dstImage.GetWidth(); col++)
			{
				BYTE* pixelEntry = static_cast<BYTE*>(dstImage.GetPixelAddress(col, row));
				const BYTE intensity = static_cast<const BYTE*>(srcImage.GetPixelAddress(col, row))[0];
				pixelEntry[0] = intensity;
				pixelEntry[1] = intensity;
				pixelEntry[2] = intensity;
				pixelEntry[3] = 0xFF;
			}
		}
	}


} // End of namespace slcimage
