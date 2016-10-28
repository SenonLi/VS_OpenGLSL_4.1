#include "Sen_BreakOut.h"


Sen_BreakOut::Sen_BreakOut()
	: breakOutLevel(0)
	, ballVelocity(glm::vec2(0.0f, 0.0f)), isBallStuckOnPlayerBoard(GL_TRUE)
{
	strWindowName = "Sen BreakOut in 2D GameSpace";
}


Sen_BreakOut::~Sen_BreakOut()
{
}

void Sen_BreakOut::init2DGamePaceGL()	{

	initShaderPrograms();
	initVertexAttributes();
	initTextures();
	initBrickMapsVector();
	initBallVariables();

	//OutputDebugString(" After BrickMapsVector Initial \n\n");
}

void Sen_BreakOut::initBallVariables()	{

	isBallStuckOnPlayerBoard = GL_TRUE;
	ballVelocity = originalBallVELOCITY;
	ballSpinAngleDegree = 0.0f;
	ballSpinSpeed = originalBallSpinSpeed;
	ballPosition = originalBallPOSITION;
}

void Sen_BreakOut::initBrickMapsVector()	{

	GLfloat spareSide = getCubeLogoOccupancySquareSide();
	std::vector<std::vector<GLuint>> map2DBrickTypesinfo;

	init2DMapInfo(map2DBrickTypesinfo, 1);
	Sen_BreakOutMap levelOneMap(spareSide, map2DBrickTypesinfo);
	brickMapsVector.push_back(levelOneMap);

	init2DMapInfo(map2DBrickTypesinfo, 2);
	Sen_BreakOutMap levelTwoMap(spareSide, map2DBrickTypesinfo);
	brickMapsVector.push_back(levelTwoMap);

	init2DMapInfo(map2DBrickTypesinfo, 3);
	Sen_BreakOutMap levelThirdMap(spareSide, map2DBrickTypesinfo);
	brickMapsVector.push_back(levelThirdMap);

	//OutputDebugString(" After brickMapsVector.push_back Initial \n\n");
}

Sen_BreakOutMap::Sen_BreakOutMap(GLfloat spareSide, std::vector<std::vector<GLuint>> &map2DBrickTypesinfo) {
	bricksVector.clear();

	GLuint totalLinesNum = map2DBrickTypesinfo.size();
	if (totalLinesNum)	{
		GLuint lineBricksNum = map2DBrickTypesinfo.at(0).size();

		GLfloat brickWidth = 2.0 / static_cast<GLfloat>(lineBricksNum);
		GLfloat brickHeight = 1.0 / static_cast<GLfloat>(totalLinesNum);

		GLuint incompleteLinesNum = std::ceil(spareSide / brickHeight);
		GLuint incompleteLineBricksNum = std::floor((2.0 - spareSide) / brickWidth);

		glm::vec2 brickSize(brickWidth, brickHeight);
		glm::vec2 brickPosition = glm::vec2(0.0f);
		glm::vec3 brickColor = glm::vec3(1.0f); // Initial: white, none for blending

		for (GLuint i = 0; i < totalLinesNum; i++)	{
			// Initial incomplete Lines of Bricks
			GLuint lineBricksNumMax = i < incompleteLinesNum ? incompleteLineBricksNum : lineBricksNum;

			for (GLuint j = 0; j < lineBricksNumMax; j++)	{

				if (map2DBrickTypesinfo.at(i).at(j) > 0)	{
					GLboolean isBrickSolid = map2DBrickTypesinfo.at(i).at(j) == 1 ? GL_TRUE : GL_FALSE;

					brickPosition.x = -1.0 + (static_cast<GLfloat>(j)+0.5) * brickWidth;
					brickPosition.y = 1.0 - (static_cast<GLfloat>(i)+0.5) * brickHeight;

					if (map2DBrickTypesinfo.at(i).at(j) == 1)			brickColor = glm::vec3(1.0f);
					else if (map2DBrickTypesinfo.at(i).at(j) == 2)		brickColor = glm::vec3(0.8f, 0.8f, 0.4f);
					else if (map2DBrickTypesinfo.at(i).at(j) == 3)		brickColor = glm::vec3(1.0f, 0.5f, 0.0f);
					else if (map2DBrickTypesinfo.at(i).at(j) == 4)		brickColor = glm::vec3(0.0f, 0.7f, 0.0f);
					else if (map2DBrickTypesinfo.at(i).at(j) == 5)		brickColor = glm::vec3(0.9f, 0.6f, 0.9f);

					bricksVector.push_back(Sen_2D_BlockBrick(brickPosition, brickSize, brickColor, isBrickSolid));
				}
			}

		}
	}	else std::cout << "totalLinesNum == 0 !!";
}

