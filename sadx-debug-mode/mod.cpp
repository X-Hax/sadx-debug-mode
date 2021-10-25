#include <SADXModLoader.h>
#include "IniFile.hpp"
#include "Trampoline.h"
#include "Data.h"
#include "SADXFunctionsNew.h"
#include "FreeCam.h"
#include "FreeMovements.h"

FunctionPointer(void, Cutscene_WaitForInput, (int a), 0x4314D0);
FunctionPointer(void, DrawCollisionInfo, (CollisionInfo* collision), 0x79F4D0);
FunctionPointer(void, DrawDebugCollision, (ObjectMaster* a1), 0x4DBCC0);
FastcallFunctionPointer(void, stSetTexture, (int index), 0x0078D140);

int CurrentPalettes[]= { -1, -1, -1, -1, -1, -1 };
int CurrentLights[] = { -1, -1, -1, -1, -1, -1 };
int CurrentPalette = 0;
int CurrentStageLight = 0;
int DebugMessageTimer = 0;
bool SpeedHack = false;
int FrameIncrementCurrent = 1;
bool CollisionDebug = false;
bool TextureDebug = false;
const char* DebugMessage;
char DebugMsgBuffer[32];
bool FreeCamEnabled = false;
NJS_COLOR DebugFontColorBK;
float DebugFontSizeBK;

char DebugSetting = 0;
bool CrashDebug = false;
bool EnableFontScaling = false;
signed char DeathPlanesEnabled = -1;
int DisplaySoundIDMode = 0;
int CurTexList_Current = 0;
int VoiceID = -1;
bool FreezeFrame_Pressed = false;
int FreezeFrame_Mode = 0;
char BackupBytes[] = { 0xC3u, 0xC3u };
std::string FreeCamModeStrings[] = {"OFF", "LOOK", "MOVE", "ZOOM", "LOCKED"};

// White texture for texture override
unsigned __int8 whitetexturedata[6144];
NJS_TEXINFO whitetexturetexinfo;
NJS_TEXNAME whitetextures[1];
NJS_TEXLIST whitetexturetexlist = {arrayptrandlength(whitetextures)};
NJS_TEXMEMLIST whitetexturetexmemlist;

void BackupDebugFontSettings()
{
	DebugFontColorBK = DebugFontColor;
	DebugFontSizeBK = DebugFontSize;
}

void RestoreDebugFontSettings()
{
	DebugFontColor = DebugFontColorBK;
	DebugFontSize = DebugFontSizeBK;
}

void DrawDebugRectangle(float leftchars, float topchars, float numchars_horz, float numchars_vert)
{
	float FontScale;
	if (!EnableFontScaling) FontScale = 1.0f;
	else
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) FontScale = floor((float)VerticalResolution / 480.0f);
		else FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	if (DebugSetting == 6 || DebugSetting == 7)
	{
		if (EnableFontScaling || HorizontalResolution < 1024) DrawRect_Queue(leftchars*FontScale*10.0f, topchars*FontScale*10.0f, numchars_horz*FontScale*10.0f, numchars_vert*FontScale*10.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
		else DrawRect_Queue(leftchars*FontScale*16.0f, topchars*FontScale*16.0f, numchars_horz*FontScale*16.0f, numchars_vert*FontScale*16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite); 
	}
	else DrawRect_Queue(leftchars*FontScale*16.0f, topchars*FontScale*16.0f, numchars_horz*FontScale*16.0f, numchars_vert*FontScale*16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

void RenderDeathPlanes(NJS_OBJECT* object)
{
	SetTextureToCommon();
	njPushMatrix(0);
	njControl3D_Backup();
	njControl3D_Add(NJD_CONTROL_3D_CONSTANT_MATERIAL | NJD_CONTROL_3D_ENABLE_ALPHA | NJD_CONTROL_3D_CONSTANT_ATTR);
	BackupConstantAttr();
	AddConstantAttr(0, NJD_FLAG_USE_ALPHA);
	SetMaterialAndSpriteColor_Float(0.5f, 1.0f, 0, 0);
	DrawQueueDepthBias = 47952.0f;
	ProcessModelNode(object, (QueuedModelFlagsB)4, 1.0f);
	njPopMatrix(1u);
	DrawQueueDepthBias = 0.0f;
	RestoreConstantAttr();
	njControl3D_Restore();
}

void UpdateKeys()
{
	int KeysHeld = 0;
	int CursorPosX1 = 14;
	int CursorPosY1 = 21;
	int CursorPosX2 = 14;
	int CursorPosY2 = 23;
	for (int i = 1; i < 256; i++) //exclude key 0 which is always pressed
	{
		if (KeyboardKeys[i].held)
		{
			BackupDebugFontSettings();
			SetDebugFontSize(16);
			SetDebugFontColor(0xFFBFBFBF);
			if (KeysHeld < 4)
			{ 
				DisplayDebugStringFormatted(NJM_LOCATION(CursorPosX1, CursorPosY1), "%03d", i);
				CursorPosX1 += 4;
			}
			else
			{
				DisplayDebugStringFormatted(NJM_LOCATION(CursorPosX2, CursorPosY2), "%03d", i);
				CursorPosX2 += 4;
			}
			KeysHeld++;
			RestoreDebugFontSettings();
		}
	}
}

void UpdateButtons()
{
	std::string ButtonsString = "";
	std::string PadString = "";
	int CursorPos = 17;
	if (ControllerPointers[0]->HeldButtons & Buttons_A)
	{
		ButtonsString += "A ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_B)
	{
		ButtonsString += "B ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_C)
	{
		ButtonsString += "C ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_D)
	{
		ButtonsString += "D ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_X)
	{
		ButtonsString += "X ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Y)
	{
		ButtonsString += "Y ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Z)
	{
		ButtonsString += "Z ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Start)
	{
		ButtonsString += "START ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Up)
	{
		PadString += "UP ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Down)
	{
		PadString += "DOWN ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Left)
	{
		PadString += "LEFT ";
	}
	if (ControllerPointers[0]->HeldButtons & Buttons_Right)
	{
		PadString += "RIGHT ";
	}
	if (PadString == "") PadString = "CENTER";
	BackupDebugFontSettings();
	SetDebugFontSize(16);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(CursorPos, 12), ButtonsString.c_str());
	DisplayDebugStringFormatted(NJM_LOCATION(18, 14), PadString.c_str());
	RestoreDebugFontSettings();
}

void ScaleDebugFont(int scale)
{
	float FontScale;
	if (!EnableFontScaling) FontScale = 1.0f;
	else
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) FontScale = floor((float)VerticalResolution / 480.0f);
		else FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	SetDebugFontSize(FontScale*scale);
}

void PlayerDebug()
{
	ScaleDebugFont(16);
	if (EntityData1Ptrs[0] == nullptr || CharObj2Ptrs[0] == nullptr)
	{
		SetDebugFontColor(0xFFFF0000);
		DisplayDebugString(NJM_LOCATION(2, 1), "- PLAYER INFO UNAVAILABLE -");
		return;
	}
	DrawDebugRectangle(1.75f, 0.75f, 25, 26);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(6, 1), "- PLAYER INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %.2f", EntityData1Ptrs[0]->Position.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %.2f", EntityData1Ptrs[0]->Position.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %.2f", EntityData1Ptrs[0]->Position.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.x, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.y, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.z, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "ACTION: %03d", EntityData1Ptrs[0]->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "STATUS: %X", EntityData1Ptrs[0]->Status);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "NEXT  : %X", EntityData1Ptrs[0]->NextAction);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "ANIM: %03d", CharObj2Ptrs[0]->AnimationThing.Index);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "FRAME: %.2f", CharObj2Ptrs[0]->AnimationThing.Frame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "SPEED X: %.4f", CharObj2Ptrs[0]->Speed.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "SPEED Y: %.4f", CharObj2Ptrs[0]->Speed.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "SPEED Z: %.4f", CharObj2Ptrs[0]->Speed.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 22), "RINGS: %03d", Rings);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 23), "LIVES: %03d", Lives);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 24), "IDLE: %04d", CharObj2Ptrs[0]->IdleTime);
}

void CameraDebug()
{
	ScaleDebugFont(16);
	if (Camera_Data1 == nullptr)
	{
		SetDebugFontColor(0xFFFF0000);
		DisplayDebugString(NJM_LOCATION(2, 1), "- CAMERA INFO UNAVAILABLE -");
		return;
	}
	SetDebugFontColor(0xFF88FFAA);
	DrawDebugRectangle(1.75f, 0.75f, 23, 21);
	DisplayDebugString(NJM_LOCATION(5, 1), "- CAMERA INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %.2f", Camera_Data1->Position.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %.2f", Camera_Data1->Position.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %.2f", Camera_Data1->Position.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.x, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.y, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.z, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "HZFOV: %06d / %03.0f", (Uint16)HorizontalFOV_BAMS, (360.0f / 65535.0f) *(Uint16)HorizontalFOV_BAMS);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ACTION: %02d", Camera_Data1->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %.2f", Camera_CurrentActionFrame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "MODE: %d", CameraType[3]);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "FLAGS: %X", camera_flags);	
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "FREE CAM: %s", FreeCamModeStrings[FreeCamMode].c_str());
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "CAM SPEED: %.2f", FreeCamSpeed);
}

