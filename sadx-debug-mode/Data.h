#pragma once

#pragma warning(push)
#pragma warning(disable: 4267 4838)

struct SoundEntry
{
	int MaxIndex;
	int PlayLength; //0xFFFFFFFF to loop indefinitely
	void *SourceEntity; //Sound stops when entity is destroyed
	int Flags; //3D sound related, ANDs with 1, 10, 100, 200, 800, 2000, 4000, 0x11 in PlaySound2, 0x801 in PlaySound, | 0x10 in DualEntity
	int SoundID;
	int Panning;
	int VolumeA;
	int VolumeB;
	int PitchShift;
	NJS_VECTOR origin;
	int NoIndex; //Only used when a free index can't be found (set to 0xFFFFFFFF)
	float null_2; //Unused
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
DataPointer(int, CutsceneID, 0x3B2C570);
DataPointer(char, CurrentCharacterSelection, 0x3B2A2FD);
DataPointer(uint8_t, TextureFilterSettingForPoint_1, 0x0078B7C4);
DataPointer(uint8_t, TextureFilterSettingForPoint_2, 0x0078B7D8);
DataPointer(uint8_t, TextureFilterSettingForPoint_3, 0x0078B7EC);

#pragma warning(pop)