#include <SADXModLoader.h>
#include "IniFile.hpp"
#include "Trampoline.h"
#include "Data.h"
#include "FreeCam.h"
#include "FreeMovement.h"
#include "Lantern.h"
#include "Keys.h"

void LSPaletteInfo();
void SoundInfo();
void SoundBankInfo();
void StageLightInfo();
void LanternPaletteInfo();
void SetLanternDebugPalette(LanternDebugPaletteType type);
void LanternDebug_OnFrame();

const HelperFunctions* helperFunctionsGlobal;

FunctionPointer(void, DrawCollisionInfo, (CollisionInfo* collision), 0x79F4D0);
FastcallFunctionPointer(void, stSetTexture, (int index), 0x0078D140);

DataArray(KeyboardKey, KeyboardKeys, 0x3B0E3E0, 256);
DataPointer(KeyboardKey, Key_B, 0x03B0E3EF);
DataPointer(NJS_TEXLIST, DebugFontTexlist, 0x38A5CF8);
DataPointer(int, CurrentCutsceneCode, 0x3B2C568);
DataPointer(int, CutsceneID, 0x3B2C570);
DataPointer(char, CurrentCharacterSelection, 0x3B2A2FD);
DataPointer(char, FreezeFrameByte1, 0x78BA50);
DataPointer(char, FreezeFrameByte2, 0x78B880);
DataPointer(char, CutsceneFramerateMode, 0x00431488);

char DebugSetting = 0; // Menu ID
bool FreeCamEnabled = false;

bool LanternLoaded;
static bool FogEnable = true;
static bool CollisionDebug = false;
static Sint8 DeathPlanesEnabled = -1;

static bool TextureDebug = false;
static int CurTexList_Current = 0;

static bool CrashDebug = false;
static bool AngleHexadecimal = false;
static int VoiceID = -1;

static bool SpeedHack = false;
static int FrameIncrementCurrent = 1;
static bool FreezeFrame_Pressed = false;
static int FreezeFrame_Mode = 0;
static Uint8 FreezeFrameBackupBytes[] = { 0xC3u, 0xC3u };

bool LanternDebug = false;
LanternDebugPaletteType LanternDebugMode = LanternDebugPaletteType::None;

static char DebugMsgBuffer[32];
static std::string FreeCamModeStrings[] = { "OFF", "LOOK", "MOVE", "ZOOM", "LOCKED" };

// White texture for texture override
static Uint8 whitetexturedata[6144];
static NJS_TEXINFO whitetexturetexinfo;
static NJS_TEXNAME whitetextures[1];
static NJS_TEXLIST whitetexturetexlist;

void UpdateKeys()
{
	int KeysHeld = 0;
	int CursorPosX1 = 14;
	int CursorPosY1 = 21;
	int CursorPosX2 = 14;
	int CursorPosY2 = 23;
	for (int i = 1; i < 256; i++) // Exclude key 0 which is always pressed
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
		ButtonsString += "A ";
	if (ControllerPointers[0]->HeldButtons & Buttons_B)
		ButtonsString += "B ";
	if (ControllerPointers[0]->HeldButtons & Buttons_C)
		ButtonsString += "C ";
	if (ControllerPointers[0]->HeldButtons & Buttons_D)
		ButtonsString += "D ";
	if (ControllerPointers[0]->HeldButtons & Buttons_X)
		ButtonsString += "X ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Y)
		ButtonsString += "Y ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Z)
		ButtonsString += "Z ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Start)
		ButtonsString += "START ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Up)
		PadString += "UP ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Down)
		PadString += "DOWN ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Left)
		PadString += "LEFT ";
	if (ControllerPointers[0]->HeldButtons & Buttons_Right)
		PadString += "RIGHT ";
	if (PadString == "") 
		PadString = "CENTER";
	BackupDebugFontSettings();
	SetDebugFontSize(16);
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(CursorPos, 12), ButtonsString.c_str());
	DisplayDebugStringFormatted(NJM_LOCATION(18, 14), PadString.c_str());
	RestoreDebugFontSettings();
}

void PlayerInfo()
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
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), AngleHexadecimal ? "ANG X: %04X / %03.0f" : "ANG X: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.x, (360.0f / 65535.0f) * (Uint16)EntityData1Ptrs[0]->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), AngleHexadecimal ? "ANG Y: %04X / %03.0f" : "ANG Y: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.y, (360.0f / 65535.0f) * (Uint16)EntityData1Ptrs[0]->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), AngleHexadecimal ? "ANG Z: %04X / %03.0f" : "ANG Z: %06d / %03.0f", (Uint16)EntityData1Ptrs[0]->Rotation.z, (360.0f / 65535.0f) * (Uint16)EntityData1Ptrs[0]->Rotation.z);
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