void FogDebug()
{
	NJS_COLOR FogColor;
	FogColor.color = LevelFogData.Color;
	DrawDebugRectangle(1.75f, 0.75f, 31, 23);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(10, 1), "- FOG INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "ENABLED: %01d", LevelFogData.Toggle);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "DIST MIN: %.4f", LevelFogData.Layer);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "DIST MAX: %.4f", LevelFogData.Distance);
	SetDebugFontColor(LevelFogData.Color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "COLOR");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(9, 6), ": R%03d G%03d B%03d A%03d", FogColor.argb.r, FogColor.argb.g, FogColor.argb.b, FogColor.argb.a);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(8, 9), "- SKY BOX INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "SCALE X: %.4f", Skybox_Scale.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "SCALE Y: %.4f", Skybox_Scale.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "SCALE Z: %.4f", Skybox_Scale.z);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(7, 16), "- DRAW DIST INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "SKY MIN: %.4f", SkyboxDrawDistance.Minimum);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "SKY MAX: %.4f", SkyboxDrawDistance.Maximum);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "LEVEL MIN: %.4f", LevelDrawDistance.Minimum);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "LEVEL MAX: %.4f", LevelDrawDistance.Maximum);
}

void GameDebug()
{
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	DrawDebugRectangle(1.75f, 0.75f, 22, 21.5f);
	DisplayDebugString(NJM_LOCATION(5, 1), "- GAME STATS -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "FRAME   : %08d", FrameCounter);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "UNPAUSED: %08d", FrameCounterUnpaused);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "LEVEL   : %08d", LevelFrameCount);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "GAME MODE : %02d", GameMode);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "GAME STATE: %02d", GameState);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "CHARACTER : %01d", CurrentCharacter);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "CHAR SEL  : %01d", CurrentCharacterSelection);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "LEVEL: %02d", CurrentLevel);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "ACT: %01d", CurrentAct);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "CHAO STAGE: %02d", CurrentChaoStage);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "EVENT ID  : %03d", CutsceneID);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 17), "EVENT CODE: %X", CurrentCutsceneCode);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "MUSIC ID  : %d", CurrentSong);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "LAST VOICE: %d", VoiceID);
}

Sint32 __cdecl njSetTexture_Hax(NJS_TEXLIST* texlist)
{
	CurrentTexList = texlist;
	CurrentTextureNum = 0;
	if (CrashDebug)
	{
		if (CurTexList_Current != (int)CurrentTexList)
		{
			PrintDebug("Texlist change: %X\n", CurrentTexList);
			CurTexList_Current = (int)CurrentTexList;
		}
	}
	return Direct3D_SetTexList(texlist);
}

void InputDebug()
{
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 30.5f, 24.5f);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(7, 1), "- CONTROLLER INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "ANALOG1 X: %04d", ControllerPointers[0]->LeftStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "ANALOG1 Y: %04d", ControllerPointers[0]->LeftStickY);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "ANALOG2 X: %04d", ControllerPointers[0]->RightStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANALOG2 Y: %04d", ControllerPointers[0]->RightStickY);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "TRIGGER L: %03d", ControllerPointers[0]->LTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "TRIGGER R: %03d", ControllerPointers[0]->RTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "BUTTONS HELD:");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "PAD DIRECTION:");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "BUTTONS RAW: %08X", ControllerPointers[0]->HeldButtons);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(8, 19), "- KEYBOARD INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "KEYS HELD:");
	njPushMatrix(0);
	float FontScale = 1.0f;
	if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) FontScale = floor((float)VerticalResolution / 480.0f) * 0.5f;
	else FontScale = floor((float)HorizontalResolution / 640.0f) * 0.5f;
	float AnalogRectPosX = (float)HorizontalResolution - 160.0f * FontScale;
	float AnalogRectPosY = (float)VerticalResolution - 160.0f * FontScale;
	float XPos = (float)ControllerPointers[0]->LeftStickX * FontScale * 0.5f;
	float YPos = (float)ControllerPointers[0]->LeftStickY * FontScale * 0.5f;
	float XPos2 = (float)ControllerPointers[0]->RightStickX * FontScale * 0.5f;
	float YPos2 = (float)ControllerPointers[0]->RightStickY * FontScale * 0.5f;
	float AnalogCenterX = XPos + AnalogRectPosX + 64.0f * FontScale;
	float AnalogCenterY = YPos + AnalogRectPosY + 64.0f * FontScale;
	float AnalogCenterX2 = XPos2 + AnalogRectPosX + 64.0f * FontScale;
	float AnalogCenterY2 = YPos2 + AnalogRectPosY + 64.0f * FontScale;
	DrawRect_Queue(AnalogRectPosX, AnalogRectPosY, AnalogRectPosX + 137.0f*FontScale, AnalogRectPosY + 137.0f * FontScale, 64000.0f, 0x7F000000, QueuedModelFlagsB_SomeTextureThing);
	DrawRect_Queue(AnalogCenterX2, AnalogCenterY2, AnalogCenterX2 + 10.0f * FontScale, AnalogCenterY2 + 10.0f * FontScale, 64000.0f, 0xFF0000FF, QueuedModelFlagsB_SomeTextureThing);
	DrawRect_Queue(AnalogCenterX, AnalogCenterY, AnalogCenterX + 10.0f * FontScale, AnalogCenterY + 10.0f * FontScale, 64000.0f, 0xFFFF0000, QueuedModelFlagsB_SomeTextureThing);
	njPopMatrix(1u);
}

