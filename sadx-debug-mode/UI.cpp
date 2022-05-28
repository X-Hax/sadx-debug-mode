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