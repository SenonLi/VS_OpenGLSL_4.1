#pragma once

#ifndef __SenAbstractGLFW__
#define __SenAbstractGLFW__

#include <stdlib.h>
#include <stdio.h>  
#include <iostream>
#include <Windows.h>
#include <string>

#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include "LoadShaders.h"

#define SenGL_CheckError() SenGL_CheckError_(__FILE__, __LINE__)
#define SenGL_MajorVersion 4
#define SenGL_MinorVersion 3

class SenAbstractGLFW
{
public:
	SenAbstractGLFW();
	virtual ~SenAbstractGLFW();

	void ShowWidget();

	void _protectedKeyDetection(GLFWwindow* widget, int key, int scancode, int action, int mode) { 
		keyDetection(widget, key, scancode, action, mode);
	}

protected:
	GLFWwindow* widgetGLFW;
	GLint m_WidgetWidth, m_WidgetHeight;
	std::string strWindowName;
	float xRot, yRot;
	float aspect;

	virtual void initGlfwGlewGL();
	virtual void paintGL();
	virtual void finalize() { ; }

	virtual void keyDetection(GLFWwindow* widget, int key, int scancode, int action, int mode);


	// shader info variables
	GLuint programA, programB;
	GLuint defaultTextureID;
	GLint textureLocation;

	GLuint verArrObjArray[2];
	GLuint verBufferObjArray[2];
	GLuint verIndicesObjArray[2];

	GLint modelMatrixLocation;
	GLint projectionMatrixLocation;


	const GLint DEFAULT_widgetWidth = 640;
	const GLint DEFAULT_widgetHeight = 640;

private:
	void keyboardRegister();

};


GLenum SenGL_CheckError_(const char *file, int line);

void APIENTRY SenGL_DebugOutput(GLenum source,	GLenum type,	GLuint id,
		GLenum severity,	GLsizei length,	const GLchar *message,	const void *userParam);


#endif //__SenAbstractGLFW__