void Sen_BreakOut::initShaderPrograms(){

	ShaderInfo shadersScreenTextureCoords[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_ScreenTextureCoords.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_TextureCoords.frag" },
		{ GL_NONE, NULL }
	};
	programA = LoadShaders(shadersScreenTextureCoords);

	ShaderInfo shaders2DTextureCoords[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_2D_TextureCoords.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_PlayerColorRendering.frag" },
		{ GL_NONE, NULL }
	};
	programB = LoadShaders(shaders2DTextureCoords);

	ShaderInfo shaders2DTextureCoordsRender[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_2D_TextureCoords.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_TextureCoordsRender.frag" },
		{ GL_NONE, NULL }
	};
	blendUnitOneSquareProgram = LoadShaders(shaders2DTextureCoordsRender);

	ShaderInfo shaders2DTextureCoordsDiscardWhite[] = {
		{ GL_VERTEX_SHADER, "./../WatchMe/Shaders/Sen_2D_TextureCoords.vert" },
		{ GL_FRAGMENT_SHADER, "./../WatchMe/Shaders/Sen_DiscardPureWhitePixels.frag" },
		{ GL_NONE, NULL }
	};
	ballProgram = LoadShaders(shaders2DTextureCoordsDiscardWhite);
}

void Sen_BreakOut::initTextures(){
	uploadTextureImage(std::string("./../WatchMe/Images/SunRaise.jpg").c_str(), backgroundTexture, "RGBA", GL_TRUE);
	uploadTextureImage(std::string("./../WatchMe/Images/paddle.png").c_str(), playerBoardTexture, "RGBA", GL_TRUE);

	uploadTextureImage(std::string("./../WatchMe/Images/block.png").c_str(), blockTexture, "RGB", GL_TRUE);
	uploadTextureImage(std::string("./../WatchMe/Images/block_solid.png").c_str(), solidBlockTexture, "RGB", GL_TRUE);

	uploadTextureImage(std::string("./../WatchMe/Images/ball.jpg").c_str(), ballTexture, "RGBA", GL_TRUE);
}

