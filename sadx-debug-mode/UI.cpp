#include <SADXModLoader.h>
#include "Data.h"

int DebugMessageTimer = 0;
const char* DebugMessage;

bool EnableFontScaling = false;

static NJS_COLOR DebugFontColorBK;
static float DebugFontSizeBK;

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
	float FontScale = 1.0f;
	if (EnableFontScaling)
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f)
			FontScale = floor((float)VerticalResolution / 480.0f);
		else
			FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	switch (DebugSetting)
	{
	case 6:
	case 7:
		if (EnableFontScaling || HorizontalResolution < 1024)
			DrawRect_Queue(leftchars * FontScale * 10.0f, topchars * FontScale * 10.0f, numchars_horz * FontScale * 10.0f, numchars_vert * FontScale * 10.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
		else
			DrawRect_Queue(leftchars * FontScale * 16.0f, topchars * FontScale * 16.0f, numchars_horz * FontScale * 16.0f, numchars_vert * FontScale * 16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
		break;
	default:
		DrawRect_Queue(leftchars * FontScale * 16.0f, topchars * FontScale * 16.0f, numchars_horz * FontScale * 16.0f, numchars_vert * FontScale * 16.0f, 62041.496f, 0x7F0000FF, QueuedModelFlagsB_EnableZWrite);
		break;
	}
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

void DrawColoredLine(float left, float top, float right, float bottom, NJS_COLOR color)
{
	NJS_POINT2 rectpos[] = { { 0,0 }, { 0,0 },{ 0,0 },{ 0,0 } };
	NJS_COLOR rectcol[] = { 0, 0, 0, 0 };
	NJS_POINT2COL rect = { rectpos, rectcol, NULL, 4 };
	rect.p[0] = { left, top };
	rect.p[1] = { left, bottom };
	rect.p[2] = { right, top };
	rect.p[3] = { right, bottom };
	rect.col[0] = rect.col[1] =
		rect.col[2] = rect.col[3] = color;
	___SAnjDrawPolygon2D(&rect, 4, -1.2f, NJD_TRANSPARENT | NJD_FILL | NJD_DRAW_CONNECTED);
}

void ScaleDebugFont(int scale)
{
	float FontScale = 1.0f;
	if (EnableFontScaling)
	{
		if ((float)HorizontalResolution / (float)VerticalResolution > 1.33f)
			FontScale = floor((float)VerticalResolution / 480.0f);
		else
			FontScale = floor((float)HorizontalResolution / 640.0f);
	}
	SetDebugFontSize((int)(FontScale * scale));
}

void SendDebugMessage(const char* msg)
{
	DebugMessageTimer = 60;
	DebugMessage = msg;
}