#include <SADXModLoader.h>
#include "FreeCam.h"

// Speeps' MemeMaker used for reference

int FreeCamMode = 0;
float FreeCamSpeed = 2.0f;
long double FreeCamMoveX;
long double FreeCamMoveY;
POINT MouseCursorPosition;
bool FreeCamLockCheck = false;

enum FreeCamModes
{
	Camera_None = 0,
	Camera_Look = 1,
	Camera_Move = 2,
	Camera_Zoom = 3,
	Camera_Lock = 4
};

void FreeCam_OnInput()
{
	// Stop if free camera mode is disabled or camera data is unavailable
	if (!FreeCamEnabled || Camera_Data1 == NULL)
		return;

	// Toggle camera lock
	if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState(VK_LSHIFT) & 0x8000)
	{
		if (!FreeCamLockCheck)
		{
			if (FreeCamMode != Camera_Lock)
				FreeCamMode = Camera_Lock;
			else
				FreeCamMode = Camera_Look;
			FreeCamLockCheck = true;
		}
	}
	else
		FreeCamLockCheck = false;

	// If camera is locked, stop there
	if (FreeCamMode == Camera_Lock)
		return;

	// Set camera mode
	if (GetKeyState(VK_CONTROL) & 0x8000)
		FreeCamMode = Camera_Zoom;
	else if (GetKeyState(VK_LSHIFT) & 0x8000)
		FreeCamMode = Camera_Move;
	else FreeCamMode = Camera_Look;

	// Get screen center and cursor delta
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	GetCursorPos(&MouseCursorPosition);

	// Set delta and center cursor
	int deltaX = MouseCursorPosition.x - w / 2;
	int deltaY = MouseCursorPosition.y - h / 2;
	FreeCamMoveX = (double)(deltaX * 4);
	FreeCamMoveY = (double)(deltaY * 4);
	SetCursorPos(w / 2, h / 2);

	// Calculate camera movement speed and angle
	float movementSpeed_CurX = FreeCamSpeed * abs(FreeCamMoveX) / 40.0f;
	float movementSpeed_CurY = FreeCamSpeed * abs(FreeCamMoveY) / 40.0f;
	Angle angle_hz_move = -Camera_Data1->Rotation.y - (unsigned __int64)(atan2(0, FreeCamMoveX) * 65536.0 * -0.1591549762031479);
	Angle angle_hz_zoom = -Camera_Data1->Rotation.y - (unsigned __int64)(atan2(FreeCamMoveY, 0) * 65536.0 * -0.1591549762031479);
	Angle angle_vt_zoom = Camera_Data1->Rotation.x - (unsigned __int64)(atan2(FreeCamMoveY, 0) * 65536.0 * -0.1591549762031479);

	// Perform movement
	switch (FreeCamMode)
	{
	case Camera_Look:
		if (FreeCamMoveY > 0)  Camera_Data1->Rotation.x = (int)(Camera_Data1->Rotation.x - (movementSpeed_CurY * 100));
		if (FreeCamMoveY < 0) Camera_Data1->Rotation.x = (int)(Camera_Data1->Rotation.x + (movementSpeed_CurY * 100));
		if (FreeCamMoveX > 0)  Camera_Data1->Rotation.y = (int)(Camera_Data1->Rotation.y - (movementSpeed_CurX * 100));
		if (FreeCamMoveX < 0) Camera_Data1->Rotation.y = (int)(Camera_Data1->Rotation.y + (movementSpeed_CurX * 100));
		break;
	case Camera_Move:
		if (FreeCamMoveX > 0 || FreeCamMoveX < 0)
		{
			Camera_Data1->Position.x -= njCos(angle_hz_move) * (movementSpeed_CurX);
			Camera_Data1->Position.z -= njSin(angle_hz_move) * (movementSpeed_CurX);
		}
		if (FreeCamMoveY > 0)
		{
			Camera_Data1->Position.y += movementSpeed_CurY;
		}
		if (FreeCamMoveY < 0)
		{
			Camera_Data1->Position.y -= movementSpeed_CurY;
		}
		break;
	case Camera_Zoom:
		if (FreeCamMoveY > 0 || FreeCamMoveY < 0)
		{
			Camera_Data1->Position.x += njCos(angle_hz_zoom) * (movementSpeed_CurY);
			Camera_Data1->Position.z += njSin(angle_hz_zoom) * (movementSpeed_CurY);
			Camera_Data1->Position.y += njCos(angle_vt_zoom) * (movementSpeed_CurY);
		}
		break;
	}
}

void FreeCam_OnFrame()
{
	// Enable camera hacks
	if (FreeCamEnabled)
	{
		WriteData<1>((int*)0x437760, 0x75); // Stop normal cameras trying to take control
		// Always default camera
		if (FreeCamMode != 0)
		{
			CameraType[0] = 0;
			CameraType[1] = 0;
			CameraType[2] = 0;
			WriteData((int*)0x3B2CAE3, 0x462E9002);
			WriteData((int*)0x3B2CAE7, 0x467D8000);
			// Force Auto Cam because it breaks otherwise
			WriteData<1>((int*)0x3B2CBA8, 4); // camera_flags
		}
	}
	// Disable camera hacks
	else if (FreeCamMode != 0)
	{
		WriteData<1>((int*)0x437760, 0x74); // Enable normal cameras
		WriteData<1>((int*)0x436308, 0x8F); // Enable first person cam
		FreeCamMode = 0;
	}
}