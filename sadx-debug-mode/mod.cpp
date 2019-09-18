#include <SADXModLoader.h>
#include "IniFile.hpp"
#include "Trampoline.h"
#include "Data.h"

static char DebugSetting = 0;
static int WaitFrames = -1;
static bool EnableFontScaling = false;

void DrawDebugRectangle(float leftchars, float topchars, float numchars_horz, float numchars_vert)
{
	float FontScale;
	float HorizontalResolution_float = (float)HorizontalResolution;
	float VerticalResolution_float = (float)VerticalResolution;
	if (!EnableFontScaling) FontScale = 1.0f;
	else
	{
		if (HorizontalResolution_float / VerticalResolution_float > 1.33f) FontScale = floor(VerticalResolution_float / 480.0f);
		else FontScale = floor(HorizontalResolution_float / 640.0f);
	}
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	if (DebugSetting == 6)
	{
		if (EnableFontScaling || HorizontalResolution < 1024) DrawRect_Queue(leftchars*FontScale*10.0f, topchars*FontScale*10.0f, numchars_horz*FontScale*10.0f, numchars_vert*FontScale*10.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
		else DrawRect_Queue(leftchars*FontScale*16.0f, topchars*FontScale*16.0f, numchars_horz*FontScale*16.0f, numchars_vert*FontScale*16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite); 
	}
	else DrawRect_Queue(leftchars*FontScale*16.0f, topchars*FontScale*16.0f, numchars_horz*FontScale*16.0f, numchars_vert*FontScale*16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

void DrawDebugText_NoFiltering(NJS_QUAD_TEXTURE_EX *quad)
{
	uint8_t Backup1 = TextureFilterSettingForPoint_1;
	uint8_t Backup2 = TextureFilterSettingForPoint_2;
	uint8_t Backup3 = TextureFilterSettingForPoint_3;
	WriteData((uint8_t*)0x0078B7C4, (uint8_t)0x01);
	WriteData((uint8_t*)0x0078B7D8, (uint8_t)0x01);
	WriteData((uint8_t*)0x0078B7EC, (uint8_t)0x01);
	Direct3D_TextureFilterPoint();
	Direct3D_DrawQuad(quad);
	WriteData((uint8_t*)0x0078B7C4, Backup1);
	WriteData((uint8_t*)0x0078B7D8, Backup2);
	WriteData((uint8_t*)0x0078B7EC, Backup3);
}

void ScaleDebugFont(int scale)
{
	float FontScale;
	float HorizontalResolution_float = (float)HorizontalResolution;
	float VerticalResolution_float = (float)VerticalResolution;
	if (!EnableFontScaling) FontScale = 1.0f;
	else
	{
		if (HorizontalResolution_float / VerticalResolution_float > 1.33f) FontScale = floor(VerticalResolution_float / 480.0f);
		else FontScale = floor(HorizontalResolution_float / 640.0f);
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
	DrawDebugRectangle(1.75f, 0.75f, 25, 23);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(6, 1), "- PLAYER INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %06d.%04d", (int)EntityData1Ptrs[0]->Position.x, abs(int(EntityData1Ptrs[0]->Position.x*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %06d.%04d", (int)EntityData1Ptrs[0]->Position.y, abs(int(EntityData1Ptrs[0]->Position.z*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %06d.%04d", (int)EntityData1Ptrs[0]->Position.z, abs(int(EntityData1Ptrs[0]->Position.z*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.x, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.y, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.z, (360.0f / 65535.0f) *(Uint16)EntityData1Ptrs[0]->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "ACTION: %03d", EntityData1Ptrs[0]->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ANIM: %03d", CharObj2Ptrs[0]->AnimationThing.Index);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %.2f", CharObj2Ptrs[0]->AnimationThing.Frame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "SPEED X: %.4f", CharObj2Ptrs[0]->Speed.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "SPEED Y: %.4f", CharObj2Ptrs[0]->Speed.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 17), "SPEED Z: %.4f", CharObj2Ptrs[0]->Speed.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "RINGS: %03d", Rings);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "LIVES: %03d", Lives);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "IDLE: %04d", CharObj2Ptrs[0]->IdleTime);
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
	DrawDebugRectangle(1.75f, 0.75f, 23, 18);
	DisplayDebugString(NJM_LOCATION(5, 1), "- CAMERA INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %06d.%04d", (int)Camera_Data1->Position.x, abs(int(Camera_Data1->Position.x*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %06d.%04d", (int)Camera_Data1->Position.y, abs(int(Camera_Data1->Position.z*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %06d.%04d", (int)Camera_Data1->Position.z, abs(int(Camera_Data1->Position.z*100)));
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.x, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.y, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.z, (360.0f / 65535.0f) *(Uint16)Camera_Data1->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "HZFOV: %06d / %03.0f", (Uint16)HorizontalFOV_BAMS, (360.0f / 65535.0f) *(Uint16)HorizontalFOV_BAMS);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ACTION: %02d", Camera_Data1->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %.2f", Camera_CurrentActionFrame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "MODE: %d", CameraType[3]);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "FLAGS: %X", camera_flags);	
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
	DrawDebugRectangle(1.75f, 0.75f, 22, 18);
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
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "CUTSCENE ID: %03d", CutsceneID);
}

void UpdateKeys()
{
	int CursorPos = 14;
	for (int i = 0; i < 256; i++)
	{
		if (KeyboardKeys[i].held)
		{
			DisplayDebugStringFormatted(NJM_LOCATION(CursorPos, 19), "%d ", i);
			CursorPos += 3;
		}
	}
}

void UpdateButtons()
{
	std::string ButtonsString = "";
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
	DisplayDebugStringFormatted(NJM_LOCATION(CursorPos, 12), ButtonsString.c_str());
}

void InputDebug()
{
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 21);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(6, 1), "- CONTROLLER INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "ANALOG1 X: %04d", ControllerPointers[0]->LeftStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "ANALOG1 Y: %04d", ControllerPointers[0]->LeftStickY);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "ANALOG2 X: %04d", ControllerPointers[0]->RightStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANALOG2 Y: %04d", ControllerPointers[0]->RightStickY);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "TRIGGER L: %03d", ControllerPointers[0]->LTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "TRIGGER R: %03d", ControllerPointers[0]->RTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "BUTTONS HELD:");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "BUTTONS RAW: %08X", ControllerPointers[0]->HeldButtons);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(7, 17), "- KEYBOARD INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "KEYS HELD:");
}

