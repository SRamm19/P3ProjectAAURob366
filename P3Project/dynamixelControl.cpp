#include "dynamixelControl.h"

dynamixelControl::dynamixelControl()
{
	motorAmount = 5;
	maxVelocity = 50;
	startpos = 2024;
	comm_result = COMM_TX_FAIL;
	addparam_result = false;
	getdata_result = false;
	dxl_error = 0;	
	dxl_present_current = 0;
}

dynamixelControl::~dynamixelControl()
{

}

void dynamixelControl::runDynamixelSetup()
{
	dynamixelControl::setupDynamixelPort();
	dynamixelControl::rebootAll();
	dynamixelControl::specifyControlMode();
	dynamixelControl::enableTorque();
	dynamixelControl::specifyVelocity();
	dynamixelControl::moveDynamixelStartPos();
}

void dynamixelControl::restartSetup()
{
	dynamixelControl::rebootAll();
	dynamixelControl::specifyControlMode();
	dynamixelControl::enableTorque();
	dynamixelControl::specifyVelocity();
}

void dynamixelControl::setupDynamixelPort()
{
	portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
	dynamixel::GroupSyncWrite groupSyncWritePos(portHandler, packetHandler, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION);		// Initialize GroupSyncWrite instance
	dynamixel::GroupSyncRead groupSyncReadPos(portHandler, packetHandler, ADDR_PRO_PRESENT_POSITION, LEN_PRO_PRESENT_POSITION);	// Initialize Groupsyncread instance for Present Position
	// Open port
	if (portHandler->openPort())
	{
		printf("Succeeded to open the port!\n");
	}
	else
	{
		printf("Failed to open the port!\n");
	}
	// Set baudrate
	if (portHandler->setBaudRate(BAUDRATE))
	{
		printf("Succeeded to change the baudrate!\n");
	}
	else
	{
		printf("Failed to change the baudrate!\n");
	}
}

void dynamixelControl::rebootAll()
{
	for (size_t i = 0; i < motorAmount; i++)
	{
		comm_result = packetHandler->reboot(portHandler, i + 1, &dxl_error);
		if (comm_result != COMM_SUCCESS)
		{
			printf("%s\n", packetHandler->getTxRxResult(comm_result));
		}
		else if (dxl_error != 0)
		{
			printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		}
		else
		{
			printf("[ID:%03d] reboot Succeeded\n", (i+1));
		}
	}
}

void dynamixelControl::enableTorque()
{
	for (int i = 0; i < motorAmount; i++)
	{
		// Enable Dynamixel#i+1 Torque
		comm_result = packetHandler->write1ByteTxRx(portHandler, i + 1, ADDR_PRO_TORQUE_ENABLE, true, &dxl_error);
		if (comm_result != COMM_SUCCESS)
		{
			printf("%s\n", packetHandler->getTxRxResult(comm_result));
		}
		else if (dxl_error != 0)
		{
			printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		}
		else
		{
			printf("Dynamixel#%d has been successfully connected \n", i + 1);
		}
	}
}

void dynamixelControl::specifyVelocity()
{
	for (int i = 0; i < motorAmount; i++)
	{
		// Enable Dynamixel#i+1 Torque
		comm_result = packetHandler->write4ByteTxRx(portHandler, i + 1, 112, 50, &dxl_error);
		if (comm_result != COMM_SUCCESS)
		{
			printf("%s\n", packetHandler->getTxRxResult(comm_result));
		}
		else if (dxl_error != 0)
		{
			printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		}
		else
		{
			printf("Dynamixel#%d's velocity has succesfully been set \n", i + 1);
		}
	}
}

void dynamixelControl::specifyControlMode()
{
	for (int i = 0; i < motorAmount; i++)
	{
		// Enable Dynamixel#i+1 Torque
		comm_result = packetHandler->write2ByteTxRx(portHandler, i + 1, 11, 3, &dxl_error);
		if (comm_result != COMM_SUCCESS)
		{
			printf("%s\n", packetHandler->getTxRxResult(comm_result));
		}
		else if (dxl_error != 0)
		{
			printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		}
		else
		{
			printf("Dynamixel#%d has succesfully been set to position control mode\n", i + 1);
		}
	}
}

