#include <SADXModLoader.h>
#include "Data.h"

int CurrentStageLight = 0;

DataArray(LE_LIGHT_ENV, le_env, 0x3ABD9F8, 4); // Current stage lights

bool IsStageLightValid(int id)
{
	if (le_env[id].stgNo != CurrentLevel || le_env[id].actNo != CurrentAct)
		return false;
	return true;
}

void StageLightInfo()
{
	if (!IsStageLightValid(CurrentStageLight))
		CurrentStageLight = 0;
	NJS_COLOR AmbColor = { 0 };
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 22.5f);
	SetDebugFontColor(GetModuleHandle(L"sadx-dc-lighting") != nullptr ? 0xFFFF0000 : 0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(5, 1), "- STAGE LIGHTS INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "INDEX: %d", le_env[CurrentStageLight].ligNo);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "FLAGS: %d", le_env[CurrentStageLight].flgs);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "DIR X: %.3f", le_env[CurrentStageLight].vec.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "DIR Y: %.3f", le_env[CurrentStageLight].vec.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "DIR Z: %.3f", le_env[CurrentStageLight].vec.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "DIFFUSE : %.3f", le_env[CurrentStageLight].dif);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "SPECULAR: %.3f", le_env[CurrentStageLight].spe);
	// Stage Ambient
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * le_env[CurrentStageLight].ambR);
	AmbColor.argb.g = int(255.0f * le_env[CurrentStageLight].ambG);
	AmbColor.argb.b = int(255.0f * le_env[CurrentStageLight].ambB);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "AMBIENT");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "R  : %.3f", le_env[CurrentStageLight].ambR);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "G  : %.3f", le_env[CurrentStageLight].ambG);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "B  : %.3f", le_env[CurrentStageLight].ambB);
	// Stage Diffuse
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * le_env[CurrentStageLight].r);
	AmbColor.argb.g = int(255.0f * le_env[CurrentStageLight].g);
	AmbColor.argb.b = int(255.0f * le_env[CurrentStageLight].b);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "COLOR");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "R  : %.3f", le_env[CurrentStageLight].r);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "G  : %.3f", le_env[CurrentStageLight].g);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "B  : %.3f", le_env[CurrentStageLight].b);
}