void SoundDebug()
{
	DrawDebugRectangle(0.25f, 0.75f, 63, 44);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	if (EnableFontScaling || HorizontalResolution < 1024) DisplayDebugString(NJM_LOCATION(12, 1), "- SOUND QUEUE -");
	else DisplayDebugString(NJM_LOCATION(24, 1), "- SOUND QUEUE -");
	if (!EnableFontScaling && HorizontalResolution >= 1024) ScaleDebugFont(16); else ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFFF00);
	DisplayDebugString(NJM_LOCATION(2, 4), "N   ID  INDEX   LENGTH  FLAG   VOLUME    PAN     3D    PITCH");
	SetDebugFontColor(0xFFBFBFBF);
	int ActiveSounds = 0;
	for (unsigned int i = 0; i < 35; i++)
	{
		if (SoundQueue[i].PlayLength == 0)
		{
			SetDebugFontColor(0xFFBF0000);
		}
		else
		{
			SetDebugFontColor(0xFFBFBFBF);
			ActiveSounds++;
		}
		PrintDebugNumber(NJM_LOCATION(1, i + 1 + 5), i, 2);
		if (SoundQueue[i].SoundID != -1) PrintDebugNumber(NJM_LOCATION(5, i + 1+ 5), SoundQueue[i].SoundID, 4);
		if (SoundQueue[i].SoundID != -1) PrintDebugNumber(NJM_LOCATION(11, i + 1+ 5), SoundQueue[i].MaxIndex, 4);
		if (SoundQueue[i].PlayLength != 0) PrintDebugNumber(NJM_LOCATION(19, i + 1+ 5), SoundQueue[i].PlayLength, 4);
		if (SoundQueue[i].Flags != 0) DisplayDebugStringFormatted(NJM_LOCATION(26, i + 1+ 5), "%04X", SoundQueue[i].Flags);
		if (SoundQueue[i].VolumeA != 0 || SoundQueue[i].VolumeB != 0) DisplayDebugStringFormatted(NJM_LOCATION(32, i + 1+ 5), "%04i/%04i", SoundQueue[i].VolumeA, SoundQueue[i].VolumeB);
		if (SoundQueue[i].Panning != 0) DisplayDebugStringFormatted(NJM_LOCATION(43, i + 1+ 5), "%04i", SoundQueue[i].Panning);
		if (SoundQueue[i].NoIndex != 0) DisplayDebugStringFormatted(NJM_LOCATION(48, i + 1+ 5), "%04X", SoundQueue[i].NoIndex);
		if (SoundQueue[i].PitchShift != 0) DisplayDebugStringFormatted(NJM_LOCATION(57, i + 1+ 5), "%05i", SoundQueue[i].PitchShift);
		if (SoundQueue[i].null_2 != 0) DisplayDebugStringFormatted(NJM_LOCATION(90, i + 1+ 5), "%04X", SoundQueue[i].null_2);
	}
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(2, 42), "ACTIVE SOUNDS: %d", ActiveSounds);
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions &helperFunctions)
	{
		const IniFile *config = new IniFile(std::string(path) + "\\config.ini");
		EnableFontScaling = config->getBool("General", "EnableFontScaling", false);
		if (GetModuleHandle(L"DLCs_Main") == nullptr) WriteCall((void*)0x77E9E4, DrawDebugText_NoFiltering);
		delete config;
	}
	__declspec(dllexport) void __cdecl OnInput()
	{
		if (WaitFrames > 0)
		{
			WaitFrames--;
		}
		else if (WaitFrames == 0)
		{
			ControllerPointers[0]->PressedButtons |= Buttons_A;
			ControllerPointers[0]->HeldButtons |= Buttons_A;
			WaitFrames = -1;
		}
		if ((ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && !(ControllerPointers[0]->HeldButtons & Buttons_A))
		{
			DebugSetting++;
			if (DebugSetting > 6) DebugSetting = 0;
		}
		if ((ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && ControllerPointers[0]->HeldButtons & Buttons_A)
		{
			if (DebugMode)
			{
				DebugMode = 0;
				WaitFrames = 5;
			}
			else if (CurrentCharacter != Characters_Gamma) DebugMode = 1;
		}		
		if (DebugSetting == 4)
		{
			UpdateKeys();
			UpdateButtons();
		}
	}
	__declspec(dllexport) void __cdecl OnFrame()
	{
		ScaleDebugFont(true);
		if (DebugSetting == 1) GameDebug();
		if (DebugSetting == 2) PlayerDebug();
		if (DebugSetting == 3) CameraDebug();
		if (DebugSetting == 4) InputDebug();
		if (DebugSetting == 5) FogDebug();
		if (DebugSetting == 6) SoundDebug();
	}
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}