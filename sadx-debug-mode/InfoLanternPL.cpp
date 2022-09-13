#include <SADXModLoader.h>
#include "Data.h"

static NJS_COLOR DiffusePalettes[10][256];
static NJS_COLOR SpecularPalettes[10][256];

struct ColorPair
{
	NJS_COLOR diffuse;
	NJS_COLOR specular;
};

DataArray(ColorPair[256], LSPAL, 0x3B12210, 10);

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
	DrawDebugRectangle(1.75f, 0.75f, 38, noscale ? 23 : 37);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(8, 1), "- LANTERN PALETTE INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, noscale ? 19 : 33), "RY : %08X     RZ : %08X", CasinoLightRotation_Y, CasinoLightRotation_Z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, noscale ? 21 : 35), "X: %.4f  Y: %.4f  Z: %.4f", CurrentStageLights[0].direction.x, CurrentStageLights[0].direction.y, CurrentStageLights[0].direction.z);
	FillPalettes();
	DrawPalettes();
}