void Sen_BreakOut::initBackgroundVertices(){

	GLfloat backgroundVertexAttributes[] = {
		// Positions            // Texture Coords 
		1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &backgroundVAO);
	glGenBuffers(1, &backgroundVBO);

	glBindVertexArray(backgroundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertexAttributes), backgroundVertexAttributes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sen_BreakOut::initPlayerVertices(){

	GLfloat width = originalPlayerBOARDWIDTH / 2.0, height = originalPlayerBOARDHEIGHT / 2.0;
	GLfloat playerBoardVertexAttributes[] = {
		// Positions            // Texture Coords 
		width, height, 1.0f, 0.0f,
		-width, height, 0.0f, 0.0f,
		-width, -height, 0.0f, 1.0f,

		width, height, 1.0f, 0.0f,
		-width, -height, 0.0f, 1.0f,
		width, -height, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &playerBoardVAO);
	glGenBuffers(1, &playerBoardVBO);

	glBindVertexArray(playerBoardVAO);
	glBindBuffer(GL_ARRAY_BUFFER, playerBoardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(playerBoardVertexAttributes), playerBoardVertexAttributes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	playerBoardWidth = originalPlayerBOARDWIDTH;
	playerBoardPosition = originalPlayerBoardPOSITION;
}

void Sen_BreakOut::initVertexAttributes(){

	initBackgroundVertices();
	initPlayerVertices();
	initUnitOneSquareVertices();
}

void Sen_BreakOut::initUnitOneSquareVertices(){
	GLfloat unitOneSquareVertexAttributes[] = {
		// Positions            // Texture Coords 
		0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,

		0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &unitOneSquareVAO);
	glGenBuffers(1, &unitOneSquareVBO);

	glBindVertexArray(unitOneSquareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, unitOneSquareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unitOneSquareVertexAttributes), unitOneSquareVertexAttributes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sen_BreakOut::paint2DGameSpaceGL()	{
	glDisable(GL_DEPTH_TEST);

	paintBackground();

	// Alpha blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	paintBricksMap();
	paintPlayer();
	paintBall();

	glDisable(GL_BLEND);
}

void Sen_BreakOut::paintBricksMap()	{

	glUseProgram(blendUnitOneSquareProgram);
	glBindVertexArray(unitOneSquareVAO);
	glActiveTexture(GL_TEXTURE0);

	GLuint totalBricksNum = brickMapsVector.at(breakOutLevel).bricksVector.size();
	for (GLuint i = 0; i < totalBricksNum; i++)	{
		if (brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickNotDestroyedStatus())	{
			if (brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickIsSolidStatus())
				glBindTexture(GL_TEXTURE_2D, solidBlockTexture);
			else  glBindTexture(GL_TEXTURE_2D, blockTexture);

			model = glm::mat4();
			model = glm::translate(	model, 
				glm::vec3(brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickPosition(), 
				0.0f)	
				);
			model = glm::scale(model, 
				glm::vec3(brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickSize(),
				1.0f)
				); // Last scale

			glUniformMatrix4fv(glGetUniformLocation(blendUnitOneSquareProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glUniform3f(glGetUniformLocation(blendUnitOneSquareProgram, "renderColor"),
				brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickColor().x,
				brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickColor().y,
				brickMapsVector.at(breakOutLevel).bricksVector.at(i).getBrickColor().z
				);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Sen_BreakOut::paintBall()	{

	glUseProgram(ballProgram);
	glBindVertexArray(unitOneSquareVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ballTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(ballPosition, 0.0f));
	model = glm::scale(model,
		glm::vec3(ballSquareSIDE * widgetHeight / widgetWidth, ballSquareSIDE, 1.0f)
		); // Last scale
	model = glm::rotate(model, static_cast<GLfloat>(glm::radians(ballSpinAngleDegree)), glm::vec3(0.0f, 0.0f, 1.0f));


	glUniformMatrix4fv(glGetUniformLocation(ballProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(glGetUniformLocation(ballProgram, "renderColor"),
		1.0f, 0.0f, 0.0f);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Sen_BreakOut::paintPlayer()	{

	glUseProgram(programB);
	glBindVertexArray(playerBoardVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, playerBoardTexture);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(playerBoardPosition, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(programB, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	glUniform3f(glGetUniformLocation(programB, "renderColor"),
		0.5f, 0.5f, 0.5f);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Sen_BreakOut::paintBackground()	{

	glUseProgram(programA);
	glBindVertexArray(backgroundVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Sen_BreakOut::clean_2D_GrameFrame()	{
	if (glIsVertexArray(backgroundVAO))		glDeleteVertexArrays(1, &backgroundVAO);
	if (glIsBuffer(backgroundVBO))			glDeleteBuffers(1, &backgroundVBO);
	if (glIsTexture(backgroundTexture))		glDeleteTextures(1, &backgroundTexture);

	if (glIsVertexArray(playerBoardVAO))		glDeleteVertexArrays(1, &playerBoardVAO);
	if (glIsBuffer(playerBoardVBO))			glDeleteBuffers(1, &playerBoardVBO);
	if (glIsTexture(playerBoardTexture))		glDeleteTextures(1, &playerBoardTexture);

	if (glIsVertexArray(unitOneSquareVAO))		glDeleteVertexArrays(1, &unitOneSquareVAO);
	if (glIsBuffer(unitOneSquareVBO))			glDeleteBuffers(1, &unitOneSquareVBO);
	if (glIsTexture(blockTexture))			glDeleteTextures(1, &blockTexture);
	if (glIsTexture(solidBlockTexture))		glDeleteTextures(1, &solidBlockTexture);
	if (glIsTexture(ballTexture))		glDeleteTextures(1, &ballTexture);

	if (glIsProgram(programA))					glDeleteProgram(programA);
	if (glIsProgram(programB))					glDeleteProgram(programB);
	if (glIsProgram(blendUnitOneSquareProgram))	glDeleteProgram(blendUnitOneSquareProgram);
	if (glIsProgram(ballProgram))				glDeleteProgram(ballProgram);
}


void Sen_BreakOut::daltaTimeUpdate(GLfloat deltaTime)	{
	//if (this->State == GAME_ACTIVE)
	//{
	GLfloat palyerShift = PLAYER_VELOCITY * deltaTime;

	// ******************* Key Input Process*************************************
	if (keys[GLFW_KEY_LEFT])
	{
		playerBoardPosition.x -= palyerShift;
		GLfloat xLeftBorder = -1.0f + playerBoardWidth / 2.0;
		if (playerBoardPosition.x < xLeftBorder)
			playerBoardPosition.x = xLeftBorder;

		if (isBallStuckOnPlayerBoard)	ballPosition.x = playerBoardPosition.x;
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		playerBoardPosition.x += palyerShift;
		GLfloat xRightBorder = 1.0f - playerBoardWidth / 2.0;
		if (playerBoardPosition.x > xRightBorder)
			playerBoardPosition.x = xRightBorder;
		
		if (isBallStuckOnPlayerBoard)	ballPosition.x = playerBoardPosition.x;
	}
	if (keys[GLFW_KEY_SPACE])		isBallStuckOnPlayerBoard = GL_FALSE;


	// ************ Ball Movement ***********************************************
	if (!isBallStuckOnPlayerBoard)
	{
		// Move the ball
		ballSpinAngleDegree += ballSpinSpeed * deltaTime;
		normalizeAngleDegree(ballSpinAngleDegree);

		ballPosition += ballVelocity * deltaTime;
		GLfloat ballRadiusWidth = ballRADIUS * widgetHeight / widgetWidth;

		if (ballPosition.x <= -1.0f + ballRadiusWidth)
		{
			ballVelocity.x = -ballVelocity.x;
			ballPosition.x = -1.0f + ballRadiusWidth;

			ballSpinSpeed = -ballSpinSpeed;
		}
		else if (ballPosition.x >= 1.0f - ballRadiusWidth)
		{
			ballVelocity.x = -ballVelocity.x;
			ballPosition.x = 1.0f - ballRadiusWidth;

			ballSpinSpeed = -ballSpinSpeed;
		}

		if (ballPosition.y >= 1.0f - ballRADIUS)
		{
			ballVelocity.y = -ballVelocity.y;
			ballPosition.y = 1.0f - ballRADIUS;

			ballSpinSpeed = -ballSpinSpeed;
		}
	}

	bricksCollisionKill();
	//}
}



void Sen_BreakOut::init2DMapInfo(std::vector<std::vector<GLuint>> &map2DBrickTypesinfo, GLuint breakOutLevel)	{

	GLuint levelOneBrickTypesMap[][15] = {
		{ 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
		{ 1, 3, 3, 2, 3, 2, 3, 2, 3, 3, 2, 3, 3, 1, 1 },
		{ 1, 2, 3, 3, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 1 },
		{ 1, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 1 },
		{ 1, 3, 2, 3, 2, 3, 2, 3, 3, 2, 3, 3, 2, 3, 1 },
		{ 1, 0, 2, 2, 0, 2, 2, 0, 2, 2, 0, 2, 2, 0, 1 },
		{ 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1 }
	};

	GLuint levelTwoBrickTypesMap[][17] = {
		{ 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4 },
		{ 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 },
		{ 3, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 2 },
		{ 2, 4, 1, 4, 1, 4, 0, 0, 1, 0, 0, 4, 1, 4, 1, 4, 3 },
		{ 1, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 4 },
		{ 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3 },
		{ 4, 1, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 1, 5 },
		{ 2, 3, 3, 1, 3, 2, 3, 4, 5, 5, 4, 3, 3, 1, 3, 3, 5 },
	};

	GLuint levelThirdBrickTypesMap[][19] = {
		{ 4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 5 },
		{ 4, 2, 3, 3, 1, 3, 2, 3, 4, 5, 5, 4, 3, 3, 1, 3, 3, 5, 5 },
		{ 4, 3, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 2, 5 },
		{ 4, 2, 3, 3, 1, 3, 2, 3, 4, 5, 5, 4, 3, 3, 1, 3, 3, 5, 5 },
		{ 4, 2, 4, 1, 4, 1, 4, 0, 0, 1, 0, 0, 4, 1, 4, 1, 4, 3, 5 },
		{ 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5 },
		{ 4, 2, 3, 3, 1, 3, 2, 3, 4, 5, 5, 4, 3, 3, 1, 3, 3, 5, 5 },
		{ 4, 1, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 4, 5 },
		{ 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 5 },
		{ 4, 4, 1, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 3, 3, 0, 1, 5, 5 },
	};

	map2DBrickTypesinfo.clear();

	if (breakOutLevel == 1)	{
		GLuint rowNum = sizeof(levelOneBrickTypesMap) / sizeof(levelOneBrickTypesMap[0]);
		for (int i = 0; i < rowNum; i++)	{
			GLuint columnNum = sizeof(levelOneBrickTypesMap[i]) / sizeof(GLuint);
			std::vector<GLuint> lineBrickTypesInfo;
			for (int j = 0; j < columnNum; j++)	{
				lineBrickTypesInfo.push_back(levelOneBrickTypesMap[i][j]);
			}
			map2DBrickTypesinfo.push_back(lineBrickTypesInfo);
		}
	}
	else if (breakOutLevel == 2)	{
		GLuint rowNum = sizeof(levelTwoBrickTypesMap) / sizeof(levelTwoBrickTypesMap[0]);
		for (int i = 0; i < rowNum; i++)	{
			GLuint columnNum = sizeof(levelTwoBrickTypesMap[i]) / sizeof(GLuint);
			std::vector<GLuint> lineBrickTypesInfo;
			for (int j = 0; j < columnNum; j++)	{
				lineBrickTypesInfo.push_back(levelTwoBrickTypesMap[i][j]);
			}
			map2DBrickTypesinfo.push_back(lineBrickTypesInfo);
		}
	}
	else if (breakOutLevel == 3)	{
		GLuint rowNum = sizeof(levelThirdBrickTypesMap) / sizeof(levelThirdBrickTypesMap[0]);
		for (int i = 0; i < rowNum; i++)	{
			GLuint columnNum = sizeof(levelThirdBrickTypesMap[i]) / sizeof(GLuint);
			std::vector<GLuint> lineBrickTypesInfo;
			for (int j = 0; j < columnNum; j++)	{
				lineBrickTypesInfo.push_back(levelThirdBrickTypesMap[i][j]);
			}
			map2DBrickTypesinfo.push_back(lineBrickTypesInfo);
		}
	}	else std::cout << "breakOutLevel Wrong !!";
}


GLboolean Sen_BreakOut::checkBrickCollision(const Sen_2D_BlockBrick &brick)
{
	// AABB - AABB collision
	GLfloat ballRadiusWidth = ballRADIUS * widgetHeight / widgetWidth;

	bool isCollidedX = brick.getBrickPosition().x + brick.getBrickSize().x / 2.0f >= ballPosition.x - ballRadiusWidth
		&& brick.getBrickPosition().x - brick.getBrickSize().x / 2.0f <= ballPosition.x + ballRadiusWidth;

	bool isCollidedY = brick.getBrickPosition().y + brick.getBrickSize().y / 2.0f >= ballPosition.y - ballRADIUS
		&& brick.getBrickPosition().y - brick.getBrickSize().y / 2.0f <= ballPosition.y + ballRADIUS;

	return isCollidedX && isCollidedY;
}

void Sen_BreakOut::bricksCollisionKill()
{
	GLfloat ballRadiusWidth = ballRADIUS * widgetHeight / widgetWidth;

	// Check Bricks + LogoCube Collision
	if (ballPosition.y >= 0.0f - ballRadiusWidth)	{

		for (Sen_2D_BlockBrick &brick : brickMapsVector.at(breakOutLevel).bricksVector)
		{
			if (brick.getBrickNotDestroyedStatus())
			{
				if (checkBrickCollision(brick))
				{
					if (!brick.getBrickIsSolidStatus())
						brick.setBrickNotDestroyStatus(GL_FALSE);
				}
			}
		}

	} 
	// Check playerBoard Collision
	else if (ballPosition.y <= -1.0f + originalPlayerBOARDHEIGHT + ballRadiusWidth)		{

	}
}
