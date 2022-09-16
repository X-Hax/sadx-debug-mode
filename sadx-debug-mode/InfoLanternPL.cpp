#include <SADXModLoader.h>
#include "Data.h"
#include "Lantern.h"
#include <fstream>
#include <vector>

static NJS_COLOR DiffusePalettes[10][256];
static NJS_COLOR SpecularPalettes[10][256];
static ColorPair PaletteBackup[8][256];
static bool PalettesBackedUp = false;

// Info and gradient stuff

void FillPalettes()
{
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			DiffusePalettes[y][x].color = LSPAL[y][x].diffuse.color;
			SpecularPalettes[y][x].color = LSPAL[y][x].specular.color;
			DiffusePalettes[y][x].argb.a = 255;
			SpecularPalettes[y][x].argb.a = 255;
		}
	}
}

void DrawPalettesCallback(int whatever)
{
	int scale = (int)((float)VerticalResolution / 480.0f);
	float x_left = EnableFontScaling ? scale * 16.0f * 3 : 46.0f;
	float y_top = EnableFontScaling ? scale * 16.0f * 3 : 48.0f;
	float x_gap = 32.0f;
	float y_gap = 24.0f;
	if (scale > 2 && !EnableFontScaling)
		scale = 2;
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			DrawColoredLine(x_left + x, y_top + y_gap * y * scale, x_left + 1 + x, y_top + 16.0f * scale + y_gap * y * scale, DiffusePalettes[y][x]);
			DrawColoredLine(x_left + x + 256 + x_gap, y_top + y_gap * y * scale, x_left + 1 + x + 256 + x_gap, y_top + 16.0f * scale + y_gap * y * scale, SpecularPalettes[y][x]);
		}
	}
}

void DrawPalettes()
{
	late_SetFunc((void(__cdecl*)(void*))DrawPalettesCallback, (void*)-1, 88000.0f, LATE_WZ);
}

void LanternPaletteInfo()
{
	bool noscale = (EnableFontScaling || HorizontalResolution < 1024);
	DrawDebugRectangle(1.75f, 0.75f, 38.0f, noscale ? 23.0f : 37.0f);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(8, 1), "- LANTERN PALETTE INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, noscale ? 19 : 33), "RY : %08d     RZ : %08d", CasinoLightRotation_Y, CasinoLightRotation_Z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, noscale ? 21 : 35), "X: %.4f  Y: %.4f  Z: %.4f", CurrentStageLights[0].direction.x, CurrentStageLights[0].direction.y, CurrentStageLights[0].direction.z);
	FillPalettes();
	DrawPalettes();
}

// Debug palettes

void BackupLanternPalettes()
{
	memcpy(PaletteBackup, LSPAL.data(), sizeof(ColorPair) * 2048);
	PalettesBackedUp = true;
	//PrintDebug("Backed up original lantern palettes\n");
}

void RestoreLanternPalettes()
{
	memcpy(LSPAL.data(), PaletteBackup, sizeof(ColorPair) * 2048);
	PalettesBackedUp = false;
	lig_convHalfBrightPalette(9, 0); // Rebuild atlas
	//PrintDebug("Restored original lantern palettes\n");
}

void LoadPalettes(const char* path)
{
	if (!PalettesBackedUp)
		BackupLanternPalettes();

	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
	{
		PrintDebug("Debug lantern palette not found: %s\n", path);
		return;
	}

	//PrintDebug("Loading debug lantern palette: %s\n", path);

	std::vector<ColorPair> color_data;

	do
	{
		ColorPair pair = {};
		file.read(reinterpret_cast<char*>(&pair.diffuse), sizeof(NJS_COLOR));
		file.read(reinterpret_cast<char*>(&pair.specular), sizeof(NJS_COLOR));
		color_data.push_back(pair);
		file.peek();
	} while (!file.eof());

	file.close();

	memset(LSPAL.data(), 0, sizeof(ColorPair) * 2048);
	memcpy(LSPAL.data(),
		color_data.data(),
		min(sizeof(ColorPair) * color_data.size(), sizeof(ColorPair) * 2048));
	lig_convHalfBrightPalette(9, 0); // Rebuild atlas
}

void SetLanternDebugPalette(LanternDebugPaletteType type)
{
	switch (type)
	{
		case LanternDebugPaletteType::None:
			RestoreLanternPalettes();
			SendDebugMessage("DEBUG PALETTES: OFF");
			break;
		case LanternDebugPaletteType::Selection:
			LoadPalettes(helperFunctionsGlobal->GetReplaceablePath("system\\lantern\\select.bin"));
			SendDebugMessage("DEBUG PALETTES: SELECTION");
			break;
		case LanternDebugPaletteType::SelectionFull:
			LoadPalettes(helperFunctionsGlobal->GetReplaceablePath("system\\lantern\\select2.bin"));
			SendDebugMessage("DEBUG PALETTES: SELECTION FULL");
			break;
		case LanternDebugPaletteType::Direction:
			LoadPalettes(helperFunctionsGlobal->GetReplaceablePath("system\\lantern\\dir.bin"));
			SendDebugMessage("DEBUG PALETTES: DIRECTION");
			break;
		case LanternDebugPaletteType::Index:
			LoadPalettes(helperFunctionsGlobal->GetReplaceablePath("system\\lantern\\index.bin"));
			SendDebugMessage("DEBUG PALETTES: INDEX");
			break;
		case LanternDebugPaletteType::Fullbright:
			LoadPalettes(helperFunctionsGlobal->GetReplaceablePath("system\\lantern\\fullbright.bin"));
			SendDebugMessage("DEBUG PALETTES: FULLBRIGHT");
			break;
	}
}

void LanternDebug_OnFrame()
{
	if (!LanternLoaded || !LanternDebug)
		return;
	bool BackupOk = LSPAL[0][0].diffuse.argb.a == 0xA1 && LSPAL[0][1].diffuse.argb.a == 0xB2 &&
		LSPAL[0][2].diffuse.argb.a == 0xC3 && LSPAL[0][3].diffuse.argb.a == 0xD4;
	if (!BackupOk)
	{
		BackupLanternPalettes();
		LanternDebugMode = (LanternDebugPaletteType)(max((int)LanternDebugMode, 1));
		SetLanternDebugPalette(LanternDebug ? LanternDebugMode : LanternDebugPaletteType::None);
	}
}