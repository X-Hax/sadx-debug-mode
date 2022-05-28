#include <SADXModLoader.h>
#include "Data.h"

int CurrentLights[] = { -1, -1, -1, -1, -1, -1 };
int CurrentStageLight = 0;

DataPointer(LE_LIGHT_ENV*, StageLights, 0x40A8A2);

void AddStageLight(int ID)
{
	for (unsigned int q = 0; q < LengthOfArray(CurrentLights); q++)
	{
		if (CurrentLights[q] == ID)
		{
			return;
		}
		if (CurrentLights[q] == -1)
		{
			CurrentLights[q] = ID;
			//PrintDebug("Added stage light: %d\n", ID);
			return;
		}
	}
}

void FindStageLight()
{
	for (unsigned int q = 0; q < LengthOfArray(CurrentLights); q++)
	{
		CurrentLights[q] = -1;
	}
	for (unsigned int i = 0; i < 255; i++)
	{
		if (StageLights[i].stgNo == CurrentLevel && StageLights[i].actNo == CurrentAct)
		{
			//PrintDebug("Adding stage light\n");
			AddStageLight(i);
		}
	}
}

void StageLightInfo()
{
	FindStageLight();
	NJS_COLOR AmbColor;
	ScaleDebugFont(16);
	DrawDebugRectangle(1.75f, 0.75f, 29, 22.5f);
	SetDebugFontColor(GetModuleHandle(L"sadx-dc-lighting") != nullptr ? 0xFFFF0000 : 0xFF88FFAA);
	DisplayDebugString(NJM_LOCATION(5, 1), "- STAGE LIGHTS INFO -");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 3), "INDEX: %d", StageLights[CurrentLights[CurrentStageLight]].ligNo);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 4), "FLAGS: %d", StageLights[CurrentLights[CurrentStageLight]].flgs);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 6), "DIR X: %.3f", StageLights[CurrentLights[CurrentStageLight]].vec.x);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 7), "DIR Y: %.3f", StageLights[CurrentLights[CurrentStageLight]].vec.y);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 8), "DIR Z: %.3f", StageLights[CurrentLights[CurrentStageLight]].vec.z);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 10), "DIFFUSE : %.3f", StageLights[CurrentLights[CurrentStageLight]].dif);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 11), "SPECULAR: %.3f", StageLights[CurrentLights[CurrentStageLight]].spe);
	// Stage Ambient
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].ambR);
	AmbColor.argb.g = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].ambG);
	AmbColor.argb.b = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].ambB);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 13), "AMBIENT");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 14), "R  : %.3f", StageLights[CurrentLights[CurrentStageLight]].ambR);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 15), "G  : %.3f", StageLights[CurrentLights[CurrentStageLight]].ambG);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 16), "B  : %.3f", StageLights[CurrentLights[CurrentStageLight]].ambB);
	// Stage Diffuse
	AmbColor.argb.a = 255;
	AmbColor.argb.r = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].r);
	AmbColor.argb.g = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].g);
	AmbColor.argb.b = int(255.0f * StageLights[CurrentLights[CurrentStageLight]].b);
	SetDebugFontColor(AmbColor.color);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 18), "COLOR");
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 19), "R  : %.3f", StageLights[CurrentLights[CurrentStageLight]].r);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 20), "G  : %.3f", StageLights[CurrentLights[CurrentStageLight]].g);
	DisplayDebugStringFormatted(NJM_LOCATION(3, 21), "B  : %.3f", StageLights[CurrentLights[CurrentStageLight]].b);
}