void dynamixelControl::moveDynamixelStartPos()
{
	dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION);		// Initialize GroupSyncWrite instance
	
	alocatePosVal(startpos);	// Allocate starting position value into byte array
	addparam_result = groupSyncWrite.addParam(1, param_goal_position);	// Add Dynamixel#1 goal position value to the Syncwrite storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", 1);
	}
	addparam_result = groupSyncWrite.addParam(2, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", 2);
	}

	alocatePosVal(startpos - 1024);	// Allocate starting position value into byte array
	addparam_result = groupSyncWrite.addParam(3, param_goal_position);	// Add Dynamixel#3 goal position value to the Syncwrite storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", 3);
	}

	alocatePosVal(DXL_OPEN_GRIPPER_VALUE);	// Allocate starting position value into byte array
	addparam_result = groupSyncWrite.addParam(4, param_goal_position);	// Add Dynamixel#4 goal position value to the Syncwrite storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", 4);
	}
	addparam_result = groupSyncWrite.addParam(5, param_goal_position);	// Add Dynamixel#5 goal position value to the Syncwrite storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", 5);
	}


	// Syncwrite goal position
	comm_result = groupSyncWrite.txPacket();
	if (comm_result != COMM_SUCCESS) printf("%s\n", packetHandler->getTxRxResult(comm_result));
	// Clear syncwrite parameter storage
	groupSyncWrite.clearParam();
}

void dynamixelControl::alocatePosVal(int PosVal)
{
	param_goal_position[0] = DXL_LOBYTE(DXL_LOWORD(PosVal));
	param_goal_position[1] = DXL_HIBYTE(DXL_LOWORD(PosVal));
	param_goal_position[2] = DXL_LOBYTE(DXL_HIWORD(PosVal));
	param_goal_position[3] = DXL_HIBYTE(DXL_HIWORD(PosVal));
}

void dynamixelControl::groupMoveDynamixel(int motorID1, int goalPos1, int motorID2, int goalPos2, int motorID3, int goalPos3)
{
	dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION);		// Initialize GroupSyncWrite instance
	
	alocatePosVal(goalPos1);
	addparam_result = groupSyncWrite.addParam(motorID1, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", motorID1);
	}
	
	alocatePosVal(goalPos2);
	addparam_result = groupSyncWrite.addParam(motorID2, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", motorID2);
	}

	alocatePosVal(goalPos3);
	addparam_result = groupSyncWrite.addParam(motorID3, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", motorID3);
	}

	comm_result = groupSyncWrite.txPacket();
	if (comm_result != COMM_SUCCESS) printf("%s\n", packetHandler->getTxRxResult(comm_result));
	// Clear syncwrite parameter storage
	groupSyncWrite.clearParam();
}

void dynamixelControl::groupMoveDynamixel(int motorID1, int goalPos1, int motorID2, int goalPos2)
{
	dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION);		// Initialize GroupSyncWrite instance

	alocatePosVal(goalPos1);
	addparam_result = groupSyncWrite.addParam(motorID1, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", motorID1);
	}

	alocatePosVal(goalPos2);
	addparam_result = groupSyncWrite.addParam(motorID2, param_goal_position);	// Add Dynamixel#2 goal position value to the Syncwrite parameter storage
	if (addparam_result != true)
	{
		fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", motorID2);
	}

	comm_result = groupSyncWrite.txPacket();
	if (comm_result != COMM_SUCCESS) printf("%s\n", packetHandler->getTxRxResult(comm_result));
	// Clear syncwrite parameter storage
	groupSyncWrite.clearParam();
}

void dynamixelControl::openCloseGripper(bool action)
{
	if (action)
		groupMoveDynamixel(4, DXL_OPEN_GRIPPER_VALUE, 5, DXL_OPEN_GRIPPER_VALUE);
	else
		groupMoveDynamixel(4, DXL_CLOSED_GRIPPER_VALUE, 5, DXL_CLOSED_GRIPPER_VALUE);
}

void dynamixelControl::writeSinglePos(int motorID, int posVal)
{
	comm_result = packetHandler->write4ByteTxRx(portHandler, motorID, ADDR_PRO_GOAL_POSITION, posVal, &dxl_error);
	if (comm_result != COMM_SUCCESS)
	{
		printf("%s\n", packetHandler->getTxRxResult(comm_result));
	}
	else if (dxl_error != 0)
	{
		printf("%s\n", packetHandler->getRxPacketError(dxl_error));
	}
}

float dynamixelControl::dynamixelCurrentToTorque(int motorID)
{
	comm_result = packetHandler->read2ByteTxRx(portHandler, motorID, ADDR_PRO_PRESENT_CURRENT, (uint16_t*)&dxl_present_current, &dxl_error);
	if (comm_result != COMM_SUCCESS)
	{
		printf("%s\n", packetHandler->getTxRxResult(comm_result));
	}
	else if (dxl_error != 0)
	{
		printf("%s\n", packetHandler->getRxPacketError(dxl_error));
	}
	float dxl_converted_torque = dxl_present_current;
	dxl_converted_torque = (1.09*dxl_converted_torque-0.03);
	//std::cout << dxl_converted_torque << std::endl;
	dxl_converted_torque = dxl_converted_torque / 1000;

	return dxl_converted_torque;
}