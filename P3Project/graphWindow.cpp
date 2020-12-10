#include "graphWindow.h"

#include <iostream>
#include <string>

void fatalError(std::string errorString)
{
	std::cout << errorString << std::endl;
	std::cout << "Enter any key to quit.....";
	int tmp;
	std::cin >> tmp;
	SDL_Quit();
}

graphWindow::graphWindow()
{
	_window = nullptr;
	_screenWidth = 768;
	_screenHeight = 768;
	_graphName = "noName";
	_graphState = graphState::ON;
	_mX = 0;
	_mY = 0;
	_samplingOnset = 0.1;
}

graphWindow::~graphWindow()
{

}

void graphWindow::setGraphName(const char* graphName)
{
	_graphName = graphName;
}

void graphWindow::run()
{
	initSystems();
	//graphLoop();
}

void graphWindow::initSystems()
{
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	//Create new window
	_window = SDL_CreateWindow(_graphName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
	if (_window == nullptr)
	{
		fatalError("SDL Window could not be created!");
	}
	//Start GL Context for new window
	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
	if (glContext == nullptr)
	{
		fatalError("SDL_GL context could not be created!");
	}
	//Initialize Glew
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		fatalError("Could not initialize glew!");
	}
	//Stops window from flickering
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Set window color to blue
	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void graphWindow::processInput()
{
	SDL_Event evnt;

	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_graphState = graphState::EXIT;
			break;
		default:
			break;
		}
	}
}

int graphWindow::getGraphState()
{
	if (_graphState == graphState::EXIT) return 0;
	else return 1;
}

void graphWindow::beginDrawLine()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapWindow(_window);
	glEnableClientState(GL_COLOR_ARRAY);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			float xGrid = i;
			float yGrid = j;

			glVertex2f(1-(xGrid/10), -1.0f);
			glVertex2f(1 - (xGrid / 10), 1.0f);

			if (j == 10)
			{
				glColor3f(0, 0, 1.0f);
				glVertex2f(-1.0f, 1 - (yGrid / 10));
				glVertex2f(1.0f, 1 - (yGrid / 10));
				glColor3f(1.0f, 1.0f, 1.0f);
			}
			else
			{
				glVertex2f(-1.0f, 1 - (yGrid / 10));
				glVertex2f(1.0f, 1 - (yGrid / 10));
			}
		}
	}
	glColor3f(1.0f, 0.0f, 0.0f);
}	

float graphWindow::drawLine(float time, float prevValue, float currValue)
{
	glVertex2f(time, prevValue);
	time = time + _samplingOnset;
	glVertex2f(time, currValue);
	return time;
}

float graphWindow::shiftDrawLine(float prevValue, float currValue)
{
	float time;
	glEnd();
	SDL_GL_SwapWindow(_window);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_COLOR_ARRAY);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			float xGrid = i;
			float yGrid = j;

			
			glVertex2f(1 - (xGrid / 10), -1.0f);
			glVertex2f(1 - (xGrid / 10), 1.0f);
			

			if (j == 10)
			{
				glColor3f(0, 0, 1.0f);
				glVertex2f(-1.0f, 1 - (yGrid / 10));
				glVertex2f(1.0f, 1 - (yGrid / 10));
				glColor3f(1.0f, 1.0f, 1.0f);
			}
			else
			{
				glVertex2f(-1.0f, 1 - (yGrid / 10));
				glVertex2f(1.0f, 1 - (yGrid / 10));
			}

		}
	}
	glColor3f(1.0f, 0.0f, 0.0f);
	time = drawLine(-1.0f, prevValue, currValue);
	return time;
}

void graphWindow::endDrawLine()
{
	glEnd();
	SDL_GL_SwapWindow(_window);
}