NJS_POINT2COL Pause_Point2Col;
NJS_POINT2 Pause_Points[4];
NJS_COLOR Pause_Colors[4];

void DrawAss(NJS_COLOR color1, NJS_COLOR color2, float C1_POW, float C2_POW, float YPos)
{
	NJS_POINT2COL ColorGradient_Point2Col;
	NJS_POINT2 ColorGradient_Points[4];
	NJS_COLOR ColorGradient_Colors[] = { {0xFFFF00FF}, {0xFFFF00FF}, {0x00000000}, {0x00000000} };
	for (int i = 0; i < 256; i++)
	{
		//ColorR( "Generate_CO1" ) * ( 1 - ( LoopIndex("co1") ) / 256.0 ) pow COPow( "Generate_CO1" )
		//ColorR( "Generate_CO2" ) * ( 1 - ( LoopIndex("co2") ) / 256.0 ) pow COPow( "Generate_CO2" )
		ColorGradient_Colors[0].argb.a = 255;
		ColorGradient_Colors[0].argb.r = min(255, (color1.argb.r * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.r * pow(1.0f - i / 256.0f, C2_POW)));
		ColorGradient_Colors[0].argb.g = min(255, (color1.argb.g * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.g * pow(1.0f - i / 256.0f, C2_POW)));
		ColorGradient_Colors[0].argb.b = min(255, (color1.argb.b * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.b * pow(1.0f - i / 256.0f, C2_POW)));
		ColorGradient_Colors[1].color = ColorGradient_Colors[0].color;
		ColorGradient_Colors[2].color = 0xFF000000;
		ColorGradient_Colors[3].color = 0xFF000000;
		ColorGradient_Point2Col.tex = 0;
		ColorGradient_Points[0].x = 32+i;
		ColorGradient_Points[0].y = YPos - 128;
		ColorGradient_Points[1].x = 32+i;
		ColorGradient_Points[1].y = YPos - 96;
		ColorGradient_Points[2].x = 33+i;
		ColorGradient_Points[2].y = YPos - 128;
		ColorGradient_Points[3].x = 33+i;
		ColorGradient_Points[3].y = YPos - 96;
		ColorGradient_Point2Col.p = (NJS_POINT2*)&ColorGradient_Points;
		ColorGradient_Point2Col.col = (NJS_COLOR*)&ColorGradient_Colors;
		Draw2DLinesMaybe_Queue((NJS_POINT2COL*)&ColorGradient_Point2Col, 4, 34000.0f, NJD_TRANSPARENT, QueuedModelFlagsB_SomeTextureThing);
	}
}

void AddLSPalette(int ID)
{
	for (int q = 0; q < LengthOfArray(CurrentPalettes); q++)
	{
		if (CurrentPalettes[q] == ID)
		{
			return;
		}
		if (CurrentPalettes[q] == -1)
		{
			CurrentPalettes[q] = ID;
			//PrintDebug("Added palette: %d\n", ID);
			return;
		}
	}
}

void AddStageLight(int ID)
{
	for (int q = 0; q < LengthOfArray(CurrentLights); q++)
	{
		if (CurrentLights[q] == ID)
		{
			return;
		}
		if (CurrentLights[q] == -1)
		{
			CurrentLights[q] = ID;
			//PrintDebug("Added stage light: %d\n", ID);
			return;
		}
	}
}

void FindLSPalette()
{
	for (int q = 0; q < LengthOfArray(CurrentPalettes); q++)
	{
		CurrentPalettes[q] = -1;
	}
	for (int i = 0; i < 255; i++)
	{
		if (LightPaletteData[i].Level == CurrentLevel && LightPaletteData[i].Act == CurrentAct)
		{
			//PrintDebug("Adding palette\n");
			AddLSPalette(i);
		}
	}
}

void FindStageLight()
{
	for (int q = 0; q < LengthOfArray(CurrentLights); q++)
	{
		CurrentLights[q] = -1;
	}
	for (int i = 0; i < 255; i++)
	{
		if (StageLights[i].level == CurrentLevel && StageLights[i].act == CurrentAct)
		{
			//PrintDebug("Adding stage light\n");
			AddStageLight(i);
		}
	}
}