void CameraInfo()
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
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), AngleHexadecimal ? "ANG X: %04X / %03.0f" : "ANG X: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.x, (360.0f / 65535.0f) * (Uint16)Camera_Data1->Rotation.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), AngleHexadecimal ? "ANG Y: %04X / %03.0f" : "ANG Y: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.y, (360.0f / 65535.0f) * (Uint16)Camera_Data1->Rotation.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 9), AngleHexadecimal ? "ANG Z: %04X / %03.0f" : "ANG Z: %06d / %03.0f", (Uint16)Camera_Data1->Rotation.z, (360.0f / 65535.0f) * (Uint16)Camera_Data1->Rotation.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), AngleHexadecimal ? "HZFOV: %04X / %03.0f" : "HZFOV: %06d / %03.0f", (Uint16)HorizontalFOV_BAMS, (360.0f / 65535.0f) * (Uint16)HorizontalFOV_BAMS);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "ACTION: %02d", Camera_Data1->Action);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "FRAME: %.2f", Camera_CurrentActionFrame);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "MODE: %d", CameraType[3]);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "FLAGS: %X", camera_flags);	
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "FREE CAM: %s", FreeCamModeStrings[FreeCamMode].c_str());
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "CAM SPEED: %.2f", FreeCamSpeed);
}

void FogInfo()
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

void GameInfo()
{
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	DrawDebugRectangle(1.75f, 0.75f, 22, 22.5f);
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
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "CURR MUSIC  : %d", CurrentSong);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "LAST MUSIC  : %d", LastSong);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "LAST VOICE  : %d", VoiceID);
}

void InputInfo()
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
	if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f) 
		FontScale = floor((float)VerticalResolution / 480.0f) * 0.5f;
	else 
		FontScale = floor((float)HorizontalResolution / 640.0f) * 0.5f;
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
	switch (FreezeFrame_Mode)
	{
	case 0:
		original();
		break;
	case 1:
		original();
		FreezeFrame_Mode = 2;
		break;
	}
}

void DrawDebugModel(NJS_MODEL_SADX* a1)
{
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	DrawVisibleModel_Queue(a1, QueuedModelFlagsB_SomeTextureThing);
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
			&& a1->CollisionInfo) 
				DrawCollisionInfo_Player(a1->CollisionInfo);
		else if (a1->CollisionInfo) 
				DrawCollisionInfo(a1->CollisionInfo);
	}
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
	whitetexturetexlist.nbTexture = 1;
	whitetexturetexlist.textures = whitetextures;
}

