#include <SADXModLoader.h>
#include "Data.h"

static NJS_COLOR DiffuseGradient[256];
static NJS_COLOR SpecularGradient[256];

int LSPaletteArray[] = { -1, -1, -1, -1, -1, -1 };
int CurrentPalette = 0;

void DrawPaletteGradientCallback(int diffuse_or_specular)
{
	float y_top;
	float y_bottom;
	NJS_COLOR* palette = diffuse_or_specular == 0 ? DiffuseGradient : SpecularGradient;
	int scale = (int)((float)HorizontalResolution / 640.0f);
	float x_left = ((float)HorizontalResolution - 256.0f * scale) / 2.0f;
	switch (diffuse_or_specular)
	{
	case 0: // Diffuse
		y_top = (float)VerticalResolution - 96.0f;
		y_bottom = y_top + 32;
		break;
	case 1: // Specular
		y_top = (float)VerticalResolution - 48.0f;
		y_bottom = y_top + 32;
		break;
	case -1: // Background
		y_top = (float)VerticalResolution - 120.0f;
		y_bottom = (float)VerticalResolution;
		break;
	}
	if (HorizontalResolution < 1024)
	{
		// Don't draw background at 640x480
		if (diffuse_or_specular == -1)
			return;
		x_left = 200.0f;
		scale = 1;
		y_top = y_top - 200.0f - ((float)VerticalResolution - 480.0f);
		y_bottom = y_top + 32;
	}
	// Draw background
	if (diffuse_or_specular == -1)
	{
		ds_DrawBoxFill2D(0, y_top, (float)HorizontalResolution, y_bottom, -2.0f, 0xA0000000);
	}
	// Draw palettes
	else
	{
		for (int i = 0; i < 256; i++)
		{
			for (int s = 0; s < scale; s++)
				DrawColoredLine(x_left + i * scale - s, y_top, x_left + 1 + i * scale - s, y_bottom, palette[i]);
		}
	}
}

void FillPalette(NJS_COLOR* palette, NJS_COLOR ambient, NJS_COLOR color1, NJS_COLOR color2, float C1_POW, float C2_POW)
{
	for (int i = 0; i < 256; i++)
	{
		palette[i].argb.a = 255;
		palette[i].argb.r = min(255, ambient.argb.r + (Uint8)(color1.argb.r * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.r * pow(1.0f - i / 256.0f, C2_POW)));
		palette[i].argb.g = min(255, ambient.argb.g + (Uint8)(color1.argb.g * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.g * pow(1.0f - i / 256.0f, C2_POW)));
		palette[i].argb.b = min(255, ambient.argb.b + (Uint8)(color1.argb.b * pow(1.0f - i / 256.0f, C1_POW) + color2.argb.b * pow(1.0f - i / 256.0f, C2_POW)));
	}
}

void AddLSPalette(int ID)
{
	for (unsigned int q = 0; q < LengthOfArray(LSPaletteArray); q++)
	{
		if (LSPaletteArray[q] == ID)
		{
			return;
		}
		if (LSPaletteArray[q] == -1)
		{
			LSPaletteArray[q] = ID;
			//PrintDebug("Added palette: %d\n", ID);
			return;
		}
	}
}

void FindLSPalette()
{
	for (unsigned int q = 0; q < LengthOfArray(LSPaletteArray); q++)
	{
		LSPaletteArray[q] = -1;
	}
	for (unsigned int i = 0; i < 255; i++)
	{
		if (LightPaletteData[i].Level == CurrentLevel && LightPaletteData[i].Act == CurrentAct)
		{
			//PrintDebug("Adding palette\n");
			AddLSPalette(i);
		}
	}
}

void LSPaletteInfo()
{
	FindLSPalette();
	NJS_COLOR AmbColor;
	NJS_COLOR CO1Color;
	NJS_COLOR CO2Color;
	NJS_COLOR SP1Color;
	NJS_COLOR SP2Color;
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 29);
	SetDebugFontColor(0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(6, 1), "- LS PALETTE INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "TYPE : %X", LightPaletteData[LSPaletteArray[CurrentPalette]].Type);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "FLAGS: %X", LightPaletteData[LSPaletteArray[CurrentPalette]].Flags);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "DIR X: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].Direction.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "DIR Y: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].Direction.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "DIR Z: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].Direction.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "DIFFUSE: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].DIF);
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_R));
	AmbColor.argb.g = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_G));
	AmbColor.argb.b = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_B));
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 12), "AMBIENT");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "R: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "G: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "B: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].AMB_B);
	// CO1
	CO1Color.argb.a = 255;
	CO1Color.argb.r = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO_R));
	CO1Color.argb.g = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO_G));
	CO1Color.argb.b = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO_B));
	SetDebugFontColor(CO1Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 17), "COLOR1");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "R  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "G  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "B  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO_B);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "POW: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO_pow);
	// SP1
	SP1Color.argb.a = 255;
	SP1Color.argb.r = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP_R));
	SP1Color.argb.g = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP_G));
	SP1Color.argb.b = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP_B));
	SetDebugFontColor(SP1Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 17), "SPECULAR1");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 18), "R  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP_R);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 19), "G  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP_G);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 20), "B  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP_B);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 21), "POW: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP_pow);
	// CO2
	CO2Color.argb.a = 255;
	CO2Color.argb.r = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_R));
	CO2Color.argb.g = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_G));
	CO2Color.argb.b = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_B));
	SetDebugFontColor(CO2Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 23), "COLOR2");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 24), "R  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_R);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 25), "G  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_G);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 26), "B  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_B);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 27), "POW: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_pow);
	// SP2
	SP2Color.argb.a = 255;
	SP2Color.argb.r = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_R));
	SP2Color.argb.g = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_G));
	SP2Color.argb.b = int(255.0f * min(1.0f, LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_B));
	SetDebugFontColor(SP2Color.color);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 23), "SPECULAR2");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 24), "R  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_R);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 25), "G  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_G);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 26), "B  : %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_B);
	DisplayDebugStringFormatted(NJM_LOCATION(17, 27), "POW: %.3f", LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_pow);
	// Generate palette gradients
	FillPalette(DiffuseGradient, AmbColor, CO1Color, CO2Color, LightPaletteData[LSPaletteArray[CurrentPalette]].CO_pow, LightPaletteData[LSPaletteArray[CurrentPalette]].CO2_pow);
	AmbColor.argb.r = 0;
	AmbColor.argb.g = 0;
	AmbColor.argb.b = 0;
	FillPalette(SpecularGradient, AmbColor, SP1Color, SP2Color, LightPaletteData[LSPaletteArray[CurrentPalette]].SP_pow, LightPaletteData[LSPaletteArray[CurrentPalette]].SP2_pow);
	// Draw palette gradients
	late_SetFunc((void(__cdecl*)(void*))DrawPaletteGradientCallback, (void*)-1, 88000.0f, LATE_WZ); // Background
	late_SetFunc((void(__cdecl*)(void*))DrawPaletteGradientCallback, (void*)0, 88000.0f, LATE_WZ); // Diffuse
	late_SetFunc((void(__cdecl*)(void*))DrawPaletteGradientCallback, (void*)1, 88000.0f, LATE_WZ); // Specular
}