void LSPaletteDebug()
{
	FindLSPalette();
	NJS_COLOR AmbColor;
	NJS_COLOR CO1Color;
	NJS_COLOR CO2Color;
	NJS_COLOR SP1Color;
	NJS_COLOR SP2Color;
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 29);
	if (GetModuleHandle(L"sadx-dc-lighting") != nullptr) SetDebugFontColor(0xFFFF0000);
	else SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(6, 1), "- LS PALETTE INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "TYPE : %X", LightPaletteData[CurrentPalettes[CurrentPalette]].Type);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "FLAGS: %X", LightPaletteData[CurrentPalettes[CurrentPalette]].Flags);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "DIR X: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].Direction.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "DIR Y: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].Direction.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "DIR Z: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].Direction.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "DIFFUSE: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].DIF);
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_R);
	AmbColor.argb.g = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_G);
	AmbColor.argb.b = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_B);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "AMBIENT");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "R: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "G: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "B: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].AMB_B);
	//CO1
	CO1Color.argb.a = 255;
	CO1Color.argb.r = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO_R);
	CO1Color.argb.g = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO_G);
	CO1Color.argb.b = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO_B);
	SetDebugFontColor(CO1Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 17), "COLOR1");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "R  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "G  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "B  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO_B);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "POW: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO_pow);
	//SP1
	SP1Color.argb.a = 255;
	SP1Color.argb.r = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP_R);
	SP1Color.argb.g = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP_G);
	SP1Color.argb.b = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP_B);
	SetDebugFontColor(SP1Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 17), "SPECULAR1");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 18), "R  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP_R);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 19), "G  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP_G);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 20), "B  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP_B);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 21), "POW: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP_pow);
	//CO2
	CO2Color.argb.a = 255;
	CO2Color.argb.r = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_R);
	CO2Color.argb.g = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_G);
	CO2Color.argb.b = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_B);
	SetDebugFontColor(CO2Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 23), "COLOR2");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 24), "R  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 25), "G  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 26), "B  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_B);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 27), "POW: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].CO2_pow);
	//SP2
	SP2Color.argb.a = 255;
	SP2Color.argb.r = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_R);
	SP2Color.argb.g = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_G);
	SP2Color.argb.b = int(255.0f * LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_B);
	SetDebugFontColor(SP2Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 23), "SPECULAR2");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 24), "R  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_R);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 25), "G  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_G);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 26), "B  : %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_B);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 27), "POW: %.3f", LightPaletteData[CurrentPalettes[CurrentPalette]].SP2_pow);
	//DisplayDebugStringFormatted(NJM_LOCATION(17, 27), "POW: %.3f", LightPaletteData[CurrentPalette].SP2_pow);
	//DrawAss(CO1Color, CO2Color, LightPaletteData[CurrentPalette].CO_pow, LightPaletteData[CurrentPalette].CO2_pow, VerticalResolution);
	//DrawAss(SP1Color, SP2Color, LSPalette.SP_pow, LSPalette.SP2_pow, VerticalResolution-48);
}

void StageLightDebug()
{
	FindStageLight();
	NJS_COLOR AmbColor;
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 22.5f);
	if (GetModuleHandle(L"sadx-dc-lighting") != nullptr) SetDebugFontColor(0xFFFF0000);
	else SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(5, 1), "- STAGE LIGHTS INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "INDEX: %d", StageLights[CurrentLights[CurrentStageLight]].index);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "FLAGS: %d", StageLights[CurrentLights[CurrentStageLight]].use_direction);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "DIR X: %.3f", StageLights[CurrentLights[CurrentStageLight]].direction.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "DIR Y: %.3f", StageLights[CurrentLights[CurrentStageLight]].direction.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "DIR Z: %.3f", StageLights[CurrentLights[CurrentStageLight]].direction.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "DIFFUSE : %.3f", StageLights[CurrentLights[CurrentStageLight]].dif);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "SPECULAR: %.3f", StageLights[CurrentLights[CurrentStageLight]].spe);
	//Stage Ambient
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].amb_r);
	AmbColor.argb.g = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].amb_g);
	AmbColor.argb.b = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].amb_b);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "AMBIENT"); 
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "R  : %.3f", StageLights[CurrentLights[CurrentStageLight]].amb_r);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "G  : %.3f", StageLights[CurrentLights[CurrentStageLight]].amb_g);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "B  : %.3f", StageLights[CurrentLights[CurrentStageLight]].amb_b);
	//Stage Diffuse
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].r);
	AmbColor.argb.g = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].g);
	AmbColor.argb.b = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].b);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "COLOR");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "R  : %.3f", StageLights[CurrentLights[CurrentStageLight]].r);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "G  : %.3f", StageLights[CurrentLights[CurrentStageLight]].g);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "B  : %.3f", StageLights[CurrentLights[CurrentStageLight]].b);

}

SoundBank_SE GetBankNumberAndID(int SoundID_HEX)
{
	char BankID = 0;
	char SoundID = 0;
	const char* BankName = "ASS";
	SoundBank_SE result;
	if (SoundID_HEX == -1)
	{
		result.Bank_Name = "";
		result.Bank_ID = -1;
		result.SE_ID = -1;
		return result;
	}
	else
	{
		for (int i = 0; i < LengthOfArray(SoundBanks) - 1; i++)
		{
			if (SoundID_HEX < SoundBanks[i + 1].StartID)
			{
				result.Bank_Name = SoundBanks[i].Name;
				result.SE_ID = max(0, SoundID_HEX - SoundBanks[i].StartID - 1);
				result.Bank_ID = SoundBanks[i].Name[8] - 48;
				if (result.Bank_ID == 9) result.Bank_ID = 10; //to make 9 into A for ADX bank
				if (result.Bank_ID == 0) result.SE_ID = SoundID_HEX; //exception for the first bank
				return result;
			}
		}
	}
	result.Bank_Name = "ASS";
	result.Bank_ID = 999;
	result.SE_ID = 999;
	return result;
}

const char* SoundLookUp(int SoundID)
{
	for (int i = 0; i < 1519; i++)
	{
		if (SENameLookUp[i].SE_ID == SoundID) return SENameLookUp[i].SE_Name;
	}
	return "ERROR";
}

