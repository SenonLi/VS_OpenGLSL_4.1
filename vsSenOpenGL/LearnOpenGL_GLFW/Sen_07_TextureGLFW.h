#pragma once
#include "SenAbstractGLFW.h"

#include <SOIL.h>


class Sen_07_TextureGLFW :	public SenAbstractGLFW
{
public:
	Sen_07_TextureGLFW();
	virtual ~Sen_07_TextureGLFW();

protected:
	void paintGL(void);
	void initialGlfwGlewGL();
	void finalize();

};

