#ifndef __SLGLFW_01_TEXTURE__
#define __SLGLFW_01_TEXTURE__
#pragma once

#include "SLAbstractGLFW_GLEW.h"                 // Virtual GLFW_GLEW class
#include "Paintables\SLLibreImagePaintable.h"    // slgeom: LibreImage to load texture

/// <remark> std::unique_ptr require SLTexture2D_Renderer to be declared in header,
///        otherwise any other class that is trying to instance SLGLFW_01_Texture will need to include this header again </remark>
#include "Rendering/SLTexture2D_Renderer.h" 


class SLGLFW_01_Texture : public SLAbstractGLFW_GLEW
{
public:
	SLGLFW_01_Texture();

protected:
	void InitPaintContext() override;
	void PaintGL() override;
	void FinalizeGL() override;
	void PostPaintProcess() override;

private:
	// Relative Address start from the project file .proj where instance SLTexture2D_Renderer, which is now in folder SLUnit_VisualTest
	const TCHAR* m_ImagePath = _T("../../WatchMe/Images/grass.png");//Einstein 8bit.jpg   Einstein.jpg    grass.png   Google24Bit225.jpg
	std::wstring m_SaveFolderPath = _T("../../../../../Developer/Processed Images/");

	sldip::SLLibreImage m_LibreImage{};                     // For PostPaintProcess 
	slgeom::SLLibreImagePaintable m_LibreImagePaintable{};  // For Texture Paint

	/// <summary>One global renderer that can handle all texture renderings </summary>
	std::unique_ptr<slgeom::SLTexture2D_Renderer> m_Texture2DRenderer = nullptr;


	void LoadTexture();
};

#endif // __SLGLFW_01_TEXTURE__