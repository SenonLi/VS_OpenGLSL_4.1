#include "SenDebugWindowFreeSpace.h"


SenDebugWindowFreeSpace::SenDebugWindowFreeSpace()
{
	strWindowName = "Sen DebugWindow FreeSpace";
}

SenDebugWindowFreeSpace::~SenDebugWindowFreeSpace() {}


void SenDebugWindowFreeSpace::initGlfwGlewGL()
{
	SenFreeSpaceAbstract::initGlfwGlewGL();

	//*****   Initial Debug Window   *****************************************************************************************************
	initDebugWindowProgram();
	initDebugWindowVertexAttributes();
	initDebugWindowFrameBuffer();


	OutputDebugString(" Sen FrameBuffer FreeSpace Initial \n\n");
}

void SenDebugWindowFreeSpace::paintFreeSpaceGL(void)
{
	// ======== Render Customer FrameBuffer =================================================================
	glBindFramebuffer(GL_FRAMEBUFFER, debugWindowFrameBufferObject);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer so why bother with clearing?

	// Get Rear CameraView
	camera.Front = -camera.Front;
	paintScene();

	// ======== Render Normal Screen =============================================================
	camera.Front = -camera.Front; // Recover front CameraView
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	paintScene();

	// ======== End of Normal Rendering =============================================================
	// Bind to default framebuffer again and draw the quad plane with attched screen texture.

	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

	// Paint DebugWindow Outline
	glUseProgram(debugWindowOutlineProgram);
	glBindVertexArray(debugWindowOutlineVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw DebugWindow
	glUseProgram(debugWindowProgram);
	glBindVertexArray(debugWindowVAO);
	glBindTexture(GL_TEXTURE_2D, debugWindowRGB_TextureAttach);	// Use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}


void SenDebugWindowFreeSpace::initDebugWindowFrameBuffer()
{
	glGenFramebuffers(1, &debugWindowFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, debugWindowFrameBufferObject);
	
	// Color Texture
	debugWindowRGB_TextureAttach = generateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, debugWindowRGB_TextureAttach, 0);
	
	// Depth + Stencil RenderBuffer
	glGenRenderbuffers(1, &debugWindowDepthStencil_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, debugWindowDepthStencil_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, widgetWidth, widgetHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, debugWindowDepthStencil_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// Check Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error::Framebuffer:: Framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   // Recover Default Framebuffer (Main Screen)
}

void SenDebugWindowFreeSpace::initDebugWindowVertexAttributes()
{
	/******** DebugWindow Outline Quad *********/
	GLfloat debugWindowOutlineVertices[] = {
		// Positions																	// TexCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f + 2 * SenGOLDEN_SectionScale, 0.0f, 0.0f,
		1.0f - 2 * SenGOLDEN_SectionScale, -1.0f + 2 * SenGOLDEN_SectionScale, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f - 2 * SenGOLDEN_SectionScale, -1.0f + 2 * SenGOLDEN_SectionScale, 1.0f, 0.0f,
		1.0f - 2 * SenGOLDEN_SectionScale, 1.0f, 1.0f, 1.0f
	};

	/******** DebugWindow  Quad *********/
	GLfloat OutlineToWindowScale = SenGOLDEN_SectionScale * 1.001;
	GLfloat debugWindowVertices[] = {
		// Positions												// TexCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f + 2 * OutlineToWindowScale, 0.0f, 0.0f,
		1.0f - 2 * OutlineToWindowScale, -1.0f + 2 * OutlineToWindowScale, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f - 2 * OutlineToWindowScale, -1.0f + 2 * OutlineToWindowScale, 1.0f, 0.0f,
		1.0f - 2 * OutlineToWindowScale, 1.0f, 1.0f, 1.0f
	};

	// ****** Setup DebugWindow Outline Quad,  debugWindowOutlineVAO ****************************/
	glGenVertexArrays(1, &debugWindowOutlineVAO);
	glGenBuffers(1, &debugWindowOutlineVBO);

	glBindVertexArray(debugWindowOutlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugWindowOutlineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(debugWindowOutlineVertices), debugWindowOutlineVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ****** Setup DebugWindow Quad,  debugWindowVAO ****************************/
	glGenVertexArrays(1, &debugWindowVAO);
	glGenBuffers(1, &debugWindowVBO);

	glBindVertexArray(debugWindowVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugWindowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(debugWindowVertices), debugWindowVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0);
}

void SenDebugWindowFreeSpace::initDebugWindowProgram()
{
	//ShaderInfo shaders[] = {
	//	{ GL_VERTEX_SHADER, "./LearnOpenGL_GLFW/Shaders/Sen_22_DepthTest.vert" },
	//	{ GL_FRAGMENT_SHADER, "./LearnOpenGL_GLFW/Shaders/Sen_22_DepthTest.frag" },
	//	{ GL_NONE, NULL }
	//};

	ShaderInfo shadersScreenTexture[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_26_PostProcessing.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_23_StencilObjectOutline.frag" },
		{ GL_NONE, NULL }
	};

	ShaderInfo shadersScreenSingular[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_26_PostProcessing.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_ColorRed.frag" },
		{ GL_NONE, NULL }
	};

	debugWindowProgram = LoadShaders(shadersScreenTexture);
	debugWindowOutlineProgram = LoadShaders(shadersScreenSingular);
}

void SenDebugWindowFreeSpace::cleanFreeSpace(void)	{

	cleanDebugWindowFreeSpace();

	// finalize Debug Window Vertex Attributes
	if (glIsProgram(debugWindowProgram))				glDeleteProgram(debugWindowProgram);
	if (glIsProgram(debugWindowOutlineProgram))			glDeleteProgram(debugWindowOutlineProgram);
	if (glIsVertexArray(debugWindowVAO))				glDeleteVertexArrays(1, &debugWindowVAO);
	if (glIsVertexArray(debugWindowVBO))				glDeleteVertexArrays(1, &debugWindowVBO);
	if (glIsBuffer(debugWindowVBO))						glDeleteBuffers(1, &debugWindowVBO);	
	if (glIsBuffer(debugWindowOutlineVBO))				glDeleteBuffers(1, &debugWindowOutlineVBO);
	
	// finalize FrameBuffer 
	if (glIsTexture(debugWindowRGB_TextureAttach))		glDeleteTextures(1, &debugWindowRGB_TextureAttach);
	if (glIsFramebuffer(debugWindowFrameBufferObject))	glDeleteFramebuffers(1, &debugWindowFrameBufferObject);
	if (glIsRenderbuffer(debugWindowDepthStencil_RBO))	glDeleteRenderbuffers(1, &debugWindowDepthStencil_RBO);
}