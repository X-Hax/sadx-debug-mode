#pragma once
#include <SADXModLoader.h>

enum class LanternDebugPaletteType : int
{
	None = 0,
	Selection = 1,
	SelectionFull = 2,
	Direction = 3,
	Index = 4,
	Fullbright = 5
};

struct ColorPair
{
	NJS_COLOR diffuse;
	NJS_COLOR specular;
};

DataArray(ColorPair[256], LSPAL, 0x3B12210, 10);

extern LanternDebugPaletteType LanternDebugMode;
extern bool LanternDebug;