void SoundDebug()
{
	DrawDebugRectangle(0.25f, 0.75f, 63.75f, 45);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	if (EnableFontScaling || HorizontalResolution < 1024) DisplayDebugString(NJM_LOCATION(12, 1), "- SOUND QUEUE -");
	else DisplayDebugString(NJM_LOCATION(24, 1), "- SOUND QUEUE -");
	if (!EnableFontScaling && HorizontalResolution >= 1024) ScaleDebugFont(16); else ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFFF00);
	DisplayDebugString(NJM_LOCATION(2, 4), "N   ID   PRI  TIME  FLAG   VOL MI/MX   PAN   PITCH    QNUM");
	SetDebugFontColor(0xFFBFBFBF);
	int ActiveSounds = 0;
	for (unsigned int i = 0; i < 36; i++)
	{
		//Add data to debug info array
		if (SoundQueue[i].PlayTime != 0)
		{
			SoundQueueDebug[i].Bank_ID = GetBankNumberAndID(SoundQueue[i].SoundID).Bank_ID;
			SoundQueueDebug[i].SE_ID = GetBankNumberAndID(SoundQueue[i].SoundID).SE_ID;
			SoundQueueDebug[i].EnumName = SoundLookUp(SoundQueue[i].SoundID);
			SoundQueueDebug[i].PlayTime = SoundQueue[i].PlayTime;
			SoundQueueDebug[i].Flags = SoundQueue[i].Flags;
			SoundQueueDebug[i].VolumeCur = SoundQueue[i].CurrentVolume;
			SoundQueueDebug[i].VolumeMax = SoundQueue[i].MaxVolume;
		}
		if (SoundQueue[i].PlayTime == 0) SetDebugFontColor(0xFFBF0000);
		else
		{
			if (SoundQueueDebug[i].Flags & 0x4000)
			{
				if (SoundQueueDebug[i].Flags & 0x1000) SetDebugFontColor(0xFF00FFFF);
				else SetDebugFontColor(0xFFBF00BF);
			}
			else if (SoundQueue[i].Panning != 0) SetDebugFontColor(0xFFFF7FB2);
			else if (SoundQueueDebug[i].Flags & 0x2000) SetDebugFontColor(0xFF7F4040);
			else if (SoundQueueDebug[i].Flags & 0x1000) SetDebugFontColor(0xFFBFBF00);
			else if (SoundQueueDebug[i].Flags & 0x100) SetDebugFontColor(0xFFFF7F00);
			else if (SoundQueueDebug[i].Flags & 0x200) SetDebugFontColor(0xFF00BF00);
			else SetDebugFontColor(0xFFBFBFBF);
			ActiveSounds++;
		}
		PrintDebugNumber(NJM_LOCATION(1, i + 1 + 5), i, 2);
		if (SoundQueue[i].SoundID != -1)
		{
			if (DisplaySoundIDMode == 1) DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%03X", SoundQueue[i].SoundID, 4);
			else if (DisplaySoundIDMode == 2) PrintDebugNumber(NJM_LOCATION(5, i + 1 + 5), SoundQueue[i].SoundID, 4);
			else DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%01X/%02i", SoundQueueDebug[i].Bank_ID, SoundQueueDebug[i].SE_ID, 4);
		}
		if (SoundQueue[i].SoundID != -1) PrintDebugNumber(NJM_LOCATION(11, i + 1+ 5), SoundQueue[i].Priority, 2);
		if (SoundQueue[i].PlayTime != 0) PrintDebugNumber(NJM_LOCATION(16, i + 1+ 5), SoundQueue[i].PlayTime, 4);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].Flags != 0) DisplayDebugStringFormatted(NJM_LOCATION(22, i + 1+ 5), "%04X", SoundQueue[i].Flags);
		if (SoundQueue[i].PlayTime != 0 && (SoundQueue[i].CurrentVolume != 0 || SoundQueue[i].MaxVolume != 0)) DisplayDebugStringFormatted(NJM_LOCATION(29, i + 1+ 5), "%04i/%04i", SoundQueue[i].CurrentVolume, SoundQueue[i].MaxVolume);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].Panning != 0) DisplayDebugStringFormatted(NJM_LOCATION(41, i + 1+ 5), "%04i", SoundQueue[i].Panning);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].PitchShift != 0) DisplayDebugStringFormatted(NJM_LOCATION(47, i + 1 + 5), "%05i", SoundQueue[i].PitchShift);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].qnum != 0) DisplayDebugStringFormatted(NJM_LOCATION(57, i + 1+ 5), "%02i", SoundQueue[i].qnum);
	}
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(2, 43), "ACTIVE SOUNDS: %d", ActiveSounds);
}

