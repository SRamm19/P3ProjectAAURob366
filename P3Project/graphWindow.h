#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <Windows.h>

enum class graphState {ON, EXIT};

class graphWindow
{
public:
	graphWindow();
	~graphWindow();
	//Outputs the graph is turned on, or if the EXIT button has been pressed
	int getGraphState();
	//Initialises SDL and Glew, as well as opens a new graph window.
	void run();
	//Specify which name is at the top of the graph window.
	void setGraphName(const char*);
	//Checks if the EXIT button of the graph has been pressed
	void processInput();
	//Draw a line from a point (time;prevValue) to another (time+samplingOnset;currValue). Note: Requires beginDrawLine() to have been called before it
	float drawLine(float time, float prevValue, float currValue);
	//Begins a new DrawLine loop
	void beginDrawLine();
	//End the current DrawLine loop, Resets the Graph window and begins a new DrawLine loop
	float shiftDrawLine(float prevValue, float currValue);
	//End the current DrawLine Loop
	void endDrawLine();
		
private:
	void initSystems();

	SDL_Window* _window;
	int _screenWidth;
	int _screenHeight;
	const char*  _graphName;
	graphState _graphState;
	POINT _cursorPos;
	int _mX;
	int _mY;
	float _samplingOnset;
};

