#pragma once

#pragma warning(push)
#pragma warning(disable: 4267 4838)

extern bool EnableFontScaling;
extern int CurrentPalette;
extern int LSPaletteArray[];
extern int DisplaySoundIDMode;
extern int CurrentStageLight;
extern int CurrentLights[];
extern char DebugSetting;
extern int DebugMessageTimer;
extern const char* DebugMessage;

void BackupDebugFontSettings();
void RestoreDebugFontSettings();
void ScaleDebugFont(int scale);
void DrawDebugRectangle(float leftchars, float topchars, float numchars_horz, float numchars_vert);
void SendDebugMessage(const char* msg);

struct EnvLightData
{
	char level;
	char act;
	char index;
	char use_direction;
	NJS_VECTOR direction;
	float spe;
	float dif;
	float r;
	float g;
	float b;
	float amb_r;
	float amb_g;
	float amb_b;
};

enum Keys
{
	KEY_ESCAPE = 41,
	KEY_F1 = 58,
	KEY_F2 = 59,
	KEY_F3 = 60,
	KEY_F4 = 61,
	KEY_F5 = 62,
	KEY_F6 = 63,
	KEY_F7 = 64,
	KEY_F8 = 65,
	KEY_F9 = 66,
	KEY_F10 = 67,
	KEY_F11 = 68,
	KEY_F12 = 69,
	KEY_PRINTSCREEN = 70,
	KEY_SCROLLLOCK = 71,
	KEY_PAUSEBREAK = 72,
	KEY_1 = 30,
	KEY_2 = 31,
	KEY_3 = 32,
	KEY_4 = 33,
	KEY_5 = 34,
	KEY_6 = 35,
	KEY_7 = 36,
	KEY_8 = 37,
	KEY_9 = 38,
	KEY_0 = 39,
	KEY_SUBTRACT = 45,
	KEY_EQUAL = 46,
	KEY_BACKSPACE = 42,
	KEY_INSERT = 73,
	KEY_HOME = 74,
	KEY_PAGEUP = 75,
	KEY_NUMLOCK = 83,
	KEY_NUM_SLASH = 84,
	KEY_NUM_MULTIPLY = 85,
	KEY_NUM_SUBTRACT = 86,
	KEY_TAB = 43,
	KEY_Q = 20,
	KEY_W = 26,
	KEY_E = 8,
	KEY_R = 21,
	KEY_T = 23,
	KEY_Y = 28,
	KEY_U = 24,
	KEY_I = 12,
	KEY_O = 18,
	KEY_P = 19,
	KEY_DELETE = 76,
	KEY_END = 77,
	KEY_PAGEDOWN = 78,
	KEY_NUM7_HOME = 95,
	KEY_NUM8_UP = 96,
	KEY_NUM9_PAGEUP = 97,
	KEY_NUM_ADD = 87,
	KEY_A = 4,
	KEY_S = 22,
	KEY_D = 7,
	KEY_F = 9,
	KEY_G = 10,
	KEY_H = 11,
	KEY_J = 13,
	KEY_K = 14,
	KEY_L = 15,
	KEY_SEMICOLON = 51,
	KEY_APOSTROPHE = 52,
	KEY_ENTER =  40,
	KEY_NUM4_LEFT = 92,
	KEY_NUM5 = 93,
	KEY_NUM6_RIGHT = 94,
	KEY_Z = 29,
	KEY_X = 27,
	KEY_C = 6,
	KEY_V = 25,
	KEY_B = 5,
	KEY_N = 17,
	KEY_M = 16,
	KEY_COMMA = 54,
	KEY_PERIOD = 55,
	KEY_SLASH = 56,
	KEY_UP = 82,
	KEY_NUM1_END = 89,
	KEY_NUM2_DOWN = 90,
	KEY_NUM3_PAGEDOWN = 91,
	KEY_SPACE = 44,
	KEY_LEFT = 80,
	KEY_DOWN = 81,
	LEY_RIGHT = 79,
	KEY_NUM0 = 98,
	//NV keys are added by the input mod
	KEY_NV_TILDE = 99,
	KEY_NV_LEFTSQUAREBRACKET = 100,
	KEY_NV_RIGHTSQUAREBRACKET = 101,
	KEY_NV_BACKSLASH = 102,
	KEY_NV_CAPSLOCK = 103,
	KEY_NV_LEFTSHIFT = 104,
	KEY_NV_RIGHTSHIFT = 105,
	KEY_NV_LEFTCTRL = 106,
	KEY_NV_LEFTALT = 107,
	KEY_NV_RIGHTALT = 108,
	KEY_NV_MENU = 109,
	KEY_NV_RIGHTCTRL = 110,
	KEY_NV_NUM_DELETE = 111,
	KEY_NV_NUM_ENTER = 112,
};

struct SoundEntry
{
	int Priority; //-1 for top priority
	int PlayTime; //-1 to loop indefinitely
	void* SourceEntity; //Sound stops when entity is destroyed
	int Flags; //ANDs with 1, 10, 100, 200, 800, 2000, 4000
	int SoundID;
	int Panning;
	int CurrentVolume;
	int MaxVolume; //Sometimes an entity pointer is put here for 3D sounds
	int PitchShift;
	NJS_VECTOR pos;
	int qnum; //Set to -1 for 3D sounds
	int banknum; //Unused
};

struct KeyboardKey
{
	char held;
	char old;
	char pressed;
};

DataArray(KeyboardKey, KeyboardKeys, 0x3B0E3E0, 256);
DataPointer(KeyboardKey, Key_B, 0x03B0E3EF);
DataArray(SoundEntry, SoundQueue, 0x3B292F8, 35);
DataPointer(EnvLightData*, StageLights, 0x40A8A2);
DataPointer(int, CutsceneID, 0x3B2C570);
DataPointer(char, CurrentCharacterSelection, 0x3B2A2FD);
DataPointer(uint8_t, TextureFilterSettingForPoint_1, 0x0078B7C4);
DataPointer(uint8_t, TextureFilterSettingForPoint_2, 0x0078B7D8);
DataPointer(uint8_t, TextureFilterSettingForPoint_3, 0x0078B7EC);
DataPointer(int, CurrentCutsceneCode, 0x3B2C568);
DataPointer(char, Byte1, 0x78BA50);
DataPointer(char, Byte2, 0x78B880);
DataPointer(int, FogToggle, 0x3ABDC6C);
DataPointer(NJS_TEXLIST, DebugFontTexlist, 0x38A5CF8);
#pragma warning(pop)