void SoundBankInfoDebug()
{
	DrawDebugRectangle(0.25f, 0.75f, 31.0f, 45);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	if (EnableFontScaling || HorizontalResolution < 1024) DisplayDebugString(NJM_LOCATION(1, 1), "- SOUNDBANK INFO -");
	else DisplayDebugString(NJM_LOCATION(6, 1), "- SOUNDBANK INFO -");
	if (!EnableFontScaling && HorizontalResolution >= 1024) ScaleDebugFont(16); else ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFFF00);
	DisplayDebugString(NJM_LOCATION(2, 4), "N  BANK  ENUM NAME");
	SetDebugFontColor(0xFFBFBFBF);
	int ActiveSounds = 0;
	//Add data to debug info array
	for (unsigned int i = 0; i < 36; i++)
	{
		if (SoundQueue[i].PlayTime != 0)
		{
			SoundQueueDebug[i].Bank_ID = GetBankNumberAndID(SoundQueue[i].SoundID).Bank_ID;
			SoundQueueDebug[i].SE_ID = GetBankNumberAndID(SoundQueue[i].SoundID).SE_ID;
			SoundQueueDebug[i].EnumName = SoundLookUp(SoundQueue[i].SoundID);
			SoundQueueDebug[i].PlayTime = SoundQueue[i].PlayTime;
			SoundQueueDebug[i].Flags = SoundQueue[i].Flags;
			SoundQueueDebug[i].VolumeCur = SoundQueue[i].CurrentVolume;
			SoundQueueDebug[i].VolumeMax = SoundQueue[i].MaxVolume;
		}
	}
	for (unsigned int i = 0; i < 36; i++)
	{
		if (SoundQueue[i].PlayTime == 0) SetDebugFontColor(0xFFBF0000);	
		else
		{
			if (SoundQueueDebug[i].Flags & 0x4000)
			{
				if (SoundQueueDebug[i].Flags & 0x1000) SetDebugFontColor(0xFF00FFFF);
				else SetDebugFontColor(0xFFBF00BF);
			}
			else if (SoundQueue[i].Panning != 0) SetDebugFontColor(0xFFFF7FB2);
			else if (SoundQueueDebug[i].Flags & 0x2000) SetDebugFontColor(0xFF7F4040);
			else if (SoundQueueDebug[i].Flags & 0x1000) SetDebugFontColor(0xFFBFBF00);
			else if (SoundQueueDebug[i].Flags & 0x100) SetDebugFontColor(0xFFFF7F00);
			else if (SoundQueueDebug[i].Flags & 0x200) SetDebugFontColor(0xFF00BF00);
			else SetDebugFontColor(0xFFBFBFBF);
		}
		PrintDebugNumber(NJM_LOCATION(1, i + 1 + 5), i, 2);
		if (SoundQueueDebug[i].Bank_ID != -1) DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%01X/%02i", SoundQueueDebug[i].Bank_ID, SoundQueueDebug[i].SE_ID, 4);
		if (SoundQueueDebug[i].EnumName != "") DisplayDebugStringFormatted(NJM_LOCATION(11, i + 1 + 5), SoundQueueDebug[i].EnumName, 4);
	}
	SetDebugFontColor(0xFFBF00BF);
	DisplayDebugStringFormatted(NJM_LOCATION(1, 43), "3D");
	SetDebugFontColor(0xFF00FFFF);
	DisplayDebugStringFormatted(NJM_LOCATION(4, 43), "3D+OLD");
	SetDebugFontColor(0xFFBFBF00);
	DisplayDebugStringFormatted(NJM_LOCATION(11, 43), "OLD");
	SetDebugFontColor(0xFFFF7F00);
	DisplayDebugStringFormatted(NJM_LOCATION(15, 43), "POS");
	SetDebugFontColor(0xFF00BF00);
	DisplayDebugStringFormatted(NJM_LOCATION(19, 43), "VOL");
	SetDebugFontColor(0xFFFF7FB2);
	DisplayDebugStringFormatted(NJM_LOCATION(23, 43), "PAN");
	SetDebugFontColor(0xFF7F4040);
	DisplayDebugStringFormatted(NJM_LOCATION(27, 43), "FRQ");
	SetDebugFontColor(0xFFBFBFBF);
}

static Sint32 __cdecl LoadPVM_r(const char* filename, NJS_TEXLIST* texlist);
static Trampoline LoadPVM_t(0x77FEB0, 0x77FEB6, LoadPVM_r);
static Sint32 __cdecl LoadPVM_r(const char* filename, NJS_TEXLIST* texlist)
{
	auto original = reinterpret_cast<decltype(LoadPVM_r)*>(LoadPVM_t.Target());
	Sint32 result = original(filename, texlist);
	PrintDebug("Loading PVM: %s result: %d\n", filename, result);
	return result;
}

static void __cdecl LoadSoundList_r(signed int a1);
static Trampoline LoadSoundList_t(0x4238E0, 0x4238E6, LoadSoundList_r);
static void __cdecl LoadSoundList_r(signed int a1)
{
	for (int i = 0; i < SoundLists[a1].Count; i++)
	{
		PrintDebug("Loaded Sounbank %d: %s (SoundList %d)\n", SoundLists[a1].List[i].Bank, SoundLists[a1].List[i].Filename, a1);
	}
	auto original = reinterpret_cast<decltype(LoadSoundList_r)*>(LoadSoundList_t.Target());
	original(a1);
}

static void __cdecl PlayVoice_r(int a1);
static Trampoline PlayVoice_t(0x425710, 0x425715, PlayVoice_r);
static void __cdecl PlayVoice_r(int a1)
{
	//PrintDebug("Play voice: %d, %s\n", a1, Voices[a1].Name);
	VoiceID = a1;
	auto original = reinterpret_cast<decltype(PlayVoice_r)*>(PlayVoice_t.Target());
	original(a1);
}

static void __cdecl FreezeFrameFilth_r();
static Trampoline FreezeFrameFilth_t(0x40C090, 0x40C096, FreezeFrameFilth_r);
static void __cdecl FreezeFrameFilth_r()
{
	auto original = reinterpret_cast<decltype(FreezeFrameFilth_r)*>(FreezeFrameFilth_t.Target());
	if (!FreezeFrame_Mode) original();
	else if (FreezeFrame_Mode == 1)
	{
		original();
		FreezeFrame_Mode = 2;
	}
	else if (FreezeFrame_Mode == 3)
	{
		original();
	}
}

void DrawDebugModel(NJS_MODEL_SADX* a1)
{
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	DrawVisibleModel_Queue(a1, QueuedModelFlagsB_SomeTextureThing);
}

void DrawCollisionInfo_Player(CollisionInfo* a1)
{
	Angle BackupRotation = a1->Object->Data1->Rotation.y;
	a1->Object->Data1->Rotation.y = -a1->Object->Data1->Rotation.y + 0x4000u;
	DrawCollisionInfo(a1);
	a1->Object->Data1->Rotation.y = BackupRotation;
}

static void __cdecl AddToCollisionListF_r(EntityData1* a1);
static Trampoline AddToCollisionListF_t(0x41C280, 0x41C285, AddToCollisionListF_r);
static void __cdecl AddToCollisionListF_r(EntityData1* a1)
	{
		auto original = reinterpret_cast<decltype(AddToCollisionListF_r)*>(AddToCollisionListF_t.Target());
		original(a1);
		if (CollisionDebug)
		{
			if (
			(a1 == EntityData1Ptrs[0] || 
			a1 == EntityData1Ptrs[1] || 
			a1 == EntityData1Ptrs[2] || 
			a1 == EntityData1Ptrs[3] || 
			a1 == EntityData1Ptrs[4] || 
			a1 == EntityData1Ptrs[5] || 
			a1 == EntityData1Ptrs[6] || 
			a1 == EntityData1Ptrs[7] ) 
			&& a1->CollisionInfo) DrawCollisionInfo_Player(a1->CollisionInfo);
		else if (a1->CollisionInfo) DrawCollisionInfo(a1->CollisionInfo);
	}
}

void SendDebugMessage(const char* msg)
{
	DebugMessageTimer = 60;
	DebugMessage = msg;
}

void __fastcall SetTextureHack(int index)
{
	if (TextureDebug && Direct3D_CurrentTexList != &DebugFontTexlist)
		Direct3D_SetNJSTexture(TexMemList_PixelFormat(&whitetexturetexinfo, 454535454));
	else
		stSetTexture(index);
}

