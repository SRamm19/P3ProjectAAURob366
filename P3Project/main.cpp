#include <Windows.h>

#include <iostream> 
//#include <string>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <dynamixel_sdk/dynamixel_sdk.h>
#include <myo/myo.hpp>

#include "graphWindow.h"
#include "dynamixelControl.h"
#include "DataCollector.h"

#define DXL1_MINIMUM_POSITION_VALUE     1800             // Dynamixel will rotate between this value
#define DXL1_MAXIMUM_POSITION_VALUE     3000              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL23_MINIMUM_POSITION_VALUE    750             // Dynamixel 2 and 3 will rotate between this value
#define DXL23_MAXIMUM_POSITION_VALUE    2322              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_CLOSED_GRIPPER_VALUE        1950                 //Value for the position of the dynamixel to close,
#define DXL_OPEN_GRIPPER_VALUE          1500                 //and open the gripper


#define SCREEN_WIDTH                    1535
#define SCREEN_HEIGHT                   863

using namespace std;

double cursorConverter(int cursorPos, double screenValue, double DXL_Max, double DXL_Min);		//function to convert cursor position value, into numbers readable by dxlmotor
int rollCalculator(int rollAmount, int rollOffset);

int main(int argc, char** argv)
{
	myo::Hub hub("Does.This.Mean.Anything");
	std::cout << "Attempting to find a Myo..." << std::endl;
	myo::Myo* myo = hub.waitForMyo(10000);
	std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
	DataCollector collector;
	hub.addListener(&collector);

	float prevTorque = 0.000f;
	float torque = prevTorque;
	float time = -1.000f;

	bool dontReapeatPlacement = false;

	POINT cursorPos;
	int mX = SCREEN_WIDTH-(((2048 - DXL1_MINIMUM_POSITION_VALUE) * SCREEN_WIDTH) / (DXL1_MAXIMUM_POSITION_VALUE - DXL1_MINIMUM_POSITION_VALUE));
	int mY = SCREEN_HEIGHT-((2048 - DXL23_MINIMUM_POSITION_VALUE) * SCREEN_HEIGHT) / (DXL23_MAXIMUM_POSITION_VALUE - DXL23_MINIMUM_POSITION_VALUE);

	int roll_nullifier;
	int dm3Offset;

	int gripperPos;
	
	dynamixelControl controlDynamixel;
	controlDynamixel.runDynamixelSetup();
	gripperPos = DXL_OPEN_GRIPPER_VALUE;
	
	graphWindow torqueGraph;
	torqueGraph.setGraphName("Torque Graph");	
	torqueGraph.run();

	torqueGraph.beginDrawLine();

	while (torqueGraph.getGraphState()) //Loops the code, until the graph is closed
	{
		hub.run(1000 / 20);
		collector.GripperPose();
		if (GetKeyState(VK_CAPITAL))
		{
			if (!dontReapeatPlacement) //Return cursor to last position
			{
				dontReapeatPlacement = true;
				SetCursorPos(mX, mY);
				roll_nullifier = collector.roll_w;
			}
			
			GetCursorPos(&cursorPos);	//Read Cursor Position
			if (cursorPos.x != mX || cursorPos.y != mY)
			{
				mX = cursorPos.x;
				mY = cursorPos.y;
				dm3Offset = rollCalculator(collector.roll_w, roll_nullifier);
				//std::cout << mX << " , " << mY << std::endl;
				dm3Offset = 0;
				int dm1 = cursorConverter(SCREEN_WIDTH - mX, SCREEN_WIDTH, DXL1_MAXIMUM_POSITION_VALUE, DXL1_MINIMUM_POSITION_VALUE);		//convert To DXL Position values for first motor
				int dm2 = cursorConverter(SCREEN_HEIGHT - mY, SCREEN_HEIGHT, DXL23_MAXIMUM_POSITION_VALUE, DXL23_MINIMUM_POSITION_VALUE);	//convert To DXL Position values for second motor
				int dm3 = (1050 * 3) + dm3Offset - dm2;																					//convert To DXL Position values for third motor

				//Write DXL Positions
				if (dm2 <= DXL23_MAXIMUM_POSITION_VALUE && dm2 >= DXL23_MINIMUM_POSITION_VALUE)
					controlDynamixel.groupMoveDynamixel(1, dm1, 2, dm2, 3, dm3);
				else
					controlDynamixel.writeSinglePos(1, dm1);
			}
		}
		
		torqueGraph.processInput();
		
		prevTorque = torque;
		torque = controlDynamixel.dynamixelCurrentToTorque(2);		//Read present current of second DXL motor and convert to torque

		if (time < 1)												//Draw a line on graph, going to new torque value
			time = torqueGraph.drawLine(time, prevTorque, torque);	
		else 
			time = torqueGraph.shiftDrawLine(prevTorque, torque);	//Reset time and draw new graph

		if (dontReapeatPlacement && !GetKeyState(VK_CAPITAL))		//set makes sure the cursor position is saved til capslock is turned on again
		{
			dontReapeatPlacement = false;
			std::cout << "Please turn on CapsLock to control the robot\n";
		}
		
		if (GetKeyState(VK_NUMLOCK))
		{
			if (collector.MyPose == 1 || GetAsyncKeyState(VK_LCONTROL))
			{
				controlDynamixel.openCloseGripper(true);
			}
			else if (collector.MyPose == 0 || GetAsyncKeyState(VK_LSHIFT))
			{
				controlDynamixel.openCloseGripper(false);
			}
		}
		else
		{
			if ((collector.currentPose == myo::Pose::fist || GetAsyncKeyState(VK_LCONTROL)) && gripperPos < DXL_CLOSED_GRIPPER_VALUE)
			{
				gripperPos = gripperPos + 10;
				controlDynamixel.groupMoveDynamixel(4, gripperPos, 5, gripperPos);
			}
			else if ((collector.currentPose == myo::Pose::fingersSpread || GetAsyncKeyState(VK_LSHIFT)) && gripperPos > DXL_OPEN_GRIPPER_VALUE)
			{
				gripperPos = gripperPos - 10;
				controlDynamixel.groupMoveDynamixel(4, gripperPos, 5, gripperPos);
			}
		}


		if (GetAsyncKeyState(VK_ESCAPE))
		{
			controlDynamixel.restartSetup();
		}
		Sleep(1);
	}
	torqueGraph.endDrawLine();		//End the graph.

	return 0;
}


double cursorConverter(int cursorPos, double screenValue, double DXL_Max, double DXL_Min)
{
	double a = (DXL_Max - DXL_Min) / screenValue;
	double jointGoal = a * cursorPos + DXL_Min;
	return jointGoal;
}


int rollCalculator(int rollAmount, int rollOffset)
{
	int calculatedValue;
	calculatedValue = rollOffset - rollAmount;
	if (calculatedValue < 1024 && calculatedValue > -300)
	{
		return calculatedValue;
	}
	else if (calculatedValue > 1024)
	{
		return 1024;
	}
	else
	{
		return -300;
	}
}
