#include <SADXModLoader.h>
#include "IniFile.hpp"
#include "Trampoline.h"
#include "Data.h"

static char DebugSetting = 0;
static int WaitFrames = 0;
static bool DisableFontScaling = true;

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
	if (DisableFontScaling) FontScale = 1.0f;
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
		DisplayDebugString(NJM_LOCATION(2, 1), "- PLAYER INFO UNAVAILABLE -");
		return;
	}
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 1), "- PLAYER INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %f", EntityData1Ptrs[0]->Position.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %f", EntityData1Ptrs[0]->Position.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %f", EntityData1Ptrs[0]->Position.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %d", EntityData1Ptrs[0]->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %d", EntityData1Ptrs[0]->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %d", EntityData1Ptrs[0]->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "ACTION: %d", EntityData1Ptrs[0]->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ANIM: %d", CharObj2Ptrs[0]->AnimationThing.Index);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %f", CharObj2Ptrs[0]->AnimationThing.Frame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "SPEED X: %f", CharObj2Ptrs[0]->Speed.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "SPEED Y: %f", CharObj2Ptrs[0]->Speed.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 17), "SPEED Z: %f", CharObj2Ptrs[0]->Speed.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "RINGS: %03d", Rings);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "IDLE: %d", CharObj2Ptrs[0]->IdleTime);
}

void CameraDebug()
{
	ScaleDebugFont(16);
	if (Camera_Data1 == nullptr)
	{
		DisplayDebugString(NJM_LOCATION(2, 1), "- CAMERA INFO UNAVAILABLE -");
		return;
	}
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 1), "- CAMERA INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "X: %f", Camera_Data1->Position.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "Y: %f", Camera_Data1->Position.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "Z: %f", Camera_Data1->Position.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "ANG X: %d", Camera_Data1->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "ANG Y: %d", Camera_Data1->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "ANG Z: %d", Camera_Data1->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "FOV: %d", HorizontalFOV_BAMS);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ACTION: %d", Camera_Data1->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %f", Camera_CurrentActionFrame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "MODE: %d", CameraType[3]);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "FLAGS: %X", camera_flags);	
}

void FogDebug()
{
	NJS_COLOR FogColor;
	FogColor.color = LevelFogData.Color;
	ScaleDebugFont(16);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 1), "- FOG INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "ENABLED: %d", LevelFogData.Toggle);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "DIST MIN: %f", LevelFogData.Layer);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "DIST MAX: %f", LevelFogData.Distance);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "COLOR: R%d G%d B%d A%d", FogColor.argb.r, FogColor.argb.g, FogColor.argb.b, FogColor.argb.a);
	SetDebugFontColor(LevelFogData.Color);
	DisplayDebugStringFormatted(NJM_LOCATION(30, 6), "COLOR");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 9), "- SKYBOX INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "SCALE X: %f", Skybox_Scale.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "SCALE Y: %f", Skybox_Scale.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "SCALE Z: %f", Skybox_Scale.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "DIST MIN: %f", SkyboxDrawDistance.Minimum);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "DIST MAX: %f", SkyboxDrawDistance.Maximum);
	DisplayDebugString(NJM_LOCATION(2, 19), "- DRAW DISTANCE INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "DIST MIN: %f", LevelDrawDistance.Minimum);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 22), "DIST MAX: %f", LevelDrawDistance.Maximum);
}

void GameDebug()
{
	ScaleDebugFont(16);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 1), "- GAME INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "FRAME: %d", FrameCounter);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "UNPAUSED: %d", FrameCounterUnpaused);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 5), "LEVEL: %d", LevelFrameCount);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "GAME MODE: %d", GameMode);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "GAME STATE: %d", GameState);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "CHARACTER: %d", CurrentCharacter);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "CHAR SEL: %d", CurrentCharacterSelection);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "LEVEL: %d", CurrentLevel);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ACT: %d", CurrentAct);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "CHAO STAGE: %d", CurrentChaoStage);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "CUTSCENE ID: %d", CutsceneID);
}

void UpdateKeys()
{
	int CursorPos = 18;
	for (int i = 0; i < 256; i++)
	{
		if (KeyboardKeys[i].held)
		{
			DisplayDebugStringFormatted(NJM_LOCATION(CursorPos, 24), "%d", i);
			CursorPos += 4;
		}
	}
}

void InputDebug()
{
	ScaleDebugFont(16);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 1), "- CONTROLLER INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "ANALOG X: %d", ControllerPointers[0]->LeftStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "ANALOG Y: %d", ControllerPointers[0]->LeftStickY);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "TRIGGER L: %d", ControllerPointers[0]->LTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "TRIGGER R: %d", ControllerPointers[0]->RTriggerPressure);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), "BUTTON A: %d", ControllerPointers[0]->HeldButtons & Buttons_A);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "BUTTON B: %d", ControllerPointers[0]->HeldButtons & Buttons_B);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "BUTTON X: %d", ControllerPointers[0]->HeldButtons & Buttons_X);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "BUTTON Y: %d", ControllerPointers[0]->HeldButtons & Buttons_Y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "BUTTON Z: %d", ControllerPointers[0]->HeldButtons & Buttons_Z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "BUTTON C: %d", ControllerPointers[0]->HeldButtons & Buttons_C);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "BUTTON D: %d", ControllerPointers[0]->HeldButtons & Buttons_D);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "ANALOG2 X: %d", ControllerPointers[0]->RightStickX);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "ANALOG2 Y: %d", ControllerPointers[0]->RightStickY);
	DisplayDebugString(NJM_LOCATION(2, 22), "- KEYBOARD INFO -");
	DisplayDebugStringFormatted(NJM_LOCATION(3, 24), "PRESSED KEYS:");

}

void SoundDebug()
{
	ScaleDebugFont(16);
	DisplayDebugString(NJM_LOCATION(2, 1), "- SOUND INFO -");
	if (DisableFontScaling && HorizontalResolution >= 1024) ScaleDebugFont(16); else ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugString(NJM_LOCATION(2, 4), "N   ID  INDEX   LENGTH  FLAG   VOLUME    PAN     3D    PITCH");
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
		DisableFontScaling = config->getBool("General", "DisableFontScaling", true);
		if (GetModuleHandle(L"DLCs_Main") == nullptr) WriteCall((void*)0x77E9E4, DrawDebugText_NoFiltering);
		delete config;
	}
	__declspec(dllexport) void __cdecl OnInput()
	{
		if (WaitFrames > 0) WaitFrames--;
		if ((ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && !(ControllerPointers[0]->HeldButtons & Buttons_A))
		{
			DebugSetting++;
			if (DebugSetting > 6) DebugSetting = 0;
		}
		if (WaitFrames <= 0 && (ControllerPointers[0]->PressedButtons & Buttons_Z || Key_B.pressed) && ControllerPointers[0]->HeldButtons & Buttons_A)
		{
			if (DebugMode)
			{
				DebugMode = 0;
				ControllerPointers[0]->PressedButtons |= Buttons_A;
			}
			else DebugMode = 1;
			WaitFrames = 5;
		}		
		if (DebugSetting == 4) UpdateKeys();
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