static void __cdecl stSetTexture_Ocean_r(Uint32 a1);
static Trampoline stSetTexture_Ocean_t(0x403090, 0x403095, stSetTexture_Ocean_r);
static void __cdecl stSetTexture_Ocean_r(Uint32 a1)
{
	auto original = reinterpret_cast<decltype(stSetTexture_Ocean_r)*>(stSetTexture_Ocean_t.Target());
	if (TextureDebug && Direct3D_CurrentTexList != &DebugFontTexlist)
		Direct3D_SetNJSTexture(TexMemList_PixelFormat(&whitetexturetexinfo, 454535454));
	else
		original(a1);
}

// White texture for compatibility with Lantern when textures are disabled
static void InitializeWhiteTexture()
{
	whitetexturedata[0] = 0xFF;
	whitetexturedata[1] = 0xFF;
	whitetexturedata[2] = 0xFF;
	whitetexturedata[3] = 0xFF;
	whitetexturedata[4] = 0xFF;
	whitetexturedata[5] = 0xFF;
	whitetexturedata[6] = 0xFF;
	whitetexturedata[7] = 0xFF;
	njSetTextureInfo(&whitetexturetexinfo, (Uint16*)&whitetexturedata, NJD_TEXFMT_VQ | NJD_TEXFMT_RGB_565, 128, 128);
	njSetTextureNameEx(whitetextures, &whitetexturetexinfo, (void*)0xFFFFFFFE, NJD_TEXATTR_GLOBALINDEX | NJD_TEXATTR_TYPE_MEMORY);
	whitetextures[0].texaddr = (Uint32)TexMemList_PixelFormat(&whitetexturetexinfo, 237542221);
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions &helperFunctions)
	{
		InitializeWhiteTexture();
		//Fix model rendering for debug collision shapes
		WriteCall((void*)0x79EAC5, DrawDebugModel);
		WriteCall((void*)0x79EC11, DrawDebugModel);
		WriteCall((void*)0x79ED09, DrawDebugModel);
		WriteCall((void*)0x79F01C, DrawDebugModel);
		WriteCall((void*)0x79F17D, DrawDebugModel);
		WriteCall((void*)0x79EE1F, DrawDebugModel);
		WriteCall((void*)0x79EE76, DrawDebugModel);
		WriteCall((void*)0x79EECE, DrawDebugModel);
		WriteCall((void*)0x79F2CA, DrawDebugModel);
		WriteCall((void*)0x79F349, DrawDebugModel);
		WriteCall((void*)0x79F349, DrawDebugModel);
		WriteCall((void*)0x79F3C8, DrawDebugModel);
		WriteCall((void*)0x79F426, DrawDebugModel);
		/*WriteData<1>((char*)0x780872, 0x02u); //Expand memory for debug string allocation
		WriteData((int*)0x780897, 256); //Expand memory for debug string allocation
		WriteData((int*)0x780892, 4096); //Expand memory for debug string allocation*/
		WriteJump((void*)0x403070, njSetTexture_Hax);
		WriteCall((void*)0x44AF3B, RenderDeathPlanes);
		WriteData((signed char**)0x44AF32, &DeathPlanesEnabled);
		const IniFile *config = new IniFile(std::string(path) + "\\config.ini");
		EnableFontScaling = config->getBool("General", "EnableFontScaling", false);
		DebugSetting = config->getInt("General", "DefaultPage", 0);
		delete config;
		WriteJump((void*)0x77DDFD, SetTextureHack);
		WriteCall((void*)0x77E36C, SetTextureHack);
		WriteCall((void*)0x784A47, SetTextureHack);
		WriteCall((void*)0x78A382, SetTextureHack);
		WriteCall((void*)0x78A589, SetTextureHack);
		WriteCall((void*)0x78ECE3, SetTextureHack);
		init_FreeMovements();
	}
	
	__declspec(dllexport) void __cdecl OnInput()
	{
		// Info panels
		if (KeyboardKeys[KEY_1].pressed) DebugSetting = 1;
		if (KeyboardKeys[KEY_2].pressed) DebugSetting = 2;
		if (KeyboardKeys[KEY_3].pressed) DebugSetting = 3;
		if (KeyboardKeys[KEY_4].pressed) DebugSetting = 4;
		if (KeyboardKeys[KEY_5].pressed) DebugSetting = 5; 
		if (KeyboardKeys[KEY_6].pressed) DebugSetting = 6;
		if (KeyboardKeys[KEY_7].pressed) DebugSetting = 7;
		if (KeyboardKeys[KEY_8].pressed) DebugSetting = 8;
		if (KeyboardKeys[KEY_9].pressed) DebugSetting = 9;
		if (KeyboardKeys[KEY_0].pressed) DebugSetting = 0;
		// Texture toggle
		if (KeyboardKeys[KEY_T].pressed)
		{
			TextureDebug = !TextureDebug;
			SendDebugMessage(TextureDebug ? "TEXTURES: OFF" : "TEXTURES: ON ");
		}
		// Fog toggle
		if (KeyboardKeys[KEY_F].pressed)
		{
			FogToggle = !FogToggle;
			SendDebugMessage(FogToggle ? "FOG: ON " : "FOG: OFF");
		}
		// Collision toggle
		if (ControllerPointers[0]->PressedButtons & Buttons_C || KeyboardKeys[KEY_C].pressed)
		{
			CollisionDebug = !CollisionDebug;
			SendDebugMessage(CollisionDebug ? "COLLI DRAW: ON " : "COLLI DRAW: OFF");
		}
		// Cycle info panels
		if ((ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && !(ControllerPointers[0]->HeldButtons & Buttons_A))
		{
			DebugSetting++;
			if (DebugSetting > 9) DebugSetting = 0;
		}
		// Vanilla Debug Mode
		if ((ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && ControllerPointers[0]->HeldButtons & Buttons_A)
		{
			if (DebugMode)
			{
				DebugMode = 0;
				DeathPlanesEnabled = -1;
				if (EntityData1Ptrs[0] != nullptr) EntityData1Ptrs[0]->Action = 1;
			}
			else if (GameMode != GameModes_Menu)
			{
				DebugMode = 1;
				DeathPlanesEnabled = 1;
			}
			SendDebugMessage(DebugMode ? "DEBUG MODE: ON " : "DEBUG MODE: OFF");
		}
		// Update info panels
		switch (DebugSetting)
		{
			// Input
		case 4:
			UpdateKeys();
			UpdateButtons();
			break;
			// Sound
		case 6:
			if (KeyboardKeys[KEY_H].pressed) DisplaySoundIDMode++;
			if (DisplaySoundIDMode > 2) DisplaySoundIDMode = 0;
			// LS Palette
		case 8:
			if (KeyboardKeys[KEY_H].pressed) CurrentPalette++;
			if (CurrentPalettes[CurrentPalette] == -1) CurrentPalette = 0;
			// Stage Lights
		case 9:
			if (KeyboardKeys[KEY_H].pressed) CurrentStageLight++;
			if (CurrentLights[CurrentStageLight] == -1) CurrentStageLight = 0;
		}
		// Crash log toggle
		if (KeyboardKeys[KEY_P].pressed) CrashDebug = !CrashDebug;
		// Freeze frame/frame advance toggle
		if ((GameState != 0 && KeyboardKeys[KEY_PAUSEBREAK].pressed && !FreezeFrame_Mode) || FreezeFrame_Mode == 3)
		{
			BackupBytes[0] = Byte1;
			BackupBytes[1] = Byte2;
			WriteData<1>((char*)0x78BA50, 0xC3u);
			WriteData<1>((char*)0x78B880, 0xC3u);
			FreezeFrame_Mode = 1;
			FreezeFrame_Pressed = true;
			PauseAllSounds(0);
		}
		if (GameState != 0 && KeyboardKeys[KEY_PAUSEBREAK].pressed && FreezeFrame_Mode && !FreezeFrame_Pressed)
		{
			WriteData<1>((char*)0x78BA50, BackupBytes[0]);
			WriteData<1>((char*)0x78B880, BackupBytes[1]);
			FreezeFrame_Mode = 0;
			UnpauseAllSounds(0);
		}
		if (GameState != 0 && KeyboardKeys[KEY_INSERT].pressed)
		{
			if (FreezeFrame_Mode)
			{
				WriteData<1>((char*)0x78BA50, BackupBytes[0]);
				WriteData<1>((char*)0x78B880, BackupBytes[1]);
			}
			FreezeFrame_Mode = 3;
			UnpauseAllSounds(0);
		}
		FreezeFrame_Pressed = false;
		// Free cam toggle
		if (KeyboardKeys[KEY_Y].pressed)
		{
			FreeCamEnabled = !FreeCamEnabled;
			while (ShowCursor(true) < 0); // Increase cursor visibility until it shows
			SendDebugMessage(FreeCamEnabled ? "FREE CAMERA: ON " : "FREE CAMERA: OFF");
			// Center cursor first to avoid jittering after turning it on
			if (FreeCamEnabled)
			{
				int w = GetSystemMetrics(SM_CXSCREEN);
				int h = GetSystemMetrics(SM_CYSCREEN);
				SetCursorPos(w / 2, h / 2);
			}
		}
		// Increase free cam speed
		if (KeyboardKeys[KEY_NUM_ADD].held)
		{
			FreeCamSpeed = min(5.0f, FreeCamSpeed + 0.01f);
			sprintf_s(DebugMsgBuffer, "FREE CAM SPEED: %.02f", FreeCamSpeed);
			SendDebugMessage(DebugMsgBuffer);
		}
		// Decrease free cam speed
		if (KeyboardKeys[KEY_NUM_SUBTRACT].held)
		{
			FreeCamSpeed = max(0.1f, FreeCamSpeed - 0.01f);
			sprintf_s(DebugMsgBuffer, "FREE CAM SPEED: %.02f", FreeCamSpeed);
			SendDebugMessage(DebugMsgBuffer);
		}
		FreeCam_OnInput();
		// Speed Hack
		if (KeyboardKeys[KEY_END].pressed)
		{
			SpeedHack = !SpeedHack;
			SendDebugMessage(SpeedHack ? "SPEED HACK: ON" : "SPEED HACK: OFF");
			if (!SpeedHack && FrameIncrementCurrent != 1)
			{
				FrameIncrement = FrameIncrementCurrent = 1;
			}
		}
		if (KeyboardKeys[KEY_PAGEUP].pressed)
		{
			FrameIncrementCurrent += 1;
			SendDebugMessage("GAME SPEED UP");
		}
		else if (KeyboardKeys[KEY_PAGEDOWN].pressed)
		{
			FrameIncrementCurrent = max(1, FrameIncrementCurrent - 1);
			SendDebugMessage("GAME SPEED DOWN");
		}

	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		// Set up font
		BackupDebugFontSettings();
		ScaleDebugFont(16);
		// Apply speed hack
		if (SpeedHack)
			FrameIncrement = FrameIncrementCurrent;
		// Display data
		if (!MissedFrames)
		{
			if (CrashDebug)
			{
				SetDebugFontColor(0xFFFF0000);
				DisplayDebugStringFormatted(NJM_LOCATION(0, 0), "CRASH LOG ON");
				SetDebugFontColor(0xFFBFBFBF);
			}
			if (DebugMessageTimer && DebugMessage != NULL)
			{
				SetDebugFontColor(0xFFBFBFBF);
				int DebugRightPos = (int)((float)HorizontalResolution / DebugFontSize);
				DisplayDebugStringFormatted(NJM_LOCATION(DebugRightPos - strlen(DebugMessage), 0), DebugMessage);
				SetDebugFontColor(0xFFBFBFBF);
				DebugMessageTimer--;
			}
			switch (DebugSetting)
			{
			case 1: GameDebug(); break;
			case 2: PlayerDebug(); break;
			case 3: CameraDebug(); break;
			case 4: InputDebug(); break;
			case 5: FogDebug(); break;
			case 6: SoundDebug(); break;
			case 7: SoundBankInfoDebug(); break;
			case 8: LSPaletteDebug(); break;
			case 9: StageLightDebug(); break;
			}
		}
		// Reset stuff after character dies etc.
		if (DebugMode && (GameState == 7 || GameState == 3 || GameState == 4))
		{
			DeathPlanesEnabled = -1;
			DebugMode = 0;
			if (EntityData1Ptrs[0] != nullptr) EntityData1Ptrs[0]->Action = 1;
		}
		// Restore font setting after finishing drawing
		RestoreDebugFontSettings();
		// Free camera stuff
		FreeCam_OnFrame();
	}
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}