void PaletteInfo()
{
	if (LanternLoaded)
		LanternPaletteInfo();
	else
		LSPaletteInfo();
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		LanternLoaded = GetModuleHandle(L"sadx-dc-lighting") != nullptr;
		helperFunctionsGlobal = &helperFunctions;
		InitializeWhiteTexture();
		// Fix model rendering for debug collision shapes
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
		/*WriteData<1>((char*)0x780872, 0x02u); // Expand memory for debug string allocation
		WriteData((int*)0x780897, 256); // Expand memory for debug string allocation
		WriteData((int*)0x780892, 4096); // Expand memory for debug string allocation*/
		WriteJump((void*)0x403070, njSetTexture_Hax);
		WriteCall((void*)0x44AF3B, RenderDeathPlanes);
		WriteData((signed char**)0x44AF32, &DeathPlanesEnabled);
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		EnableFontScaling = config->getBool("General", "EnableFontScaling", false);
		DebugSetting = config->getInt("General", "DefaultPage", 0);
		delete config;
		WriteJump((void*)0x77DDFD, SetTextureHack);
		WriteCall((void*)0x77E36C, SetTextureHack);
		WriteCall((void*)0x784A47, SetTextureHack);
		WriteCall((void*)0x78A382, SetTextureHack);
		WriteCall((void*)0x78A589, SetTextureHack);
		WriteCall((void*)0x78ECE3, SetTextureHack);
		InitFreeMovement();
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		// Info panels
		if (KeyboardKeys[KEY_1].pressed)
			DebugSetting = 1;
		if (KeyboardKeys[KEY_2].pressed)
			DebugSetting = 2;
		if (KeyboardKeys[KEY_3].pressed)
			DebugSetting = 3;
		if (KeyboardKeys[KEY_4].pressed)
			DebugSetting = 4;
		if (KeyboardKeys[KEY_5].pressed)
			DebugSetting = 5;
		if (KeyboardKeys[KEY_6].pressed)
			DebugSetting = 6;
		if (KeyboardKeys[KEY_7].pressed)
			DebugSetting = 7;
		if (KeyboardKeys[KEY_8].pressed)
			DebugSetting = 8;
		if (KeyboardKeys[KEY_9].pressed)
			DebugSetting = 9;
		if (KeyboardKeys[KEY_0].pressed)
			DebugSetting = 0;
		// Texture toggle
		if (KeyboardKeys[KEY_T].pressed)
		{
			TextureDebug = !TextureDebug;
			SendDebugMessage(TextureDebug ? "TEXTURES: OFF" : "TEXTURES: ON ");
		}
		// Fog toggle
		if (KeyboardKeys[KEY_F].pressed)
		{
			FogEnable = !FogEnable;
			gFog.u8Enable = (Uint8)FogEnable;
			SendDebugMessage(gFog.u8Enable ? "FOG: ON " : "FOG: OFF");
		}
		// Collision toggle
		if (ControllerPointers[0]->PressedButtons & Buttons_C || KeyboardKeys[KEY_C].pressed)
		{
			CollisionDebug = !CollisionDebug;
			SendDebugMessage(CollisionDebug ? "COLLI DRAW: ON " : "COLLI DRAW: OFF");
		}
		// Lantern debug toggle
		if (KeyboardKeys[KEY_L].pressed && LanternLoaded)
		{
			LanternDebug = !LanternDebug;
			if (LanternDebug)
				LanternDebugMode = (LanternDebugPaletteType)(max((int)LanternDebugMode, 1));
			SetLanternDebugPalette(LanternDebug ? LanternDebugMode : LanternDebugPaletteType::None);
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
			// Player and camera
		case 2:
		case 3:
			if (KeyboardKeys[KEY_H].pressed) AngleHexadecimal = !AngleHexadecimal;
			break;
			// Input
		case 4:
			UpdateKeys();
			UpdateButtons();
			break;
			// Sound
		case 6:
			if (KeyboardKeys[KEY_H].pressed) DisplaySoundIDMode++;
			if (DisplaySoundIDMode > 2) DisplaySoundIDMode = 0;
			// LS Palette / Lantern
		case 8:
			if (KeyboardKeys[KEY_H].pressed)
			{
				if (LanternDebug)
				{
					LanternDebugMode = (LanternDebugPaletteType)((int)LanternDebugMode + 1);
					if (LanternDebugMode > LanternDebugPaletteType::Fullbright)
						LanternDebugMode = LanternDebugPaletteType::Selection;
					SetLanternDebugPalette(LanternDebugMode);
				}
				else
				{
					CurrentPalette++;
					if (LSPaletteArray[CurrentPalette] == -1) CurrentPalette = 0;
				}
			}
			// Stage Lights
		case 9:
			if (KeyboardKeys[KEY_H].pressed) CurrentStageLight++;
			if (CurrentStageLight > 3) CurrentStageLight = 0;
		}
		// Crash log toggle
		if (KeyboardKeys[KEY_P].pressed)
			CrashDebug = !CrashDebug;
		// Freeze frame/frame advance toggle
		if ((GameState != 0 && KeyboardKeys[KEY_PAUSEBREAK].pressed && !FreezeFrame_Mode) || FreezeFrame_Mode == 3)
		{
			FreezeFrameBackupBytes[0] = FreezeFrameByte1;
			FreezeFrameBackupBytes[1] = FreezeFrameByte2;
			WriteData<1>((char*)0x78BA50, 0xC3u);
			WriteData<1>((char*)0x78B880, 0xC3u);
			FreezeFrame_Mode = 1;
			FreezeFrame_Pressed = true;
			PauseAllSounds(0);
		}
		if (GameState != 0 && KeyboardKeys[KEY_PAUSEBREAK].pressed && FreezeFrame_Mode && !FreezeFrame_Pressed)
		{
			WriteData<1>((char*)0x78BA50, FreezeFrameBackupBytes[0]);
			WriteData<1>((char*)0x78B880, FreezeFrameBackupBytes[1]);
			FreezeFrame_Mode = 0;
			UnpauseAllSounds(0);
		}
		if (GameState != 0 && KeyboardKeys[KEY_INSERT].pressed)
		{
			if (FreezeFrame_Mode)
			{
				WriteData<1>((char*)0x78BA50, FreezeFrameBackupBytes[0]);
				WriteData<1>((char*)0x78B880, FreezeFrameBackupBytes[1]);
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
				if (current_event != -1 && CutsceneFramerateMode == 2)
					dsInitInt(2, 1);
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
		{
			FrameIncrement = FrameIncrementCurrent;
			if (current_event != -1 && CutsceneFramerateMode == 2)
			{
				if (FrameIncrementCurrent == 1)
					dsInitInt(2, 1);
				else
					njSetWaitVsyncCount(2);
			}
		}
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
			case 1: GameInfo(); break;
			case 2: PlayerInfo(); break;
			case 3: CameraInfo(); break;
			case 4: InputInfo(); break;
			case 5: FogInfo(); break;
			case 6: SoundInfo(); break;
			case 7: SoundBankInfo(); break;
			case 8: PaletteInfo(); break;
			case 9: StageLightInfo(); break;
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
		// Lantern debug
		LanternDebug_OnFrame();
	}
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}